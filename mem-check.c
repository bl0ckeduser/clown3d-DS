
#include <stdlib.h>
#include <stdio.h>

size_t tot = 0;

void* malloc_check(size_t size){
	tot += size;
	return malloc(size);
}

void memcheck(void){
	printf("%d bytes were malloc'd (not counting frees)\n", tot);
	printf(" = %f KB\n", (float)tot / 1024.0);
}
