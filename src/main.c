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
 *     * Neither the name of the Mongrel2 Project, Zed A. Shaw, nor the names
 *       of its contributors may be used to endorse or promote products
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

#include <getopt.h>

#include "db.h"
#include "item.h"
#include "interface.h"
#include "security.h"

static inline void cmd_create();
static inline void cmd_add(const char *arg);
static inline void cmd_passwd(const char *arg);
static inline void cmd_user(const char *arg);
static inline void cmd_remove(const char *arg);
static inline void cmd_dump();
static inline void cmd_help();

static inline char *get_db_path();

static struct option long_options[] = {
	{"create",	no_argument,		0, 'c'},
	{"add",		required_argument,	0, 'a'},
	{"passwd",	required_argument,	0, 'p'},
	{"user",	required_argument,	0, 'u'},
	{"remove",	required_argument,	0, 'r'},
	{"list",	no_argument,		0, 'l'},
	{"dump",	no_argument,		0, 'd'},
	{"help",	no_argument,		0, 'h'},
	{0, 0, 0, 0}
};

#define DB_FILE "safely.db"

int main(int argc, char *argv[]) {
	int opts, i = 0;

	opts = getopt_long(argc, argv, "capurldh", long_options, &i);

	switch (opts) {
		case 'c': { cmd_create();	break; }
		case 'a': { cmd_add(optarg);	break; }
		case 'p': { cmd_passwd(optarg);	break; }
		case 'u': { cmd_user(optarg);	break; }
		case 'r': { cmd_remove(optarg);	break; }
		case 'd': { cmd_dump();		break; }
		case 'h': { cmd_help();		break; }

		default: { }
	}

	return 0;
}

static inline void cmd_create() {
	FILE *f; db_t *db; char *db_path;

	security_check();

	db_path = get_db_path();

	if (access(db_path, F_OK | W_OK) != -1) {
		fail_printf("DB file '%s' already exists", db_path);
	}

	f = fopen(db_path, "w");

	if (f == NULL) fail_printf("Cannot open file '%s'", db_path);
	fclose(f);

	db = db_create();

	db_sync(db, db_path);
	db_unload(db);

	free(db_path);
}

static inline void cmd_add(const char *arg) {
	db_t *db; char *db_path;
	char usr[INPUT_MAX_SIZE], pwd[INPUT_MAX_SIZE];

	security_check();

	db_path = get_db_path();
	db = db_load(db_path);

	printf("Enter user name  [%s]: ", arg);
	get_input(usr);

	security_echo_off();
	printf("Enter password   [%s]: ", arg);
	get_input(pwd);
	security_echo_on();
	putchar('\n');

	item_add(db, arg, usr, pwd);
	ok_printf("Added new item");

	db_sync(db, db_path);
	db_unload(db);

	free(db_path);
}

static inline void cmd_passwd(const char *arg) {
	const char *pwd;
	db_t *db; char *db_path;

	security_check();

	db_path = get_db_path();
	db = db_load(db_path);

	pwd = item_get_pwd(db, arg);

	puts(pwd);

	db_sync(db, db_path);
	db_unload(db);

	free(db_path);
}

static inline void cmd_user(const char *arg) {
	const char *usr;
	db_t *db; char *db_path;

	security_check();

	db_path = get_db_path();
	db = db_load(db_path);

	usr = item_get_usr(db, arg);

	puts(usr);

	db_sync(db, db_path);
	db_unload(db);

	free(db_path);
}

static inline void cmd_remove(const char *arg) {
	db_t *db; char *db_path;

	security_check();

	db_path = get_db_path();
	db = db_load(db_path);

	item_remove(db, arg);
	ok_printf("Removed item");

	db_sync(db, db_path);
	db_unload(db);

	free(db_path);
}

static inline void cmd_dump(){
	const char *dump;
	db_t *db; char *db_path;

	security_check();

	db_path = get_db_path();
	db = db_load(db_path);

	dump = db_dump(db);

	puts(dump);
	free((void *) dump);

	db_sync(db, db_path);
	db_unload(db);

	free(db_path);
}

static inline void cmd_help() {
	#define CMD_HELP(CMDL, MSG) printf("\t%s   \t%s.\n", CMDL, MSG);

	printf("  Usage:\n\tsafely [COMMAND] args...\n\n");
	printf("  Commands:\n");

	CMD_HELP("--create",	"Create a new password db");
	CMD_HELP("--add",	"Add a new account");
	CMD_HELP("--passwd",	"Show given account's password");
	CMD_HELP("--user",	"Show given account's username");
	CMD_HELP("--remove",	"Remove given account");
	CMD_HELP("--dump",	"Dump JSON database");
	CMD_HELP("--help",	"Show this help");
}

static inline char *get_db_path() {
	char *path = getenv("SAFELY_DB") != NULL ? strdup(getenv("SAFELY_DB")) : NULL;

	if (path == NULL) {
		char *home = getenv("HOME");

		path = (char *) malloc((strlen(home) + strlen(DB_FILE) + 1) * sizeof(char));
		sprintf(path, "%s/%s", home, DB_FILE);
	}

	return path;
}
