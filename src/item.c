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

#include <time.h>

#include <jansson.h>

#include "db.h"
#include "item.h"
#include "printf.h"

#define ITEM_USR_FIELD "user"
#define ITEM_PWD_FIELD "pass"
#define ITEM_DAT_FIELD "date"

void item_add(void *db, const char *item, const char *usr, const char *pwd) {
	char date[22];

	json_t *new, *root = (json_t *) db,
	       *accounts = json_object_get(root, "accounts");

	time_t t = time(NULL);
	struct tm *tmp = localtime(&t);

	strftime(date, 21, "%Y-%m-%d %H:%M:%S", tmp);

	new = json_pack(
		"{ssssss}",
		ITEM_USR_FIELD, usr,
		ITEM_PWD_FIELD, pwd,
		ITEM_DAT_FIELD, date
	);

	json_object_set(accounts, item, new);

	json_decref(new);
}

int item_exist(void *db, const char *item) {
	int check = 1;

	json_t *item_obj,
	       *root = (json_t *) db,
	       *accounts = json_object_get(root, "accounts");

	item_obj = json_object_get(accounts, item);

	if (!json_is_object(item_obj))
		check = -1;
	else
		check = 0;

	return check;
}

const char *item_get_fld(void *db, const char *item, const char *field) {
	json_t *root = (json_t *) db,
	       *item_obj, *field_obj,
	       *accounts = json_object_get(root, "accounts");

	if (getenv("SAFELY_FUZZY")) {
		item_obj = db_search_first(db, item);

		if (!item_obj)
			throw_error(1, "Item '%s' does not exist", item);
	} else {
		if (item_exist(db, item) != 0)
			throw_error(1, "Item '%s' does not exist", item);

		item_obj = json_object_get(accounts, item);
		if (!json_is_object(item_obj))
			throw_error(1, "Invalid object");
	}

	field_obj = json_object_get(item_obj, field);
	if (!json_is_string(field_obj))
		throw_error(1, "Invalid string");

	return json_string_value(field_obj);
}

const char *item_get_usr(void *db, const char *item) {
	return item_get_fld(db, item, ITEM_USR_FIELD);
}

const char *item_get_pwd(void *db, const char *item) {
	return item_get_fld(db, item, ITEM_PWD_FIELD);
}

void item_remove(void *db, const char *item) {
	json_t *root = (json_t *) db,
	       *accounts = json_object_get(root, "accounts");

	if (json_object_del(accounts, item))
		throw_error(1, "Invalid object");
}
