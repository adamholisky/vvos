#ifndef FTP_INCLUDED
#define FTP_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define FTP_DATA_BUFFER_SIZE 1024 * 20

#define FTP_RESPONSE_SUCCESS 200
#define FTP_RESPONSE_DATA_START 150
#define FTP_RESPONSE_DATA_END 226
#define FTP_RESPONSE_FILE_ACTION_SUCCESS 250
#define FTP_RESPONSE_XPWD_SUCCESS 257
#define FTP_RESPONSE_GIVE_PASS 331
#define FTP_RESPONSE_LOGIN_SUCCESS 230

class FTP {
	private:
		int connection;
		bool serial_hack;
		char user[25];
		char pass[25];
		char dir[256];

		void serial_hack_send_string( char * s );
		char* serial_hack_recv_string( char * buffer );
		char* serial_hack_recv_data( char * buffer );
		bool assert_response( uint32_t code );
		void send_port( uint32_t port );
	public:
		char* buffer;
		char* data_buffer;

		void test( void );
		void init( void );
		void login( char * user, char * pass );
		char* pwd( void );
		void cwd( char * dir );
		void list( void );
		void nlst( void );
		void get_file( char * file_name );
		void send_command( char * cmd );
		void receive_string( void );
		void receive_data( void );
};


#ifdef __cplusplus
}
#endif

#endif