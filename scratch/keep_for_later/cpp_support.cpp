#include <kernel.h>
#include <globals.h>
#include <stddef.h>
#include <kmalloc.h>
#include <string.h>

extern "C" void __cxa_pure_virtual() {
   // Intentionally blanks
}

void *operator new(size_t size)
{
	return kmalloc(size);
}
 
void *operator new[](size_t size)
{
	return kmalloc(size);
}
 
void operator delete(void *p)
{
	free(p);
}
 
void operator delete[](void *p)
{
	free(p);
}

bool assert_string_equal( const char * str1, const char * str2 );
bool assert_string_not_equal( const char * str1, const char * str2 );

bool assert_string_equal( const char * str1, const char * str2 ) {
	if( strcmp( str1, str2 ) == 0 ) {
		return true;
	}

	klog( "Assertion failed. %s != %s\n", str1, str2 );

	return false;
}

bool assert_string_not_equal( const char * str1, const char * str2 ) {
	if( strcmp( str1, str2 ) != 0 ) {
		return true;
	}

	klog( "Assertion failed. %s == %s\n", str1, str2 );

	return false;
}

class ClassA {
	private:
		const char *name = "Class A";
	public:
		virtual const char * get_name( void ) = 0;
		virtual void class_func( void ) = 0;
};

class ClassB: public ClassA {
	private:
		const char *name = "Class B";
	public: 
		const char * get_name( void ) {
			return name;
		}

		void class_func( void ) {
			klog( "Called. Name = %s.\n", name );
		}
};

class ClassC: public ClassA {
	private:
		const char *name = "Class C";
	public: 
		const char * get_name( void ) {
			return name;
		}

		void class_func( void ) {
			klog( "Called. Name = %s.\n", name );
		}
};

void cpp_tests( void ) {
	klog( "Beginning CPP tests.\n" );

	ClassA *a;
	ClassB *b = new ClassB();
	ClassC *c = new ClassC();
	
	if( !assert_string_equal( b->get_name(), "Class B" ) ) {
		do_immediate_shutdown();
	}

	a = c;

	if( !assert_string_not_equal( a->get_name(), "Class A" ) ) {
		do_immediate_shutdown();
	}

	b->class_func();
	a->class_func();
	

/* 	klog( "Shutdown now.\n");
	shutdown(); */
}