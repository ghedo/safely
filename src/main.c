#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <getopt.h>

#include "db.h"
#include "item.h"
#include "interface.h"
#include "security.h"

static inline void cmd_add(db_t *db, const char *arg);
static inline void cmd_show(db_t *db, const char *arg);
static inline void cmd_remove(db_t *db, const char *arg);
static inline void cmd_dump(db_t *db);

static inline char *get_db_path();

static struct option long_options[] = {
	{"add",		required_argument,	0, 'a'},
	{"show",	required_argument,	0, 's'},
	{"remove",	required_argument,	0, 'r'},
	{"list",	no_argument,		0, 'l'},
	{"dump",	no_argument,		0, 'd'},
	{0, 0, 0, 0}
};

#define DB_FILE "safely.db"

int main(int argc, char *argv[]) {
	db_t *db;
	char *db_path;
	int opts, i = 0;

	security_check();

	db_path = get_db_path();
	db = db_load(db_path);

	opts = getopt_long(argc, argv, "asrld", long_options, &i);

	switch (opts) {
		case 'a': { cmd_add(db, optarg);	break; }
		case 's': { cmd_show(db, optarg);	break; }
		case 'r': { cmd_remove(db, optarg);	break; }
		case 'd': { cmd_dump(db);		break; }

		default: { }
	}

	db_sync(db, db_path);
	db_unload(db);

	free(db_path);

	return 0;
}

static inline void cmd_add(db_t *db, const char *arg) {
	char usr[INPUT_MAX_SIZE], pwd[INPUT_MAX_SIZE];

	printf("Enter user name  [%s]: ", arg);
	get_input(usr);

	security_echo_off();
	printf("Enter password   [%s]: ", arg);
	get_input(pwd);
	security_echo_on();
	putchar('\n');

	item_add(db, arg, usr, pwd);
	ok_printf("Added new item");
}

static inline void cmd_show(db_t *db, const char *arg) {
	/* TODO: implement better show command */
	const char *usr = item_get_usr(db, arg);
	const char *pwd = item_get_pwd(db, arg);

	ok_printf("user: %s, password: %s", usr, pwd);
}

static inline void cmd_remove(db_t *db, const char *arg) {
	item_remove(db, arg);
	ok_printf("Removed item");
}

static inline void cmd_dump(db_t *db){
	const char *dump = db_dump(db);

	puts(dump);
	free((void *) dump);
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
