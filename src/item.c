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

#include <time.h>

#include <jansson.h>

#include "db.h"
#include "item.h"
#include "interface.h"

#define ITEM_USR_FIELD "user"
#define ITEM_PWD_FIELD "pass"
#define ITEM_DAT_FIELD "date"

void item_add(db_t *db, const char *item, const char *usr, const char *pwd) {
	char *date = malloc(19);
	json_t *new, *root = (json_t *) db,
	       *accounts = json_object_get(root, "accounts");

	time_t t = time(NULL);
	struct tm *tmp = localtime(&t);

	strftime(date, 18, "%Y-%m-%d %H:%M", tmp);

	new = json_pack(
		"{ssssss}",
		ITEM_USR_FIELD, usr,
		ITEM_PWD_FIELD, pwd,
		ITEM_DAT_FIELD, date
	);

	json_object_set(accounts, item, new);

	json_decref(new);
	free(date);
}

const char *item_get_fld(db_t *db, const char *item, const char *field) {
	json_t *item_obj, *field_obj;
	json_t *root = (json_t *) db,
	       *accounts = json_object_get(root, "accounts");

	item_obj = json_object_get(accounts, item);
	if (!json_is_object(item_obj)) fail_printf("Invalid object");

	field_obj = json_object_get(item_obj, field);
	if (!json_is_string(field_obj)) fail_printf("Invalid string");

	return json_string_value(field_obj);
}

const char *item_get_usr(db_t *db, const char *item) {
	return item_get_fld(db, item, ITEM_USR_FIELD);
}

const char *item_get_pwd(db_t *db, const char *item) {
	return item_get_fld(db, item, ITEM_PWD_FIELD);
}

void item_remove(db_t *db, const char *item) {
	json_t *root = (json_t *) db,
	       *accounts = json_object_get(root, "accounts");
	
	/* FIXME: for some reason this does not work, investigate */
	if (!json_object_del(accounts, item)) fail_printf("Invalid object");
}
