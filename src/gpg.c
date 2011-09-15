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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <errno.h>

#include <gpgme.h>

#include "interface.h"
#include "security.h"

/* TODO: implement gpg_get_keys() */

static void gpg_init(gpgme_protocol_t proto) {
	gpgme_error_t err;

	gpgme_check_version(NULL);
	setlocale(LC_ALL, "");
	gpgme_set_locale(NULL, LC_CTYPE, setlocale(LC_CTYPE, NULL));

	err = gpgme_engine_check_version(proto);
	if (err) fail_printf("Failed GPG version check: %s", gpgme_strerror(err));
}

static gpgme_error_t passphrase_cb(void *opaque, const char *uid_hint,
	const char *passphrase_info, int last_was_bad, int fd) {

	char *pass = NULL;
	int res, off = 0, passlen;

	security_echo_off();
	printf("Enter password for GPG key [%s]: ", "xxxx");
	get_input(pass);
	security_echo_on();
	putchar('\n');

	if (pass == NULL) fail_printf("NULL password");

	passlen = strlen(pass);

	do {
		res = write(fd, &pass[off], passlen - off);

		if (res > 0) off += res;
	} while (res > 0 && off != passlen);

	return off == passlen ? 0 : gpgme_error_from_errno(errno);
}

static char *gpg_data_to_char(gpgme_data_t dh) {
	int ret;
	size_t data_size;
	char *data = NULL;

	data_size = gpgme_data_seek(dh, 0, SEEK_END);
	gpgme_data_seek(dh, 0, SEEK_SET);

	if (data_size <= 0) fail_printf("Failed GPG data seek");

	data = malloc(data_size + 1);
	if (data == NULL) fail_printf("Failed malloc");

	ret = gpgme_data_read(dh, data, data_size);
	if (ret < 0) fail_printf("Failed GPG data read");

	return data;
}

char *gpg_encrypt(const char *str, const char *keyfpr) {
	char *agent_info;
	char *return_buf = NULL;

	gpgme_ctx_t ctx;
	gpgme_error_t err;
	gpgme_data_t in, out;

	gpgme_key_t key[2] = { NULL, NULL };

	gpgme_encrypt_result_t result;
	gpgme_sign_result_t sign_result;

	gpg_init(GPGME_PROTOCOL_OpenPGP);

	err = gpgme_new(&ctx);
	if (err) fail_printf("Failed GPG new: %s", gpgme_strerror(err));

	gpgme_set_textmode(ctx, 1);
	gpgme_set_armor(ctx, 1);

	agent_info = getenv("GPG_AGENT_INFO");
	if (!(agent_info && strchr(agent_info, ':')))
		gpgme_set_passphrase_cb(ctx, passphrase_cb, NULL);

	err = gpgme_data_new_from_mem(&in, str, strlen(str), 0);
	if (err) fail_printf("Failed GPG new data from mem: %s", gpgme_strerror(err));

	err = gpgme_data_new(&out);
	if (err) fail_printf("Failed GPG new data: %s", gpgme_strerror(err));

	err = gpgme_get_key(ctx, keyfpr, &key[0], 0);
	if(err) fail_printf("Failed GPG get key: %s");

	err = gpgme_op_encrypt_sign(ctx, key, GPGME_ENCRYPT_ALWAYS_TRUST, in, out);
	if (err) fail_printf("Failed GPG encrypt/sign: %s", gpgme_strerror(err));

	result = gpgme_op_encrypt_result(ctx);

	/* TODO: check result */

	if (result -> invalid_recipients)
		fail_printf("Invalid GPG recipient: %s\n", result -> invalid_recipients -> fpr);

	sign_result = gpgme_op_sign_result(ctx);

	/* TODO: check sign_result */

	return_buf = gpg_data_to_char(out);

	gpgme_key_unref(key[0]);
	gpgme_key_unref(key[1]);

	gpgme_data_release(in);
	gpgme_data_release(out);

	gpgme_release(ctx);

	return return_buf;
}

char *gpg_decrypt(const char *cipher) {
	char *agent_info;
	char *return_buf = NULL;

	gpgme_ctx_t ctx;
	gpgme_error_t err;
	gpgme_data_t in, out;

	gpgme_decrypt_result_t decrypt_result;
	gpgme_verify_result_t verify_result;

	gpg_init(GPGME_PROTOCOL_OpenPGP);

	err = gpgme_new(&ctx);
	if (err) fail_printf("Failed GPG new: %s", gpgme_strerror(err));

	agent_info = getenv("GPG_AGENT_INFO");

	if (!(agent_info && strchr(agent_info, ':')))
		gpgme_set_passphrase_cb(ctx, passphrase_cb, NULL);

	err = gpgme_data_new_from_mem(&in, cipher, strlen(cipher), 0);
	if (err) fail_printf("Failed GPG new data from mem: %s", gpgme_strerror(err));

	err = gpgme_data_new(&out);
	if (err) fail_printf("Failed GPG new data: %s", gpgme_strerror(err));

	err = gpgme_op_decrypt_verify (ctx, in, out);
	if(err) fail_printf("Failed GPG decrypt/verify: %s", gpgme_strerror(err));
	decrypt_result = gpgme_op_decrypt_result (ctx);

	if (decrypt_result -> unsupported_algorithm)
		fail_printf("Unsupported GPG algorithm: %s\n", decrypt_result -> unsupported_algorithm);

	return_buf = gpg_data_to_char(out);

	verify_result = gpgme_op_verify_result(ctx);

	/* TODO: check verify_result */

	gpgme_data_release(in);
	gpgme_data_release(out);

	gpgme_release(ctx);

	return return_buf;
}
