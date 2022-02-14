#include <string.h>
#include <ftp.h>
#include <serial.h>
#include <printf.h>
#include <serial_client.h>

void FTP::test( void ) {
	this->login( "vv", "vv" );

	this->cwd( "/usr/local/osdev/versions/v" );

	this->pwd();
	debugf( "%s\n------------------------------\n", this->dir );

	/* this->list();
	debugf( "%s\n", this->data_buffer ); */
	
	this->get_file( "Makefile" );
	debugf( "Makefile (%d):\n", strlen( this->data_buffer ) );
	
	for( int x = 0; x < strlen( this->data_buffer ); x++ ) {
		//debugf( "%c", this->data_buffer[x] );
	}
}

void FTP::init( void ) {
	this->serial_hack = true;

	this->buffer = (char *)kmalloc( 1024 );
	this->data_buffer = (char *)kmalloc( FTP_DATA_BUFFER_SIZE );

	this->receive_string();
}

void FTP::login( char * user, char * pass ) {
	char temp[30];

	strcpy( this->user, user );
	strcpy( this->pass, pass );
	
	memset( (void *)temp, 0, 30 );
	strcat( temp, "USER " );
	strcat( temp, this->user );

	this->send_command( temp );
	this->assert_response( FTP_RESPONSE_GIVE_PASS );

	memset( (void *)temp, 0, 30 );
	strcat( temp, "PASS " );
	strcat( temp, this->pass );

	this->send_command( temp );
	this->assert_response( FTP_RESPONSE_LOGIN_SUCCESS );
}

char* FTP::pwd( void ) {
	char temp[256];

	// https://www.rfc-editor.org/rfc/rfc775.html
	this->send_command( "XPWD" );
	this->assert_response( FTP_RESPONSE_XPWD_SUCCESS );
	
	strcpy( temp, (this->buffer + 5) );

	*(strchr( temp, '"' )) = '\0';

	strcpy( this->dir, temp );

	return this->dir;
}

void FTP::cwd( char * dir ) {
	char temp[256];

	memset( (void *)temp, 0, 256 );
	strcat( temp, "CWD " );
	strcat( temp, dir );

	this->send_command( temp );
	this->assert_response( FTP_RESPONSE_FILE_ACTION_SUCCESS );
}

void FTP::list( void ) {
	this->send_port( 6699 );

	this->send_command( "LIST" );
	this->assert_response( FTP_RESPONSE_DATA_START );
	this->assert_response( FTP_RESPONSE_DATA_END );

	this->receive_data();
}

void FTP::nlst( void ) {
	this->send_port( 6699 );

	this->send_command( "NLST" );
	this->assert_response( FTP_RESPONSE_DATA_START );
	this->assert_response( FTP_RESPONSE_DATA_END );

	this->receive_data();
}

void FTP::get_file( char * file_name ) {
	char temp[125];

	this->send_port( 6699 );

	memset( (void *)temp, 0, 125 );
	strcat( temp, "RETR " );
	strcat( temp, file_name );

	this->send_command( temp );
	this->assert_response( FTP_RESPONSE_DATA_START );
	this->assert_response( FTP_RESPONSE_DATA_END );

	this->receive_data();
}

void FTP::receive_data( void ) {
	memset( (void *)this->data_buffer, 0, FTP_DATA_BUFFER_SIZE );

	if( this->serial_hack ) {
		this->data_buffer = this->serial_hack_recv_data( this->data_buffer );
	}
}

bool FTP::assert_response( uint32_t code ) {
	char code_in_str[4];
	uint32_t code_in_num = 0;
	bool ret_val = false;

	this->receive_string();

	code_in_str[0] = this->buffer[0];
	code_in_str[1] = this->buffer[1];
	code_in_str[2] = this->buffer[2];
	code_in_str[3] = '\0';

	code_in_num = atoi( code_in_str );

	if( code_in_num == code ) {
		//klog( "Pass. Response: \"%s\"\n", this->buffer );
		ret_val = true;
	} else {
		klog( "Expected code %d, got %d. Response: \"%s\"\n", code, code_in_num, this->buffer );
	}

	return ret_val;
}

void FTP::send_port( uint32_t port ) {
	this->send_command( "PORT 192,168,0,100,26,43" );
	this->assert_response( FTP_RESPONSE_SUCCESS );
}

void FTP::send_command( char * cmd ) {
	char s[256];

	memset( (void *)s, 0, 256 );
	strcpy( s, cmd );
	strcat( s, "\n" );

	if( this->serial_hack ) {
		this->serial_hack_send_string( s );
	}

	//debugf( "Sent: \"%s\"\n", cmd );
}

void FTP::receive_string( void ) {
	memset( (void *)this->buffer, 0, 1024 );

	if( this->serial_hack ) {
		this->serial_hack_recv_string( buffer );
	}
}

void FTP::serial_hack_send_string( char * s ) {
	int len = strlen( s );

	//debugf( "\nlen: %d\n", len );
	for( int i = 0; i < len; i++ ) {
		serial_write_port( s[i], COM1 );
	}
}

char * FTP::serial_hack_recv_string( char * buffer ) {
	bool cont = true;
	bool rn_part_1 = false;
	bool process_char = true;
	int buff_loc = 0;
	char c = '\0';

	while( cont ) {
		process_char = true;
		
		while( serial_com1_buffer_read_loc == serial_com1_buffer_add_loc ) {
			;
		}

		c = serial_in_com1_buffer[ serial_com1_buffer_read_loc ];
		serial_com1_buffer_read_loc++;

		if( serial_com1_buffer_read_loc == 1024 ) {
			serial_com1_buffer_read_loc = 0;
		}
		
		if( c == '\n' ) {
			if( rn_part_1 ) {
				// done, return the string
				cont = false;
				process_char = false;
			}
		} else if( c == '\r' ) {
			rn_part_1 = true;
			process_char = false;
		}
		
		if( process_char ) {
			buffer[ buff_loc ] = c;
			buff_loc++;

			if( buff_loc == 256 ) {
				cont = false;
			}
		}
	}
	
	return buffer;
}

char * FTP::serial_hack_recv_data( char * buffer ) {
	bool cont = true;
	bool rn_part_1 = false;
	bool process_char = true;
	int buff_loc = 0;
	char c = '\0';

	while( cont ) {
		process_char = true;

		c = serial_in_com3_buffer[ serial_com3_buffer_read_loc ];
		serial_com3_buffer_read_loc++;

		if( serial_com3_buffer_read_loc == FTP_DATA_BUFFER_SIZE ) {
			serial_com3_buffer_read_loc = 0;
		}

		if( serial_com3_buffer_read_loc == serial_com3_buffer_add_loc ) {
			cont = false;
		}

		buffer[ buff_loc ] = c;
		buff_loc++;

		if( buff_loc == FTP_DATA_BUFFER_SIZE ) {
			cont = false;
		}

		//debugf( ". %c\n", buffer[ buff_loc - 1 ] );
	}
	
	return buffer;
}