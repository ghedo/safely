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

import "bytes"
import "fmt"
import "os/exec"
import "strings"

var keys []string

func Init(keys_spec string) error {
    keys = strings.Split(keys_spec, " ")

    return nil
}

func Encrypt(data []byte) ([]byte, error) {
    var stdout, stderr bytes.Buffer

    args := []string{
        "--encrypt", "--sign", "--batch", "--armor",
        "--always-trust", "--default-recipient-self",
    }

    for _, k := range keys {
        if k == "" {
            break
        }

        args = append(args, "--recipient", k)
    }

    cmd := exec.Command("gpg", args...)

    cmd.Stdin  = bytes.NewBuffer(data)
    cmd.Stdout = &stdout
    cmd.Stderr = &stderr

    err := cmd.Run()
    if err != nil {
        return nil, fmt.Errorf("%s", stderr.String())
    }

    return stdout.Bytes(), nil
}

func DecryptFile(path string) ([]byte, error) {
    var stdout, stderr bytes.Buffer

    cmd := exec.Command("gpg", "--batch", "--decrypt", path)

    cmd.Stdout = &stdout
    cmd.Stderr = &stderr

    err := cmd.Run()
    if err != nil {
        return nil, fmt.Errorf("%s", stderr.String())
    }

    return stdout.Bytes(), nil
}
