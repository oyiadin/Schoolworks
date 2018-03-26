#include <stdio.h>
#include <string.h>
#include "my_malloc.c"

char *f() {
	char *ptr = (char *) malloc(0x10);
	strcpy(ptr, "success!");
	return ptr;
}

int main() {
	char *ret = f();
	char *ptr = (char *) malloc(0x100);
	printf("%p %p\n", ret, ptr);
	free(ptr);
	ptr = (char *) malloc(0x10);
	printf("%p %p", ret, ptr);
	//free(ret);
}
