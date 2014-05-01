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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <errno.h>

#include <gpgme.h>

#include "printf.h"
#include "security.h"
#include "util.h"

static void gpg_init(gpgme_protocol_t proto) {
	gpgme_error_t err;

	gpgme_check_version(NULL);
	setlocale(LC_ALL, "");
	gpgme_set_locale(NULL, LC_CTYPE, setlocale(LC_CTYPE, NULL));

	err = gpgme_engine_check_version(proto);
	if (err)
		throw_error(
			2, "Cannot check GPG version: %s (%s)",
			gpgme_strerror(err), gpgme_strsource(err)
		);
}

static gpgme_error_t passphrase_cb(void *opaque, const char *uid_hint,
			const char *passphrase_info, int last_was_bad, int fd) {
	char pass[INPUT_MAX_SIZE];
	int c, i = 0, rc, off = 0, passlen;

	security_echo_off();
	fprintf(stderr, "Enter password for GPG key: ");

	while ((c = fgetc(stdin)) && i < INPUT_MAX_SIZE) {
		pass[i++] = c;

		if (c == '\n')
			break;
	}

	pass[i] = '\0';

	security_echo_on();
	fprintf(stderr, "\n");

	if (pass == NULL)
		throw_error(2, "NULL password");

	passlen = strlen(pass);

	do {
		rc = write(fd, &pass[off], passlen - off);
		if (rc > 0) off += rc;
	} while (rc > 0 && off != passlen);

	memset(pass, 0, INPUT_MAX_SIZE);

	return off == passlen ? 0 : gpgme_error_from_errno(errno);
}

static char *gpg_data_to_char(gpgme_data_t dh) {
	int rc;
	size_t data_size;
	char *data = NULL;

	data_size = gpgme_data_seek(dh, 0, SEEK_END);
	gpgme_data_seek(dh, 0, SEEK_SET);
	if (data_size <= 0) throw_error(2, "Cannot seek GPG data");

	data = calloc(data_size + 1, 1);
	if (data == NULL) throw_error(2, "Cannot allocate more memory");

	rc = gpgme_data_read(dh, data, data_size);
	if (rc < 0) throw_error(2, "Cannot read GPG data");

	return data;
}

static gpgme_key_t *gpg_parse_keys(gpgme_ctx_t ctx, int *c) {
	int keysc = 1, i;
	gpgme_key_t *keys;
	char *save, *tmp, *keyss = getenv("SAFELY_KEYS");

	if (keyss == NULL) {
		keys = malloc(sizeof(gpgme_key_t) * 2);
		keys[0] = NULL;
		goto exit;
	}

	for (tmp = keyss; *tmp != '\0'; tmp++)
		if (isspace(*tmp)) keysc++;

	keys = malloc(sizeof(gpgme_key_t) * (keysc + 1));

	for (i = 0, tmp = keyss; i < keysc; i++) {
		char *tok;

		gpgme_key_t key;
		gpgme_error_t err;

		if (keysc == 1)
			tok = tmp;
		else
			tok = strtok_r(tmp, " ", &save);

		if (tmp != NULL)
			tmp = NULL;

		if (tok == NULL)
			break;

		err = gpgme_get_key(ctx, tok, &key, 1);

		if (err)
			throw_error(
				2, "Invalid key '%s': %s (%s)",
				tok, gpgme_strerror(err), gpgme_strsource(err)
			);

		keys[i] = key;

		err = gpgme_signers_add(ctx, key);
		if (err)
			throw_error(
				2, "%s (%s)",
				tok, gpgme_strerror(err), gpgme_strsource(err)
			);
	}

exit:
	keys[keysc] = NULL;

	*c = keysc;

	return keys;
}

char *gpg_encrypt(const char *str, const char *keyfpr) {
	int i, keysc;
	char *agent_info;
	char *return_buf = NULL;

	gpgme_ctx_t   ctx;
	gpgme_data_t  in, out;
	gpgme_error_t err;

	_free_ gpgme_key_t *keys = NULL;

	gpgme_encrypt_result_t crypt_result;
	gpgme_sign_result_t sign_result;

	gpg_init(GPGME_PROTOCOL_OpenPGP);

	err = gpgme_new(&ctx);
	if (err)
		throw_error(
			2, "Cannot initialize GPG context: %s (%s)",
			gpgme_strerror(err), gpgme_strsource(err)
		);

	gpgme_set_textmode(ctx, 1);
	gpgme_set_armor(ctx, 1);

	keys = gpg_parse_keys(ctx, &keysc);

	agent_info = getenv("GPG_AGENT_INFO");
	if (!(agent_info && strchr(agent_info, ':')))
		gpgme_set_passphrase_cb(ctx, passphrase_cb, NULL);

	err = gpgme_data_new_from_mem(&in, str, strlen(str), 0);
	if (err)
		throw_error(
			2, "Cannot load GPG data from memory: %s (%s)",
			gpgme_strerror(err), gpgme_strsource(err)
		);

	err = gpgme_data_new(&out);
	if (err)
		throw_error(
			2, "Cannot load GPG data: %s (%s)",
			gpgme_strerror(err), gpgme_strsource(err)
		);

	err = gpgme_op_encrypt_sign(ctx, keys, GPGME_ENCRYPT_ALWAYS_TRUST, in, out);
	if (err)
		throw_error(
			2, "Cannot GPG encrypt/sign: %s (%s)",
			gpgme_strerror(err), gpgme_strsource(err)
		);

	crypt_result = gpgme_op_encrypt_result(ctx);

	if (crypt_result -> invalid_recipients)
		throw_error(
			2, "Invalid GPG recipient: %s\n",
			crypt_result -> invalid_recipients -> fpr
		);

	sign_result = gpgme_op_sign_result(ctx);

	if (sign_result -> invalid_signers)
		throw_error(
			2, "Invalid signers: %s",
			sign_result -> invalid_signers -> fpr
		);

	return_buf = gpg_data_to_char(out);

	gpgme_signers_clear(ctx);

	for (i = 0; i < keysc; i++)
		gpgme_key_unref(keys[i]);

	gpgme_data_release(in);
	gpgme_data_release(out);

	gpgme_release(ctx);

	return return_buf;
}

char *gpg_decrypt_data(gpgme_data_t in) {
	char *agent_info;
	char *return_buf = NULL;

	gpgme_ctx_t	ctx;
	gpgme_error_t	err;
	gpgme_data_t	out;

	gpgme_decrypt_result_t decrypt_result;
	gpgme_verify_result_t verify_result;

	gpg_init(GPGME_PROTOCOL_OpenPGP);

	err = gpgme_new(&ctx);
	if (err)
		throw_error(
			1, "Cannot initialize GPG context: %s (%s)",
			gpgme_strerror(err), gpgme_strsource(err)
		);

	agent_info = getenv("GPG_AGENT_INFO");

	if (!agent_info)
		gpgme_set_passphrase_cb(ctx, passphrase_cb, NULL);

	err = gpgme_data_new(&out);
	if (err)
		throw_error(1, "Cannot load GPG data: %s (%s)",
			gpgme_strerror(err), gpgme_strsource(err)
		);

	err = gpgme_op_decrypt_verify(ctx, in, out);
	if(err)
		throw_error(
			1, "Cannot GPG decrypt/verify: %s (%s)",
			gpgme_strerror(err), gpgme_strsource(err)
		);

	decrypt_result = gpgme_op_decrypt_result(ctx);
	if (decrypt_result -> unsupported_algorithm)
		throw_error(
			1, "Unsupported GPG algorithm: %s\n",
			decrypt_result -> unsupported_algorithm
		);

	verify_result = gpgme_op_verify_result(ctx);
	if (gpg_err_code(verify_result -> signatures -> validity_reason) != GPG_ERR_NO_ERROR)
		throw_error(
			1, "Unexpected validity reason: %i",
			verify_result -> signatures -> validity_reason
		);

	return_buf = gpg_data_to_char(out);

	gpgme_data_release(in);
	gpgme_data_release(out);

	gpgme_release(ctx);

	return return_buf;
}

char *gpg_decrypt(const char *cipher) {
	gpgme_data_t in;
	gpgme_error_t err;

	err = gpgme_data_new_from_mem(&in, cipher, strlen(cipher), 0);
	if (err)
		throw_error(
			1, "Cannot load GPG data from memory: %s (%s)",
			gpgme_strerror(err), gpgme_strsource(err)
		);

	return gpg_decrypt_data(in);
}

char *gpg_decrypt_file(const char *path) {
	gpgme_data_t in;
	gpgme_error_t err;

	err = gpgme_data_new_from_file(&in, path, 1);
	if (err)
		throw_error(
			1, "Cannot load GPG data from file: %s (%s)",
			gpgme_strerror(err), gpgme_strsource(err)
		);

	return gpg_decrypt_data(in);
}
