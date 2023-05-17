#if !defined(AFS_INCLUDED)
#define AFS_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

	/*
		Drive Byte          Data
		------------------------------------------------------------
		0                   afs_drive
		[index 0]           afs_block_directory for /
		[index ]


	*/
#define MAX_FD 20

#define AFS_VERSION_1 1

#define AFS_BLOCK_TYPE_UNKNOWN 0
#define AFS_BLOCK_TYPE_FILE 1
#define AFS_BLOCK_TYPE_DIRECTORY 2

typedef struct {
	uint32_t	start;			// starting byte of the item
	uint32_t	name_index;		// string table index
} afs_index;

typedef struct {
	uint32_t	next_free;		// next free string table name
	char		string[50][50];
} afs_string_table;

typedef struct {	
	uint32_t 	version;		// version of the drive struct
	uint32_t 	size;			// overall size of the drive, in bytes
	uint32_t	name_index;		// string table index
	uint32_t	root_directory;	// starting byte of the dir struct for root
	uint32_t	next_free;		// next free byte
} afs_drive;

typedef struct {
	uint32_t 	type;			// Type, always AFS_BLOCK_TYPE_FILE
	uint32_t 	block_size;		// size of the block
	uint32_t	file_size;		// size of the actual file data
	uint32_t	name_index;		// string table index
} afs_file;


typedef struct {
	uint32_t 	type;			// Type, always AFS_BLOCK_TYPE_DIRECTORY
    uint32_t	name_index;		// string table index
	afs_index	index[256];		// Block index for things in this directory
	uint32_t	next_index;		// next index free
} afs_block_directory;

typedef struct {
	uint8_t 	type;			// Type of the block
	uint8_t		padding[ sizeof(afs_block_directory) ];	// TODO: Fix this it's dumb
} afs_generic_block;

typedef struct {
	uint32_t	fd;
	uint8_t		*base;
	uint32_t	size;
	uint32_t	position;
	char		*name;
	bool		dirty;
} vv_file;

typedef struct {
	vv_file		fd[MAX_FD];
	char		working_directory[128];
	afs_drive *	drive;
	afs_block_directory * root_dir;
	afs_string_table * string_table;
	uint32_t	next_fd;
} vv_file_internal;

// AFS Specific implementation
uint32_t afs_add_string(vv_file_internal *fs, char *name);
uint32_t afs_exists( vv_file_internal *fs, const char * filename );
uint32_t afs_exists_in_dir( vv_file_internal *fs, afs_block_directory *d, char *name );
afs_file* afs_get_file( vv_file_internal *fs, const char *filename, afs_file *f );
uint32_t afs_get_file_location( vv_file_internal *fs, const char *filename );
uint32_t afs_get_file_location_in_dir( vv_file_internal *fs, afs_block_directory *d, const char *filename );
afs_generic_block* afs_get_generic_block( vv_file_internal *fs, char *filename, afs_generic_block *result_block );
afs_block_directory * afs_get_parent_dir( vv_file_internal *fs, char *name, afs_block_directory *d );

// Syscall implementations
vv_file * afs_open( vv_file_internal *fs, const char * filename, const char * mode );
uint32_t afs_read( vv_file_internal *fs, void *ptr, uint32_t size, vv_file *fp );
void afs_ls(vv_file_internal *fs, char *path);

// Diagnostic routines
void afs_disply_diagnostic_data( uint8_t * buff );
void dump_afs_file( vv_file_internal *fs, afs_file *f );

#ifdef __cplusplus
}
#endif

#endif