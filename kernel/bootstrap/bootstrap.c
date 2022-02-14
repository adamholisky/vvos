#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "bootstrap.h"

uint64_t __udivmoddi4(uint64_t num, uint64_t den, uint64_t *rem_p)
 {
   uint64_t quot = 0, qbit = 1;
 
   if ( den == 0 ) {
     return 1/((unsigned)den); /* Intentional divide by zero, without
                                  triggering a compiler warning which
                                  would abort the build */
   }
 
   /* Left-justify denominator and count shift */
   while ( (int64_t)den >= 0 ) {
     den <<= 1;
     qbit <<= 1;
   }
 
   while ( qbit ) {
     if ( den <= num ) {
       num -= den;
       quot += qbit;
     }
     den >>= 1;
     qbit >>= 1;
   }
 
   if ( rem_p )
     *rem_p = num;
 
   return quot;
 }

/* int atoi(const char* str) {
	int result = 0;
	while (isspace(*str))
		str++;
	while (isdigit(*str))
		result = (result * 10) + (*(str++) - '0');
	return result;
} */

unsigned long hex2int(char *a, unsigned int len)
{
	int i;
	unsigned long val = 0;

	for(i=0;i<len;i++)
	   if(a[i] <= 57)
		val += (a[i]-48)*(1<<(4*(len-1-i)));
	   else
		val += (a[i]-55)*(1<<(4*(len-1-i)));

	return val;
}