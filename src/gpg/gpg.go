/*
 * Pretty command-line password manager.
 *
 * Copyright (c) 2014, Alessandro Ghedini
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

package gpg

// #cgo LDFLAGS: -lgpgme
// #include <gpgme.h>
// #include <stdlib.h>
import "C"

import "fmt"
import "strings"
import "unsafe"

var keys []string

func Init(keys_spec string) error {
	C.gpgme_check_version(nil)

	gpg_err := C.gpgme_engine_check_version(C.GPGME_PROTOCOL_OpenPGP)
	if gpg_err != C.GPG_ERR_NO_ERROR {
		return &GPGError{gpg_err}
	}

	keys = strings.Split(keys_spec, " ")

	return nil
}

func Encrypt(data []byte) ([]byte, error) {
	var gpg_data C.gpgme_data_t

	c_data := C.CString(string(data))
	defer C.free(unsafe.Pointer(c_data))
	c_len  := C.size_t(len(data))

	gpg_err := C.gpgme_data_new_from_mem(&gpg_data, c_data, c_len, 0)
	if gpg_err != C.GPG_ERR_NO_ERROR {
		return nil, &GPGError{gpg_err}
	}
	defer C.gpgme_data_release(gpg_data)

	return EncryptData(gpg_data)
}

func EncryptData(gpg_in C.gpgme_data_t) ([]byte, error) {
	var gpg_ctx C.gpgme_ctx_t
	var gpg_out C.gpgme_data_t
	var gpg_keys []C.gpgme_key_t

	gpg_err := C.gpgme_new(&gpg_ctx)
	if gpg_err != C.GPG_ERR_NO_ERROR {
		return nil, &GPGError{gpg_err}
	}
	defer C.gpgme_release(gpg_ctx)

	C.gpgme_set_textmode(gpg_ctx, 1)
	C.gpgme_set_armor(gpg_ctx, 1)

	gpg_err = C.gpgme_data_new(&gpg_out)
	if gpg_err != C.GPG_ERR_NO_ERROR {
		return nil, &GPGError{gpg_err}
	}
	defer C.gpgme_data_release(gpg_out)

	for _, key := range keys {
		var gpg_key C.gpgme_key_t

		gpg_err = C.gpgme_get_key(gpg_ctx, C.CString(key), &gpg_key, 1)
		if gpg_err != C.GPG_ERR_NO_ERROR {
			return nil, fmt.Errorf(
				"Could not load key '%s': %s",
				key, &GPGError{gpg_err},
			)
		}
		defer C.gpgme_key_unref(gpg_key)

		gpg_err = C.gpgme_signers_add(gpg_ctx, gpg_key)
		if gpg_err != C.GPG_ERR_NO_ERROR {
			return nil, &GPGError{gpg_err}
		}

		gpg_keys = append(gpg_keys, gpg_key)
	}

	gpg_keys = append(gpg_keys, nil)

	gpg_err = C.gpgme_op_encrypt_sign(
		gpg_ctx, (*C.gpgme_key_t)(&gpg_keys[0]),
		C.GPGME_ENCRYPT_ALWAYS_TRUST, gpg_in, gpg_out,
	)
	if gpg_err != C.GPG_ERR_NO_ERROR {
		return nil, &GPGError{gpg_err}
	}

	return ReadDataAll(gpg_out), nil
}

func DecryptFile(path string) ([]byte, error) {
	var gpg_data C.gpgme_data_t

	gpg_err := C.gpgme_data_new_from_file(&gpg_data, C.CString(path), 1)
	if gpg_err != C.GPG_ERR_NO_ERROR {
		return nil, &GPGError{gpg_err}
	}
	defer C.gpgme_data_release(gpg_data)

	return DecryptData(gpg_data)
}

func DecryptData(gpg_in C.gpgme_data_t) ([]byte, error) {
	var gpg_ctx C.gpgme_ctx_t
	var gpg_out C.gpgme_data_t

	gpg_err := C.gpgme_new(&gpg_ctx)
	if gpg_err != C.GPG_ERR_NO_ERROR {
		return nil, &GPGError{gpg_err}
	}
	defer C.gpgme_release(gpg_ctx)

	gpg_err = C.gpgme_data_new(&gpg_out)
	if gpg_err != C.GPG_ERR_NO_ERROR {
		return nil, &GPGError{gpg_err}
	}
	defer C.gpgme_data_release(gpg_out)

	gpg_err = C.gpgme_op_decrypt_verify(gpg_ctx, gpg_in, gpg_out)
	if gpg_err != C.GPG_ERR_NO_ERROR {
		return nil, &GPGError{gpg_err}
	}

	buf := ReadDataAll(gpg_out)

	return buf, nil
}

func ReadDataAll(gpg_data C.gpgme_data_t) []byte {
	size := C.gpgme_data_seek(gpg_data, 0, C.SEEK_END)
	data := make([]byte, size)

	C.gpgme_data_seek(gpg_data, 0, C.SEEK_SET)
	C.gpgme_data_read(gpg_data, unsafe.Pointer(&data[0]), C.size_t(size))

	return data
}

type GPGError struct {
	err C.gpgme_error_t
}

func (e *GPGError) Error() string {
	return C.GoString(C.gpgme_strerror(e.err))
}
