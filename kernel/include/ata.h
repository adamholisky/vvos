#ifndef ATA_HEADER_INCLUDE
#define ATA_HEADER_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

void ata_initalize( void );

#define ata_primary 			0x00
#define ata_secondary 			0x01
#define ata_drive_one 			0x02
#define ata_drive_two 			0x03

#define ata_primary_port		0x1F0
#define ata_secondary_port		0x170

#define ata_primary_control		0x3F6
#define ata_secondary_control	0x376

#define ata_reg_data			0x00
#define ata_reg_error			0x01
#define ata_reg_features		0x01
#define ata_reg_seccount0		0x02
#define ata_reg_lba0			0x03
#define ata_reg_lba1			0x04
#define ata_reg_lba2			0x05
#define ata_reg_hddevsel		0x06
#define ata_reg_command			0x07
#define ata_reg_status			0x07
#define ata_reg_seccount1		0x08
#define ata_reg_lba3			0x09
#define ata_reg_lba4			0x0A
#define ata_reg_lba5			0x0B
#define ata_reg_control			0x0C
#define ata_reg_altstatus		0x0C
#define ata_reg_devaddress		0x0D

#define ata_cmd_read_pio		0x20
#define ata_cmd_read_pio_ext	0x24
#define ata_cmd_read_dma		0xC8
#define ata_cmd_read_dma_ext	0x25
#define ata_cmd_write_pio		0x30
#define ata_cmd_write_pio_ext	0x34
#define ata_cmd_write_dma		0xCA
#define ata_cmd_write_dma_ext	0x35
#define ata_cmd_cache_flush		0xE7
#define ata_cmd_cache_flush_ext	0xEA
#define ata_cmd_packet			0xA0
#define ata_cmd_identify_packet	0xA1
#define ata_cmd_identify		0xEC

#define ata_sr_busy				0x80
#define ata_sr_drdy				0x40
#define ata_sr_df				0x20
#define ata_sr_dsc				0x10
#define ata_sr_drq				0x08
#define ata_sr_corr				0x04
#define ata_sr_idx				0x02
#define ata_sr_err				0x01

#define ata_er_bbk				0x80
#define ata_er_unc				0x40
#define ata_er_mc				0x20
#define ata_er_idnf				0x10
#define ata_er_mcr				0x08
#define ata_er_abrt				0x04
#define ata_er_tk0nf			0x02
#define ata_er_amnf				0x01

#define byte_to_sector( x )		x / 512
#define byte_to_offset( x )     x % 512

// extern "C" void ata_primary_irq( void );
// extern "C" void ata_secondary_irq( void );

int ata_identify( uint8_t drive, uint8_t drive_one_or_two );

#ifdef __cplusplus
}
#endif

#endif