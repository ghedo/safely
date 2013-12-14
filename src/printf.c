/*
 * Pretty command-line password manager.
 *
 * Copyright (c) 2011-2012, Alessandro Ghedini
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
#include <unistd.h>
#include <setjmp.h>

#include <jansson.h>

#include "db.h"
#include "printf.h"

__thread char *error_str;
__thread jmp_buf error_jmp;

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
		fprintf(stderr, "[" COLOR_GREEN "✔" COLOR_OFF "] ");
		vfprintf(stderr, fmt, args);
		va_end(args);
		fprintf(stderr, "\n");
	}
}

void err_printf(const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	fprintf(stderr,  "[" COLOR_RED "✘" COLOR_OFF "] ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
}

void fail_printf(const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	fprintf(stderr,  "[" COLOR_RED "✘" COLOR_OFF "] ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");

	/* FIXME: hackish way to not leave db lock around */
	db_release_lock();

	exit(-1);
}

void throw_error(int err, const char *fmt, ...) {
	va_list args;

	char *buffer;

	va_start(args, fmt);
	vasprintf(&buffer, fmt, args);
	va_end(args);

	error_str = buffer;

	longjmp(error_jmp, err);
}