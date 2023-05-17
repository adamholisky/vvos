#include <kernel.h>
#include <pci.h>
#include <ahci.h>
#include <stdio.h>

#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier
 
#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATAPI 4
 
#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08

#define HBA_PxIS_TFES   (1 << 30) 
#define ATA_CMD_READ_DMA_EX     0x25
#define ATA_CMD_WRITE_DMA_EX     0x35

#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000

int cmdslots = 0;

typedef enum
{
	FIS_TYPE_REG_H2D	= 0x27,	// Register FIS - host to device
	FIS_TYPE_REG_D2H	= 0x34,	// Register FIS - device to host
	FIS_TYPE_DMA_ACT	= 0x39,	// DMA activate FIS - device to host
	FIS_TYPE_DMA_SETUP	= 0x41,	// DMA setup FIS - bidirectional
	FIS_TYPE_DATA		= 0x46,	// Data FIS - bidirectional
	FIS_TYPE_BIST		= 0x58,	// BIST activate FIS - bidirectional
	FIS_TYPE_PIO_SETUP	= 0x5F,	// PIO setup FIS - device to host
	FIS_TYPE_DEV_BITS	= 0xA1,	// Set device bits FIS - device to host
} FIS_TYPE;


typedef volatile struct tagHBA_PORT
{
	uint32_t clb;		// 0x00, command list base address, 1K-byte aligned
	uint32_t clbu;		// 0x04, command list base address upper 32 bits
	uint32_t fb;		// 0x08, FIS base address, 256-byte aligned
	uint32_t fbu;		// 0x0C, FIS base address upper 32 bits
	uint32_t is;		// 0x10, interrupt status
	uint32_t ie;		// 0x14, interrupt enable
	uint32_t cmd;		// 0x18, command and status
	uint32_t rsv0;		// 0x1C, Reserved
	uint32_t tfd;		// 0x20, task file data
	uint32_t sig;		// 0x24, signature
	uint32_t ssts;		// 0x28, SATA status (SCR0:SStatus)
	uint32_t sctl;		// 0x2C, SATA control (SCR2:SControl)
	uint32_t serr;		// 0x30, SATA error (SCR1:SError)
	uint32_t sact;		// 0x34, SATA active (SCR3:SActive)
	uint32_t ci;		// 0x38, command issue
	uint32_t sntf;		// 0x3C, SATA notification (SCR4:SNotification)
	uint32_t fbs;		// 0x40, FIS-based switch control
	uint32_t rsv1[11];	// 0x44 ~ 0x6F, Reserved
	uint32_t vendor[4];	// 0x70 ~ 0x7F, vendor specific
} __attribute__ ((packed)) HBA_PORT;

typedef volatile struct tagHBA_MEM
{
	// 0x00 - 0x2B, Generic Host Control
	uint32_t cap;		// 0x00, Host capability
	uint32_t ghc;		// 0x04, Global host control
	uint32_t is;		// 0x08, Interrupt status
	uint32_t pi;		// 0x0C, Port implemented
	uint32_t vs;		// 0x10, Version
	uint32_t ccc_ctl;	// 0x14, Command completion coalescing control
	uint32_t ccc_pts;	// 0x18, Command completion coalescing ports
	uint32_t em_loc;		// 0x1C, Enclosure management location
	uint32_t em_ctl;		// 0x20, Enclosure management control
	uint32_t cap2;		// 0x24, Host capabilities extended
	uint32_t bohc;		// 0x28, BIOS/OS handoff control and status
 
	// 0x2C - 0x9F, Reserved
	uint8_t  rsv[0xA0-0x2C];
 
	// 0xA0 - 0xFF, Vendor specific registers
	uint8_t  vendor[0x100-0xA0];
 
	// 0x100 - 0x10FF, Port control registers
	HBA_PORT	ports[1];	// 1 ~ 32
} __attribute__ ((packed)) HBA_MEM;
 
typedef struct tagHBA_CMD_HEADER
{
	// DW0
	uint8_t  cfl:5;		// Command FIS length in DWORDS, 2 ~ 16
	uint8_t  a:1;		// ATAPI
	uint8_t  w:1;		// Write, 1: H2D, 0: D2H
	uint8_t  p:1;		// Prefetchable
 
	uint8_t  r:1;		// Reset
	uint8_t  b:1;		// BIST
	uint8_t  c:1;		// Clear busy upon R_OK
	uint8_t  rsv0:1;		// Reserved
	uint8_t  pmp:4;		// Port multiplier port
 
	uint16_t prdtl;		// Physical region descriptor table length in entries
 
	// DW1
	volatile
	uint32_t prdbc;		// Physical region descriptor byte count transferred
 
	// DW2, 3
	uint32_t ctba;		// Command table descriptor base address
	uint32_t ctbau;		// Command table descriptor base address upper 32 bits
 
	// DW4 - 7
	uint32_t rsv1[4];	// Reserved
} __attribute__ ((packed)) HBA_CMD_HEADER;

typedef struct tagFIS_REG_H2D
{
	// DWORD 0
	uint8_t  fis_type;	// FIS_TYPE_REG_H2D
 
	uint8_t  pmport:4;	// Port multiplier
	uint8_t  rsv0:3;		// Reserved
	uint8_t  c:1;		// 1: Command, 0: Control
 
	uint8_t  command;	// Command register
	uint8_t  featurel;	// Feature register, 7:0
 
	// DWORD 1
	uint8_t  lba0;		// LBA low register, 7:0
	uint8_t  lba1;		// LBA mid register, 15:8
	uint8_t  lba2;		// LBA high register, 23:16
	uint8_t  device;		// Device register
 
	// DWORD 2
	uint8_t  lba3;		// LBA register, 31:24
	uint8_t  lba4;		// LBA register, 39:32
	uint8_t  lba5;		// LBA register, 47:40
	uint8_t  featureh;	// Feature register, 15:8
 
	// DWORD 3
	uint8_t  countl;		// Count register, 7:0
	uint8_t  counth;		// Count register, 15:8
	uint8_t  icc;		// Isochronous command completion
	uint8_t  control;	// Control register
 
	// DWORD 4
	uint8_t  rsv1[4];	// Reserved
} __attribute__ ((packed)) FIS_REG_H2D;

typedef struct tagHBA_PRDT_ENTRY
{
	uint32_t dba;		// Data base address
	uint32_t dbau;		// Data base address upper 32 bits
	uint32_t rsv0;		// Reserved
 
	// DW3
	uint32_t dbc:22;		// Byte count, 4M max
	uint32_t rsv1:9;		// Reserved
	uint32_t i:1;		// Interrupt on completion
} __attribute__ ((packed)) HBA_PRDT_ENTRY;

typedef struct tagHBA_CMD_TBL
{
	// 0x00
	uint8_t  cfis[64];	// Command FIS
 
	// 0x40
	uint8_t  acmd[16];	// ATAPI command, 12 or 16 bytes
 
	// 0x50
	uint8_t  rsv[48];	// Reserved
 
	// 0x80
	HBA_PRDT_ENTRY	prdt_entry[1];	// Physical region descriptor table entries, 0 ~ 65535
} __attribute__ ((packed)) HBA_CMD_TBL;
 


void probe_port(HBA_MEM *abar);
static int check_type(HBA_PORT *port);
int find_cmdslot(HBA_PORT *port);
bool read_ahci(HBA_PORT *port, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf);
void stop_cmd(HBA_PORT *port);
void start_cmd(HBA_PORT *port);
void port_rebase(HBA_PORT *port, int portno,uint32_t addr_base);

extern void asm_refresh_cr3( void );

uint16_t * global_buffer;
uint32_t global_buffer_addr;
uint32_t * global_port_page;
HBA_MEM * abar;

#undef KDEBUG_AHCI_INIT
void ahci_initalize( void ) {
    log_entry_enter();

    pci_header * ahci_drive = pci_get_header_by_device_id(0x2922);

    uint32_t *dev = page_allocate_and_map( (uint32_t *)0xFEA00000 );
	uint32_t *abar_raw = (uint32_t *)((uint32_t)dev + (ahci_drive->bar5 - 0xFEA00000));
    abar = (HBA_MEM *)abar_raw;

    probe_port( abar );

    cmdslots = (abar->cap & 0x0f00)>>8;

    uint32_t *port_page = page_allocate(1);
	page_directory_entry *d = get_page( port_page );
	d->write_through = 1;
	d->cache_disabled = 1;
	asm_refresh_cr3();

	#ifdef KDEBUG_AHCI_INIT
	klog( "port page: %X\n", port_page );
	#endif

    port_rebase( &abar->ports[1], 1, port_page);

	global_port_page = port_page;

    uint16_t *buff = page_allocate(2);
	
	global_buffer = buff;

	page_directory_entry *bp = get_page( buff );
	bp->cache_disabled = 1;
	bp->write_through = 1;
	asm_refresh_cr3();

	#ifdef KDEBUG_AHCI_INIT
	klog( "Buff Page:\n" );
	echo_page( bp );
	#endif 

	global_buffer_addr = bp->address<<21;

	#ifdef KDEBUG_AHCI_INIT
	klog( "Buff: %X\n", (uint32_t *)buff );
	klog( "buff should be: %X\n", global_buffer_addr);
	#endif

    bool read_result = read_ahci( &abar->ports[1], 0,0,16, (uint16_t *)global_buffer_addr );

	#ifdef KDEBUG_AHCI_INIT
    klog( "Read_result: %d\n", read_result );
	#endif

	/* printf( "Buffer: \n" );
	
	int z = 0;
	for( int b = 0; b < 50; b++ ) {
		if (z == 0) {
			printf( "\n%04X    ", b * 2 );
		}
		
		printf( "%02X %02X  ", ( 0x00FF ) & *(buff + b), ((0xFF00) & *(buff + b))>>8 );

		z++;

		if( z == 0x8 ) {
			z = 0;
		}
	} */
 	
    log_entry_exit();
}

bool ahci_read_sector( uint32_t sector, uint32_t *buffer ) {
	bool read_result = false;

	read_result = read_ahci( &abar->ports[1], sector, 0, 1, (uint16_t *)global_buffer_addr );

	if( !read_result ) {
		klog( "Read failed. sector = %X\n", sector );
		return false;
	}

	memcpy( buffer, global_buffer, 512 );

	return true;
}

#undef KDEBUG_AHCI_READ_AT_BYTE_OFFSET
bool ahci_read_at_byte_offset( uint32_t offset, uint32_t size, uint8_t *buffer ) {
	bool read_result = false;
	uint32_t sector = 0;
	uint32_t count = 0;
	uint32_t internal_offset = 0;

	sector = offset / 512;
	internal_offset = offset - (sector * 512);
	count = (size / 512) + 2;

	#ifdef KDEBUG_AHCI_READ_AT_BYTE_OFFSET
	klog( "read: offset -- %X, size %X\n", offset, size );
	klog( "read at offset -- sector = %X, count = %X, internal_offset = %X\n", sector, count, internal_offset );

	memset( global_buffer, 0xDD, 2 * 1024 * 1024 );
	#endif

	read_result = read_ahci( &abar->ports[1], sector, 0, count, (uint16_t *)global_buffer_addr );

	if( !read_result ) {
		klog( "Read failed. sector = %X\n", sector );
		return false;
	}
	
	memcpy( buffer, (uint8_t *)global_buffer + internal_offset, size );

	#ifdef KDEBUG_AHCI_READ_AT_BYTE_OFFSET
	int z = 0;
	for( int b = 0; b < (size / 2); b++ ) {
		if (z == 0) {
			//debugf( "\n%04X    ", b * 2 );
		}
		

		//debugf( "%02X %02X  ", ( 0x00FF ) & *(buffer + b), ((0xFF00) & *(buffer + b))>>8 );

		z++;

		if( z == 0x8 ) {
			z = 0;
		}
	}

	debugf( "\n" );

	//kdebug_peek_at_n(global_buffer, 300 );

	//kdebug_peek_at_n(buffer, 300 );
	#endif

	

	return true;
}

bool ahci_read_at_byte_offset_512_chunks( uint32_t offset, uint32_t size, uint8_t *buffer ) {
	bool read_result = false;
	uint32_t sector = 0;
	uint32_t count = 0;
	uint32_t internal_offset = 0;

	sector = offset / 512;
	internal_offset = offset - (sector * 512);
	count = (size / 512) + 2;

	#ifdef KDEBUG_AHCI_READ_AT_BYTE_OFFSET
	klog( "read: offset -- %X, size %X\n", offset, size );
	klog( "read at offset -- sector = %X, count = %X, internal_offset = %X\n", sector, count, internal_offset );

	memset( global_buffer, 0xDD, 2 * 1024 * 1024 );
	#endif

	for( int i = 0; i < count; i++ ) {
		read_result = read_ahci( &abar->ports[1], sector + i, 0, 1, (uint16_t *)(global_buffer_addr + (i * 512)) );

		if( !read_result ) {
			klog( "Read failed. sector = %X\n", sector );
			return false;
		}
	}

	memcpy( buffer, (uint8_t *)global_buffer + internal_offset, size );

	#ifdef KDEBUG_AHCI_READ_AT_BYTE_OFFSET
	int z = 0;
	for( int b = 0; b < (size / 2); b++ ) {
		if (z == 0) {
			//debugf( "\n%04X    ", b * 2 );
		}
		

		//debugf( "%02X %02X  ", ( 0x00FF ) & *(buffer + b), ((0xFF00) & *(buffer + b))>>8 );

		z++;

		if( z == 0x8 ) {
			z = 0;
		}
	}

	debugf( "\n" );

	//kdebug_peek_at_n(global_buffer, 300 );

	//kdebug_peek_at_n(buffer, 300 );
	#endif

	

	return true;
}


void probe_port(HBA_MEM *abar)
{
	// Search disk in implemented ports
	uint32_t pi = abar->pi;
	int i = 0;
	while (i<32)
	{
		if (pi & 1)
		{
			int dt = check_type(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{
				klog("SATA drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SATAPI)
			{
				klog("SATAPI drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SEMB)
			{
				klog("SEMB drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_PM)
			{
				klog("PM drive found at port %d\n", i);
			}
			else
			{
				klog("No drive found at port %d\n", i);
			}
		}
 
		pi >>= 1;
		i ++;
	}
}
 
// Check device type
static int check_type(HBA_PORT *port)
{
	uint32_t ssts = port->ssts;
 
	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;
 
	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
 
	switch (port->sig)
	{
	case SATA_SIG_ATAPI:
		return AHCI_DEV_SATAPI;
	case SATA_SIG_SEMB:
		return AHCI_DEV_SEMB;
	case SATA_SIG_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}

void port_rebase(HBA_PORT *port, int portno,uint32_t addr_base)
{
	stop_cmd(port);	// Stop command engine
 
	// Command list offset: 1K*portno
	// Command list entry size = 32
	// Command list entry maxim count = 32
	// Command list maxim size = 32*32 = 1K per port
	port->clb = addr_base + (portno<<10) - 0xD0000000;
	port->clbu = 0;
	memset((void*)(port->clb + 0xD0000000), 0, 1024);
 
	// FIS offset: 32K+256*portno
	// FIS entry size = 256 bytes per port
	port->fb = addr_base + (32<<10) + (portno<<8) - 0xD0000000;
	port->fbu = 0;
	memset((void*)(port->fb + 0xD0000000), 0, 256);
 
	// Command table offset: 40K + 8K*portno
	// Command table size = 256*32 = 8K per port
	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb + 0xD0000000 );
	for (int i=0; i<32; i++)
	{
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
					// 256 bytes per command table, 64+16+48+16*8
		// Command table offset: 40K + 8K*portno + cmdheader_index*256
		cmdheader[i].ctba = (addr_base - 0xD0000000) + (40<<10) + (portno<<13) + (i<<8);
		cmdheader[i].ctbau = 0;
		memset((void*)(cmdheader[i].ctba + 0xD0000000), 0, 256);
	}
 
	start_cmd(port);	// Start command engine
}
 
// Start command engine
void start_cmd(HBA_PORT *port)
{
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR)
		;
 
	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 
}
 
// Stop command engine
void stop_cmd(HBA_PORT *port)
{
	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;
 
	// Clear FRE (bit4)
	port->cmd &= ~HBA_PxCMD_FRE;
 
	// Wait until FR (bit14), CR (bit15) are cleared
	while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
 
}

#undef KDEBUG_READ_AHCI
bool read_ahci(HBA_PORT *port, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf)
{
	port->is = (uint32_t) -1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	if (slot == -1)
		return false;

	#ifdef KDEBUG_READ_AHCI
    klog( "clbu: %X\n", port->clbu );
    klog( "clb:  %X\n", port->clb );
	klog( "buf:  %X\n", buf );
	#endif
 
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb + 0xD0000000);
    
	#ifdef KDEBUG_READ_AHCI
	klog( "cmdheader: %X\n", cmdheader );
	#endif

	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;	// PRDT entries count
 
	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba + 0xD0000000);

	#ifdef KDEBUG_READ_AHCI
	klog( "cmdtable: %X\n", cmdtbl );
	#endif

	memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) +
 		(cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));

	// 8K bytes (16 sectors) per PRDT
    int i;
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024-1;	// 8K bytes (this value should always be set to 1 less than the actual value)
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
	cmdtbl->prdt_entry[i].dbc = (count<<9)-1;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;
 
	// Setup command
	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EX;
 
	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
 
	cmdfis->countl = count & 0xFF;
	cmdfis->counth = (count >> 8) & 0xFF;

	
 
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		klog("Port is hung\n");
		return false;
	}
 
	port->ci = 1<<slot;	// Issue command
 
	// Wait for completion
	while (1)
	{
		//printf( "." );
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is & HBA_PxIS_TFES)	// Task file error
		{
			klog("Read disk error\n");
			return false;
		}
	}

	// Check again
	if (port->is & HBA_PxIS_TFES)
	{
		klog("Read disk error\n");
		return false;
	}

	return true;
}
 
// Find a free command list slot
int find_cmdslot(HBA_PORT *port)
{
	// If not set in SACT and CI, the slot is free
	uint32_t slots = (port->sact | port->ci);
	for (int i=0; i<cmdslots; i++)
	{
		if ((slots&1) == 0)
			return i;
		slots >>= 1;
	}
	klog("Cannot find free command list entry\n");
	return -1;
}