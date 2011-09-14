#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "interface.h"

#define COLOR_GREEN      "[1;32m"
#define COLOR_YELLOW     "[1;33m"
#define COLOR_RED        "[1;31m"
#define COLOR_OFF        "[0m"

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
	fprintf(stderr, "[" COLOR_GREEN "ok" COLOR_OFF "] ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	printf("\n");
}

void debug_printf(const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	fprintf(stderr,  "[" COLOR_YELLOW "db" COLOR_OFF "] ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	printf("\n");
}

void fail_printf(const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	fprintf(stderr,  "[" COLOR_RED "no" COLOR_OFF "] ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	printf("\n");

	exit(-1);
}
