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
 *     * Neither the name of the Securely project, Alessandro Ghedini, nor the
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

#include <jansson.h>

#include "db.h"
#include "gpg.h"
#include "interface.h"

#define GPG_KEY "A4F455C3414B10563FCC9244AFA51BD6CDE573CB"

db_t *db_create() {
	db_t *db;

	json_t *root;
	json_error_t error;

	root = json_object();

	if (!root) fail_printf("JSON error on line %d: %s", error.line, error.text);

	db = (void *) root;

	return db;
}

db_t *db_load(const char *path) {
	db_t *db;

	char *json;
	json_t *root;
	json_error_t error;

	json = gpg_decrypt_file(path);

	root = json_loads(json, 0, &error);
	free(json);

	if (!root) fail_printf("JSON error on line %d: %s", error.line, error.text);

	db = (void *) root;

	return db;
}

const char *db_dump(db_t *db) {
	json_t *root = (json_t *) db;
	const char *dump = json_dumps(root, JSON_PRESERVE_ORDER | JSON_INDENT(4) | JSON_SORT_KEYS);

	return dump;
}

void db_sync(db_t *db, const char *path) {
	FILE *f = fopen(path, "w");
	json_t *root = (json_t *) db;
	const char *dump = json_dumps(root, JSON_COMPACT);
	char *cipher = gpg_encrypt(dump, GPG_KEY);

	if (f == NULL) fail_printf("Cannot open file '%s'", path);

	fprintf(f, "%s\n", cipher);
	fclose(f);

	free((void *) dump);
	free(cipher);
}

void db_unload(db_t *db) {
	json_t *root = (json_t *) db;

	json_decref(root);
}
