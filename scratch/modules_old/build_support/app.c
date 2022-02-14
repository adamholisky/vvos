extern void main( void );

void __start( void ) {
	// setup?

	main();

	asm	volatile ( 
				"movl %0, %%eax \n"
				"int %1 \n"
				:
				:"i"(0x6), "i"(0x99)
				:"%eax" 
			);
}