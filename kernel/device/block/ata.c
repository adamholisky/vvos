#include "kernel.h"
#include "ata.h"

#undef kdebug_ata

bool 		primary_ready;

uint16_t	primary_status[256];
uint16_t	* sector_read_buffer;

void ata_initalize( void ) {
	log_entry_enter();
	uint32_t	drive_lba_sectors;
	uint32_t	drive_size_bytes;

	sector_read_buffer = kmalloc( sizeof(uint16_t) * 256 );
	primary_ready = false;
	
	outportb( ata_primary_control, 4 );
	outportb( ata_primary_control, 0 );
	outportb( ata_secondary_control, 4 );
	outportb( ata_secondary_control, 0 );

	klog( "Identify Primary, Drive One:   %s\n", ata_identify( ata_primary, ata_drive_one ) ? "true" : "false"  );
	klog( "Identify Primary, Drive Two:   %s\n", ata_identify( ata_primary, ata_drive_two ) ? "true" : "false" );
	
	drive_lba_sectors = primary_status[60] | primary_status[61] << 16;
	drive_size_bytes = drive_lba_sectors * 512;
	
	#ifdef kdebug_ata
	klog( "Primary 60 and 61:          %X\n", drive_lba_sectors );
	klog( "Drive size:                 %db\n", drive_size_bytes );
	klog( "Drive size:                 %dk\n", drive_size_bytes / 1024 );
	klog( "Drive size:                 %dmb\n", drive_size_bytes / 1024 / 1024 );
	klog( "byte_to_sector( 20 )        %d, %d\n", byte_to_sector(20), byte_to_offset(20) );
	klog( "byte_to_sector( 512 )       %d, %d\n", byte_to_sector(512), byte_to_offset(512) );
	klog( "byte_to_sector( 513 )       %d, %d\n", byte_to_sector(513), byte_to_offset(513) );
	#endif

	log_entry_exit();
}

int ata_identify( uint8_t drive, uint8_t drive_one_or_two ) {
	uint16_t 	port = 0;
	uint8_t		status = 0;
	uint16_t	data = 0;
	int 		result = 0;

	if( drive == ata_primary ) {
		if( drive_one_or_two == ata_drive_one ) {
			outportb( ata_primary_port + ata_reg_hddevsel, 0xA0 );
		} else {
			outportb( ata_primary_port + ata_reg_hddevsel, 0xB0 );
		}
	}

	if( drive == ata_secondary ) {
		if( drive_one_or_two == ata_drive_one ) {
			outportb( ata_secondary_port + ata_reg_hddevsel, 0xA0 );
		} else {
			outportb( ata_secondary_port + ata_reg_hddevsel, 0xB0 );
		}
	}
		
	switch( drive ) {
		case ata_primary: 
			port = ata_primary_port;
			break;
		case ata_secondary:
			port = ata_secondary_port;
			break;
	}

	outportb( port + ata_reg_command, ata_cmd_identify );

	status = inportb( port + ata_reg_status );

	if( status ) {
		result = -2;

		while( inportb( port + ata_reg_status ) & ata_sr_busy != 0 ) {

		}

		do {
			status = inportb( port + ata_reg_status );

			if( status & ata_sr_err ) {
				return 0;
			}
		} while( !(status & ata_sr_busy) && !( status & ata_sr_drq ));


		for( int i = 0; i < 256; i++ ) {
			data = inportw( port + ata_reg_data );
			primary_status[ i ] = data;
		}

		result = 1;
	}

	return result;
}