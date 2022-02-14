#include <printf.h>
#include <string.h>
#include <stdbool.h>
#include "debug.h"
#include "syscall.h"

void main( void ) {
	kpanic( "Panic and run! %s\n", "Panic AND run!");
}