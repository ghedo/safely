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

#include <time.h>
#include <errno.h>
#include <regex.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <jansson.h>

#include "db.h"
#include "gpg.h"
#include "item.h"
#include "interface.h"

#define DB_FILE ".safely.db"

static char *db_get_path() {
	char *db_file_name = getenv("SAFELY_DB") != NULL ?
			     strdup(getenv("SAFELY_DB")) : NULL;

	if (db_file_name == NULL) {
		char *home = getenv("HOME");

		db_file_name = malloc(strlen(home) + strlen(DB_FILE) + 2);
		if (db_file_name == NULL) fail_printf("No more memory");

		sprintf(db_file_name, "%s/%s", home, DB_FILE);
	}

	return db_file_name;
}

static char *db_lock_get_path() {
	char *db_file_name, *lock_file_name;

	db_file_name = db_get_path();

	lock_file_name = malloc(strlen(db_file_name) + 5 + 1);
	if (lock_file_name == NULL) fail_printf("No more memory");

	sprintf(lock_file_name, "%s.lock", db_file_name);

	free(db_file_name);

	return lock_file_name;
}

void db_make_backup() {
	FILE *f1, *f2;
	size_t db_size;
	char *db_file_name, *bk_file_name, *buf;
	int dobackup = getenv("SAFELY_NOBACKUP") != NULL ? 0 : 1;

	if (dobackup == 0)
		return;

	db_file_name = db_get_path();

	bk_file_name = malloc(strlen(db_file_name) + 2);
	if (bk_file_name == NULL) fail_printf("No more memory");

	sprintf(bk_file_name, "%s~", db_file_name);

	f1 = fopen(db_file_name, "rb");
	f2 = fopen(bk_file_name, "wb");

	fseek(f1, 0, SEEK_END);
	db_size = ftell(f1);
	fseek(f1, 0, SEEK_SET);

	buf = malloc(db_size);
	if (buf == NULL) fail_printf("No more memory");

	if (fread(buf, db_size, 1, f1) <= 0)
		fail_printf("Error reading db file");

	fwrite(buf, db_size, 1, f2);

	fclose(f1);
	fclose(f2);

	free(db_file_name);
	free(bk_file_name);
	free(buf);
}

void db_acquire_lock() {
	FILE *lock_file;
	char *lock_file_name = db_lock_get_path();

	if (access(lock_file_name, F_OK) == 0)
		fail_printf("Database locked: %s", strerror(errno));

	lock_file = fopen(lock_file_name, "w");
	fclose(lock_file);
	free(lock_file_name);
}

void db_release_lock() {
	char *lock_file_name = db_lock_get_path();

	if (access(lock_file_name, F_OK))
		goto ret;

	if (unlink(lock_file_name) < 0)
		err_printf("Can't remove lock %s: %s", lock_file_name, strerror(errno));

ret:
	free(lock_file_name);
}

db_t *db_create() {
	db_t *db;
	FILE *f;
	char *db_path;

	json_t *root = json_object(),
	       *accounts = json_object();

	db_acquire_lock();

	json_object_set(root, "accounts", accounts);

	db_path = db_get_path();

	umask(066);

	if (access(db_path, F_OK | W_OK) != -1) {
		fail_printf("DB file '%s' already exists", db_path);
	}

	f = fopen(db_path, "w");

	if (f == NULL) fail_printf("Cannot open file '%s'", db_path);
	fclose(f);

	db = (void *) root;

	return db;
}

db_t *db_load() {
	db_t *db;
	char *json, *db_file_name;

	json_t *root;
	json_error_t error;

	db_acquire_lock();

	db_file_name	= db_get_path();
	json		= gpg_decrypt_file(db_file_name);
	root		= json_loads(json, 0, &error);

	free(db_file_name);
	free(json);

	if (!root) fail_printf("JSON error on line %d: %s", error.line, error.text);

	db = (void *) root;

	return db;
}

char *db_dump(db_t *db) {
	json_t *root = (json_t *) db;
	char *dump = json_dumps(root, JSON_PRESERVE_ORDER | JSON_INDENT(4) | JSON_SORT_KEYS);

	return dump;
}

int db_search(db_t *db, const char *pattern) {
	regex_t regex;
	int status, count = 0;

	void *iter;
	const char *key;
	json_t *root = (json_t *) db,
	       *accounts = json_object_get(root, "accounts");

	iter = json_object_iter(accounts);

	regcomp(&regex, pattern, REG_EXTENDED);

	while (iter) {
		key = json_object_iter_key(iter);
		status = regexec(&regex, key, 0, NULL, 0);

		if (status == 0) {
			printf("%s\n", key);
			count++;
		}

		iter = json_object_iter_next(accounts, iter);
	}

	return count;
}

void db_sync(db_t *db) {
	FILE *f;
	json_t *root = (json_t *) db;
	char *db_path, *key_fpr, *cipher, *dump;

	dump = json_dumps(root, JSON_COMPACT);

	key_fpr = gpg_get_keyfpr_first();
	cipher  = gpg_encrypt(dump, key_fpr);

	db_path = db_get_path();
	f = fopen(db_path, "w");
	if (f == NULL) fail_printf("Cannot open file '%s'", db_path);

	fprintf(f, "%s", cipher);
	fclose(f);

	free(dump);
	free(db_path);
	free(key_fpr);
	free(cipher);
}

void db_unload(db_t *db) {
	json_t *root = (json_t *) db;

	json_decref(root);
	db_release_lock();
}
