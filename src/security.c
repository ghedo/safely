#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#include <sys/resource.h>

#include "security.h"
#include "interface.h"

static struct termios termset;

static inline void security_check_root();
static inline void security_check_core_dump();
static inline void security_check_memlock();
static inline void security_check_stdinout();

void security_check() {
	security_check_root();
	security_check_core_dump();
	security_check_memlock();
	security_check_stdinout();
}

static inline void security_check_root() {
	const char *msg = "Running as non-root";

	if (geteuid() && getegid() && getuid() && getgid())
		ok_printf(msg);
	else
		fail_printf(msg);
}

static inline void security_check_core_dump() {
	struct rlimit rl;
	const char *msg = "Disabled core dumps";

	if (getrlimit(RLIMIT_CORE, &rl) < 0)
		fail_printf("Can't get RLIMIT_CORE info");

	if (rl.rlim_cur == 0)
		ok_printf(msg);
	else
		fail_printf(msg);
}

static inline void security_check_memlock() {
	/* TODO: finish memlock check */
	struct rlimit rl;
	const char *msg = "memlock";

	if (getrlimit(RLIMIT_MEMLOCK, &rl) < 0)
		fail_printf("Can't get RLIMIT_MEMLOCK info");

	if (rl.rlim_cur == rl.rlim_max)
		ok_printf(msg);
	else
		fail_printf(msg);
}

static inline void security_check_stdinout() {
	int check;
	const char *msg = "Valid stdin, stdout, stderr";

	check = dup(0);
	close(check);

	if (check == 3)
		ok_printf(msg);
	else
		fail_printf(msg);
}

/* TODO: add no-ptrace check */

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
