#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "interface.h"

/* TODO: colored output */

inline void get_input(char str[]) {
	int c, i = 0;

	while ((c = getchar())!= '\n' && c != EOF && i < INPUT_MAX_SIZE){
		str[i++] = c;
	}

	str[i] = '\0';
}

void ok_printf(const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "[ok] ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	printf("\n");
}

void fail_printf(const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "[no] ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	printf("\n");

	exit(-1);
}
