#include <jansson.h>

#include "db.h"
#include "gpg.h"
#include "interface.h"

#define GPG_KEY "A4F455C3414B10563FCC9244AFA51BD6CDE573CB"

db_t *db_load(const char *path) {
	db_t *db;

	char *data;
	char *json;
	json_t *root;
	json_error_t error;

	int size = 0;
	FILE *f = fopen(path, "rb");

	if (f == NULL) fail_printf("Cannot open file '%s'", path);

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);

	data = (char *) malloc(size + 1);

	if (size != fread(data, sizeof(char), size, f)) {
		free(data);
		return NULL;
	}

	fclose(f);

	json = gpg_decrypt(data);

	root = json_loads(json, 0, &error);
	free(data);
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
	const char *dump = db_dump(db);
	char *cipher = gpg_encrypt(dump, GPG_KEY);

	if (f == NULL) fail_printf("Cannot open file '%s'", path);

	fprintf(f, "%s", cipher);
	fclose(f);

	free((void *) dump);
	free(cipher);
}

void db_unload(db_t *db) {
	json_t *root = (json_t *) db;

	json_decref(root);
}
