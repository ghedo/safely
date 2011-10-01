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

#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#include <sys/mman.h>
#include <sys/resource.h>

#include "security.h"
#include "interface.h"

static struct termios termset;

static inline int security_check_root();
static inline int security_check_core_dump();
static inline int security_check_memlock();
static inline int security_check_ptrace();
static inline int security_check_stdinout();

#define MAX_SCORE 5

void security_check() {
#ifndef DEBUG
	int score = 0;
	int pedantic = getenv("SAFELY_NOSECURE") != NULL ? 0 : 1;

	score += security_check_root();
	score += security_check_core_dump();
	score += security_check_memlock();
	score += security_check_ptrace();
	score += security_check_stdinout();

	if (score < MAX_SCORE) {
		if (pedantic == 1) {
			fail_printf("%d/%d security tests failed", (MAX_SCORE - score), MAX_SCORE);
		} else {
			err_printf("%d/%d security tests failed", (MAX_SCORE - score), MAX_SCORE);
		}
	}
#endif
}

static inline int security_check_root() {
	const char *msg = "Running as non-root";

	if (getuid() && getgid()) {
		ok_printf(msg);
		return 1;
	} else {
		err_printf(msg);
		return 0;
	}
}

static inline int security_check_core_dump() {
	struct rlimit rl;
	const char *msg = "Disabled core dumps";

	if (getrlimit(RLIMIT_CORE, &rl) < 0)
		fail_printf("Can't get RLIMIT_CORE info");

	if (rl.rlim_cur == 0) {
		ok_printf(msg);
		return 1;
	} else {
		err_printf(msg);
		return 0;
	}
}

static inline int security_check_memlock() {
	struct rlimit rl;
	const char *msg = "Memory locked";

	if (getrlimit(RLIMIT_MEMLOCK, &rl) < 0)
		err_printf("Can't get RLIMIT_MEMLOCK info");

	if (!geteuid())
		mlockall(MCL_CURRENT | MCL_FUTURE);


	if (rl.rlim_cur == rl.rlim_max) {
		ok_printf(msg);
		return 1;
	} else {
		err_printf(msg);
		return 0;
	}
}

static inline int security_check_ptrace() {
	int check = 0;
	const char *msg = "Protection from ptrace()";

	if (!geteuid()) {
		/* drop root priviledges */
		setuid(getuid());	setuid(getuid());
		setgid(getgid());	setgid(getgid());

		check = 1;
	}

	if (getuid() && !setuid(0))
		check = 0;

	if (check == 1) {
		ok_printf(msg);
		return 1;
	} else {
		err_printf(msg);
		return 0;
	}
}

static inline int security_check_stdinout() {
	int check;
	const char *msg = "Valid stdin, stdout, stderr";

	check = dup(0);
	close(check);

	if (check == 3) {
		ok_printf(msg);
		return 1;
	} else {
		err_printf(msg);
		return 0;
	}
}

void security_echo_on() {
	tcsetattr(STDIN_FILENO, TCSANOW, &termset);
}

void security_echo_off() {
	static struct termios newtermset;

	tcgetattr(STDIN_FILENO, &termset);
	newtermset = termset;
	newtermset.c_lflag &= ~(ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newtermset);
}
