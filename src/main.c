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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <regex.h>
#include <getopt.h>
#include <signal.h>

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
static inline void cmd_help();

static void leave(int signal);

static struct option long_options[] = {
	{"create",	no_argument,		0, 'c'},
	{"add",		required_argument,	0, 'a'},
	{"passwd",	required_argument,	0, 'p'},
	{"user",	required_argument,	0, 'u'},
	{"edit",	required_argument,	0, 'e'},
	{"remove",	required_argument,	0, 'r'},
	{"search",	required_argument,	0, 's'},
	{"dump",	no_argument,		0, 'd'},
	{"help",	no_argument,		0, 'h'},
	{0, 0, 0, 0}
};

int main(int argc, char *argv[]) {
	int opts, i = 0;

	signal(SIGINT, leave);

	opts = getopt_long(argc, argv, "ca:p:u:e:r:s:dh", long_options, &i);

	switch (opts) {
		case 'c': { cmd_create();	break; }
		case 'a': { cmd_add(optarg);	break; }
		case 'p': { cmd_passwd(optarg);	break; }
		case 'u': { cmd_user(optarg);	break; }
		case 'e': { cmd_edit(optarg);	break; }
		case 'r': { cmd_remove(optarg);	break; }
		case 's': { cmd_search(optarg);	break; }
		case 'd': { cmd_dump();		break; }
		default:
		case 'h': { cmd_help();		break; }
	}

	return 0;
}

static void leave(int signal) {
	fprintf(stderr, "\n");
	fail_printf("Received SIGINT. Exiting...");
}

static inline void cmd_create() {
	db_t *db;

	security_check();

	db = db_create();

	db_sync(db);
	db_unload(db);

	ok_printf("New database created");
}

static inline void cmd_add(const char *arg) {
	db_t *db;
	char usr[INPUT_MAX_SIZE], pwd[INPUT_MAX_SIZE];

	security_check();
	db_make_backup();

	db = db_load();

	printf("Enter user name  [%s]: ", arg);
	get_input(usr);

	security_echo_off();
	printf("Enter password   [%s]: ", arg);
	get_input(pwd);
	security_echo_on();
	putchar('\n');

	item_add(db, arg, usr, pwd);

	memset(usr, 0, INPUT_MAX_SIZE);
	memset(pwd, 0, INPUT_MAX_SIZE);

	db_sync(db);
	db_unload(db);

	ok_printf("Added item '%s'", arg);
}

static inline void cmd_passwd(const char *arg) {
	db_t *db;
	const char *pwd;

	security_check();

	db	= db_load();
	pwd	= item_get_pwd(db, arg);

	puts(pwd);

	db_unload(db);
}

static inline void cmd_user(const char *arg) {
	db_t *db;
	const char *usr;

	security_check();

	db	= db_load();
	usr	= item_get_usr(db, arg);

	puts(usr);

	db_unload(db);
}

static inline void cmd_edit(const char *arg) {
	db_t *db;
	const char *old_usr, *old_pwd;
	char new_usr[INPUT_MAX_SIZE], new_pwd[INPUT_MAX_SIZE];

	security_check();
	db_make_backup();

	db	= db_load();

	old_usr	= item_get_usr(db, arg);
	old_pwd	= item_get_pwd(db, arg);

	printf("Enter user name  [%s] (default: %s): ", arg, old_usr);
	get_input(new_usr);

	security_echo_off();
	printf("Enter password   [%s] (default: %s): ", arg, old_pwd);
	get_input(new_pwd);
	security_echo_on();
	putchar('\n');

	item_add(
		db, arg,
		!strncmp("", new_usr, 1) ? old_usr : new_usr,
		!strncmp("", new_pwd, 1) ? old_pwd : new_pwd
	);

	memset(new_usr, 0, INPUT_MAX_SIZE);
	memset(new_pwd, 0, INPUT_MAX_SIZE);

	db_sync(db);
	db_unload(db);

	ok_printf("Modified item '%s'", arg);

	db_unload(db);
}

static inline void cmd_remove(const char *arg) {
	db_t *db;

	security_check();
	db_make_backup();

	db = db_load();

	item_remove(db, arg);

	db_sync(db);
	db_unload(db);

	ok_printf("Removed item '%s'", arg);
}

static inline void cmd_search(const char *arg) {
	db_t *db;
	unsigned int count = 0;

	security_check();

	db	= db_load();
	count	= db_search(db, arg);

	db_unload(db);

	ok_printf((count == 1 ? "%d item matches" : "%d items match"), count);
}

static inline void cmd_dump(){
	db_t *db;
	char *dump;

	security_check();

	db	= db_load();
	dump	= db_dump(db);

	puts(dump);
	free(dump);

	db_unload(db);
}

static inline void cmd_help() {
	#define CMD_HELP(CMDL, CMDS, MSG) printf("\t%s, %s\t%s.\n", CMDL, CMDS, MSG);

	printf("  Usage:\n\tsafely [COMMAND] args...\n\n");
	printf("  Commands:\n");

	CMD_HELP("--create", "-c",	"Create a new password db");
	CMD_HELP("--add", "-a",		"Add a new account");
	CMD_HELP("--passwd", "-p",	"Show given account's password");
	CMD_HELP("--user", "-u",	"Show given account's username");
	CMD_HELP("--edit", "-e",	"Edit the given account");
	CMD_HELP("--remove", "-r",	"Remove given account");
	CMD_HELP("--search", "-s",	"Search the given pattern");
	CMD_HELP("--dump", "-d",	"Dump JSON database");
	CMD_HELP("--help", "-h",	"Show this help");
}
