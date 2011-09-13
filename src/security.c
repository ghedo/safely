#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#include "security.h"

static struct termios termset;

/* TODO: security checks (e.g. no root, no ptrace, no core dumps, ...) */

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
