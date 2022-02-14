#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define D1 	printf( "------------1\n" );
#define D2	printf( "------------2\n" );
#define D3	printf( "------------3\n" );

typedef char *token_ptr;

char *test_script = "extern printf \n\
printf \"Hello world!\"\n\
a\n\
\n";

class VVScript {
	private:
		char *script;
		int	len;

		int find( char * source, char * text );
		bool is_reserved_word( char *token );
		bool is_function_call( char *token );
		bool is_variable_assignment( char *token );
	public:
		VVScript( char * user_script );
		void run( void );
};

VVScript::VVScript( char * user_script ) {
	int len = strlen( user_script );

	this->script = (char *)malloc( len );
	strcpy( this->script, user_script );
}

int VVScript::find( char * source, char * text ) {
	char * result = strstr( source, text );
	return result - source;
}

bool VVScript::is_reserved_word( char *token ) {
	return true;
}

bool VVScript::is_function_call( char *token ) {
	return false;
}

bool VVScript::is_variable_assignment( char *token ) {
	return false;
}

void VVScript::run( void ) {
	bool keep_running = true;
	int start_position = 0;
	char * s = this->script;
	int orig_len = strlen( this->script );
	int x = 0;

	while( keep_running ) {
		char * newline_at = strchr( s, '\n' );
		
		if( newline_at != NULL ) {
			int newline_pos = newline_at - s;
			int line_length = newline_pos;
			char * line = (char *)malloc( line_length );
			strncpy( line, s, line_length );

			char *trim_whitespaces = line;
			int trim_count = line_length;
			trim_whitespaces += line_length;
			trim_whitespaces--;

			while( trim_count != 0 ) {
				if( *trim_whitespaces == ' ' ) {
					*trim_whitespaces = '\0';
					*trim_whitespaces--;
					trim_count--;
				} else {
					trim_count = 0;
				}
			}
			
			printf( "Line: \"%s\"\n", line );
			printf( "  Newline pos: %d\n", newline_pos );
			int potential_tokens = 0;
			char * space_counter = line;
			
			if( *space_counter != '\0' ) {
				potential_tokens++;
			}

			while( *space_counter ) {
				if( *space_counter == ' ' ) {
					potential_tokens++;
				}

				space_counter++;
			}

			printf( "  Potential tokens: %d\n", potential_tokens );

			token_ptr *tokens = (token_ptr *)malloc( sizeof(token_ptr) * potential_tokens );
			char * token_itr = line;
			int i = 0;
			bool keep_tokenizing = true;

			do {
				tokens[i] = token_itr;
				bool in_string = false;

				while( *token_itr ) {
					if( *token_itr == ' ' ) {
						if( !in_string ) {
							*token_itr = '\0';
							*token_itr++;
							break;
						}
					} else if( *token_itr == '"' ) {
						if( in_string ) {
							in_string = false;
						} else {
							in_string = true;
						}
					}

					*token_itr++;
				}

				if( *token_itr == '\0' ) {
					keep_tokenizing = false;
				}

				i++;

				if( i == 5 ) {
					keep_tokenizing = false;
				}
			} while( keep_tokenizing );

			printf( "  Actual tokens: %d\n", i );
			for( int j = 0; j < i; j++ ) {
				printf( "    tokens[%d]: %s\n", j, tokens[j] );
			}

			// Process tokens
			int token_pos = 0;

			if( this->is_reserved_word( tokens[token_pos] ) ) {
				printf( "      Found reserved word.\n" );
			} else if( this->is_function_call( tokens[token_pos] ) ) {
				// do stuff
			} else if( this->is_variable_assignment( tokens[token_pos] ) ) {
				// do stuff
			}
			
		}
		
		s = newline_at;
		s++;

		if( s >= ( this->script + orig_len ) ) {
			printf( "    Stop processing\n" );
			keep_running = false;
		} else {
			printf( "    Continue processing\n" );
		}

		if( x == 5 ) {
			keep_running = false;
		}

		x++;
	}
}

int main( int argc, const char *argv[] ) {
	VVScript * my_script = new VVScript( test_script );

	my_script->run();

	exit(0);
}

