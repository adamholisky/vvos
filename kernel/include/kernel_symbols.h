#if !defined(KERNEL_SYMBOLS_INCLUDED)
#define KERNEL_SYMBOLS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define DEBUG_SYMBOLS_MAX 1024

class KernelSymbol {
	public:
		char * 		name;
		uint32_t	addr;
		uint32_t	size;
};

class KernelSymbols {
	private:
		uint32_t kernel_symbol_top;
		KernelSymbol kernel_symbols[ DEBUG_SYMBOLS_MAX ];
		
	public:
		const char * kdebug_symbol_cannot_find = "KDEBUG_cannot_find_symbol";

		void add( char * name, uint32_t addr, uint32_t size );
		KernelSymbol * get_symbol_array( void );
		KernelSymbol * get_symbol( char * name );
		uint32_t get_total_symbols( void );
		uint32_t get_symbol_addr( char * name );
		char * get_function_at( uint32_t addr );
		void initalize( void );
};

#ifdef __cplusplus
}
#endif

#endif