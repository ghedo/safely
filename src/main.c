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

#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>

#include <regex.h>
#include <getopt.h>
#include <signal.h>

#include <jansson.h>

#include "db.h"
#include "item.h"
#include "interface.h"
#include "security.h"

static inline void cmd_create();
static inline void cmd_add(const char *arg);
static inline void cmd_passwd(const char *arg);
static inline void cmd_user(const char *arg);
static inline void cmd_edit(const char *arg);
static inline void cmd_remove(const char *arg);
static inline void cmd_search(const char *arg);
static inline void cmd_dump();
static inline void cmd_version();
static inline void cmd_help();

static void leave(int signal);

enum cmd_t {
	CREATE,
	ADD,
	PASSWD,
	USER,
	EDIT,
	REMOVE,
	SEARCH,
	DUMP,
	TESTS,
	VERSION,
	HELP
};

static struct option long_opts[] = {
	/* options */
	{ "db",		required_argument,	0, 'D' },
	{ "hide",	no_argument,		0, 'H' },
	{ "keys",	required_argument,	0, 'K' },
	{ "quiet",	no_argument,		0, 'Q' },
	{ "nosecure",	no_argument,		0, 'S' },
	{ "nobackup",	no_argument,		0, 'B' },
	{ "noagent",	no_argument,		0, 'A' },

	/* commands */
	{ "create",	no_argument,		0, 'c' },
	{ "add",	required_argument,	0, 'a' },
	{ "passwd",	required_argument,	0, 'p' },
	{ "user",	required_argument,	0, 'u' },
	{ "edit",	required_argument,	0, 'e' },
	{ "remove",	required_argument,	0, 'r' },
	{ "search",	required_argument,	0, 's' },
	{ "dump",	no_argument,		0, 'd' },
	{ "tests",	no_argument,		0, 't' },
	{ "version",	no_argument,		0, 'v' },
	{ "help",	no_argument,		0, 'h' },
	{ 0, 0, 0, 0 }
};

int main(int argc, char *argv[]) {
	int err, opts, i = 0;
	enum cmd_t command = HELP;
	const char *arg = NULL, *gpg_agent_info;

	struct passwd *user;

	/* FIXME: check valid GPG_AGENT_INFO */
	gpg_agent_info	= getenv("GPG_AGENT_INFO");

	/* clear environment */
	if (clearenv()) fail_printf("Cleaning environment failed");

	user		= getpwuid(getuid());

	setenv("HOME", user -> pw_dir, 1);

	if (gpg_agent_info)
		setenv("GPG_AGENT_INFO", gpg_agent_info, 1);

	signal(SIGINT, leave);

	while ((opts = getopt_long(argc, argv, "D:HK:QFSBAca:p:u:e:r:s:dtvh", long_opts, &i)) != -1) {
		switch (opts) {
			case 'D': { setenv("SAFELY_DB", optarg, 1);	break; }
			case 'H': { setenv("SAFELY_HIDE", "y", 1);	break; }
			case 'K': { setenv("SAFELY_KEYS", optarg, 1);	break; }
			case 'Q': { setenv("SAFELY_QUIET", "y", 1);	break; }
			case 'F': { setenv("SAFELY_FUZZY", "y", 1);	break; }
			case 'S': { setenv("SAFELY_NOSECURE", "y", 1);	break; }
			case 'B': { setenv("SAFELY_NOBACKUP", "y", 1);	break; }
			case 'A': { unsetenv("GPG_AGENT_INFO");		break; }

			case 'c': { command = CREATE;	break; }

			case 'a': {
				command = ADD;
				arg = optarg;
				break;
			}

			case 'p': {
				command = PASSWD;
				arg = optarg;
				break;
			}

			case 'u': {
				command = USER;
				arg = optarg;
				break;
			}

			case 'e': {
				command = EDIT;
				arg = optarg;
				break;
			}

			case 'r': {
				command = REMOVE;
				arg = optarg;
				break;
			}

			case 's': {
				command = SEARCH;
				arg = optarg;
				break;
			}

			case 'd': { command = DUMP;	break; }
			case 't': { command = TESTS;	break; }
			case 'v': { command = VERSION;	break; }
			case 'h': { command = HELP;	break; }
		}
	}

	err = try_error;
	if (err) fail_printf(error_str);

	switch (command) {
		case CREATE: {
			err = try_error;

			switch (err) {
				case 0: cmd_create(); break;
				case 2: db_delete();
				case 1: fail_printf(error_str); break;
			}

			break;
		}

		case ADD:	{ cmd_add(arg);		break; }
		case PASSWD:	{ cmd_passwd(arg);	break; }
		case USER:	{ cmd_user(arg);	break; }
		case EDIT:	{ cmd_edit(arg);	break; }
		case REMOVE:	{ cmd_remove(arg);	break; }
		case SEARCH:	{ cmd_search(arg);	break; }
		case DUMP:	{ cmd_dump();		break; }
		case TESTS:	{ security_check();	break; }
		case VERSION:	{ cmd_version();	break; }
		default:
		case HELP:	{ cmd_help();		break; }
	}

	return 0;
}

static void leave(int signal) {
	fprintf(stderr, "\n");
	fail_printf("Received SIGINT. Exiting...");
}

static inline void cmd_create() {
	void *db;

	security_check();

	db = db_create();

	db_sync(db);
	db_unload(db);

	ok_printf("New database created");
}

static inline void cmd_add(const char *arg) {
	void *db;
	char usr[INPUT_MAX_SIZE],
	     pwd[INPUT_MAX_SIZE];

	security_check();
	db_make_backup();

	db = db_load();

	if (item_exist(db, arg) == 0)
		fail_printf("Item '%s' already exists", arg);

	printf("Enter user name for '%s': ", arg);
	get_input(usr);

	if (!strncmp("", usr, 1))
		fail_printf("Empty user field");

	security_echo_off();
	printf("Enter password for '%s': ", arg);
	get_input(pwd);
	security_echo_on();
	putchar('\n');

	if (!strncmp("", pwd, 1))
		fail_printf("Empty password field");

	item_add(db, arg, usr, pwd);

	db_sync(db);
	db_unload(db);

	ok_printf("Added item '%s'", arg);
}

static inline void cmd_passwd(const char *arg) {
	void *db;
	const char *pwd;
	unsigned int hide = getenv("SAFELY_HIDE") != NULL ? 1 : 0;

	security_check();

	db = db_load();

	pwd = item_get_pwd(db, arg);

	if (hide)
		printf(COLOR_BGRED COLOR_RED "%s" COLOR_OFF, pwd);
	else
		printf("%s", pwd);

	fflush(stdout);
	fputc('\n', stderr);

	db_unload(db);
}

static inline void cmd_user(const char *arg) {
	void *db;
	const char *usr;

	security_check();

	db = db_load();

	usr = item_get_usr(db, arg);

	printf("%s", usr);
	fflush(stdout);
	fputc('\n', stderr);

	db_unload(db);
}

static inline void cmd_edit(const char *arg) {
	void *db;
	const char *old_usr, *old_pwd;
	char new_usr[INPUT_MAX_SIZE],
	     new_pwd[INPUT_MAX_SIZE];

	security_check();
	db_make_backup();

	db = db_load();

	if (item_exist(db, arg) != 0)
		fail_printf("Item '%s' does not exist", arg);

	old_usr	= item_get_usr(db, arg);
	old_pwd	= item_get_pwd(db, arg);

	printf("Enter user name for '%s' [%s]: ", arg, old_usr);
	get_input(new_usr);

	security_echo_off();
	printf("Enter password for '%s' [%s]: ", arg, old_pwd);
	get_input(new_pwd);
	security_echo_on();
	putchar('\n');

	item_add(
		db, arg,
		!strncmp("", new_usr, 1) ? old_usr : new_usr,
		!strncmp("", new_pwd, 1) ? old_pwd : new_pwd
	);

	db_sync(db);
	db_unload(db);

	ok_printf("Modified item '%s'", arg);
}

static inline void cmd_remove(const char *arg) {
	void *db;

	security_check();
	db_make_backup();

	db = db_load();

	if (item_exist(db, arg) != 0)
		fail_printf("Item '%s' does not exist", arg);

	item_remove(db, arg);

	db_sync(db);
	db_unload(db);

	ok_printf("Removed item '%s'", arg);
}

static inline void cmd_search(const char *arg) {
	void *db, *iter;
	json_t *matches;
	unsigned int count = 0;

	security_check();

	db	= db_load();
	matches	= db_search(db, arg);

	iter = json_object_iter(matches);

	while (iter) {
		const char *key = json_object_iter_key(iter);

		printf("%s\n", key);
		count++;

		iter = json_object_iter_next(matches, iter);
	}

	db_unload(db);

	ok_printf((count == 1 ? "%d item matches" : "%d items match"), count);
}

static inline void cmd_dump(){
	void *db;
	char *dump;

	security_check();

	db	= db_load();
	dump	= db_dump(db);

	puts(dump);
	free(dump);

	db_unload(db);
}

static inline void cmd_version() {
	printf("Safely v%s\n", SAFELY_VERSION);
}

static inline void cmd_help() {
	#define CMD_HELP(CMDL, CMDS, MSG) printf("  %s, %s      \t%s.\n", COLOR_YELLOW CMDS, CMDL COLOR_OFF, MSG);

	printf(COLOR_RED "Usage: " COLOR_OFF);
	printf(COLOR_GREEN "safely " COLOR_OFF);
	puts("[OPTIONS] COMMAND [ACCOUNT]\n");
	puts(COLOR_RED " Commands:" COLOR_OFF);

	CMD_HELP("--create",	"-c",	"Create a new password database");
	CMD_HELP("--add",	"-a",	"Add the given account");
	CMD_HELP("--passwd",	"-p",	"Show the given account's password");
	CMD_HELP("--user",	"-u",	"Show the given account's username");
	CMD_HELP("--edit",	"-e",	"Edit the given account");
	CMD_HELP("--remove",	"-r",	"Remove the given account");
	CMD_HELP("--search",	"-s",	"Search for the given pattern");
	CMD_HELP("--dump",	"-d",	"Dump JSON database");
	CMD_HELP("--tests",	"-t",	"Run the security tests only");
	CMD_HELP("--version",	"-v",	"Show program's version and exit");
	CMD_HELP("--help",	"-h",	"Show this help and exit");

	puts("");
	puts(COLOR_RED " Options:" COLOR_OFF);

	CMD_HELP("--db",	"-D",	"Specifies a custom path to the password database");
	CMD_HELP("--keys",	"-K",	"Space-separated list of keys to be used");
	CMD_HELP("--hide",	"-H",	"Print the passwords in red on red to avoid shoulder surfing");
	CMD_HELP("--quiet",	"-Q",	"Output errors only (i.e. no \"[ok]\" messages)");
	CMD_HELP("--fuzzy",	"-F",	"Search for non-exact (fuzzy) matches");
	CMD_HELP("--nosecure",	"-S",	"Ignore any security test failure");
	CMD_HELP("--nobackup",	"-B",	"Do not create database file backups");
	CMD_HELP("--noagent",	"-A",	"Do not use the default gpg agent");

	puts("");
}
