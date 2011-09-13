#include <jansson.h>

#include "db.h"
#include "item.h"
#include "interface.h"

#define ITEM_USR_FIELD "user"
#define ITEM_PWD_FIELD "pwd"

void item_add(db_t *db, const char *item, const char *usr, const char *pwd) {
	json_t *root = (json_t *) db;

	json_t *new = json_pack(
		"{ssss}",
		ITEM_USR_FIELD, usr,
		ITEM_PWD_FIELD, pwd
	);

	json_object_set(root, item, new);

	json_decref(new);
}

const char *item_get_fld(db_t *db, const char *item, const char *field) {
	json_t *item_obj, *field_obj;
	json_t *root = (json_t *) db;

	item_obj = json_object_get(root, item);

	if (!json_is_object(item_obj)) fail_printf("Not a JSON object");

	field_obj = json_object_get(item_obj, field);

	if (!json_is_string(field_obj)) fail_printf("Not a JSON string");

	return json_string_value(field_obj);
}

const char *item_get_usr(db_t *db, const char *item) {
	return item_get_fld(db, item, ITEM_USR_FIELD);
}

const char *item_get_pwd(db_t *db, const char *item) {
	return item_get_fld(db, item, ITEM_PWD_FIELD);
}

void item_remove(db_t *db, const char *item) {
	json_t *root = (json_t *) db;

	json_object_del(root, item);
}
