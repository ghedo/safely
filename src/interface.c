/*
 * Secure and simple command-line password store.
 *
 * Copyright (c) 2011, Alessandro Ghedini
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *     * Neither the name of the Safely project, Alessandro Ghedini, nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "db.h"
#include "interface.h"

#define COLOR_GREEN      "[1;32m"
#define COLOR_YELLOW     "[1;33m"
#define COLOR_RED        "[1;31m"
#define COLOR_OFF        "[0m"

inline void get_input(char str[]) {
	int c, i = 0;

	while ((c = fgetc(stdin)) != '\n' && c != EOF && i < INPUT_MAX_SIZE) {
		str[i++] = c;
	}

	str[i] = '\0';
}

void ok_printf(const char *fmt, ...) {
	va_list args;

	if (!getenv("SAFELY_QUIET")) {
		va_start(args, fmt);
		fprintf(stderr, "[" COLOR_GREEN "ok" COLOR_OFF "] ");
		vfprintf(stderr, fmt, args);
		va_end(args);
		fprintf(stderr, "\n");
	}
}

void debug_printf(const char *fmt, ...) {
#ifndef DEBUG
	va_list args;

	va_start(args, fmt);
	fprintf(stderr,  "[" COLOR_YELLOW "db" COLOR_OFF "] ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
#endif
}

void err_printf(const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	fprintf(stderr,  "[" COLOR_RED "no" COLOR_OFF "] ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
}

void fail_printf(const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	fprintf(stderr,  "[" COLOR_RED "no" COLOR_OFF "] ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");

	/* FIXME: hackish way to not leave db lock around */
	db_release_lock();

	exit(-1);
}
