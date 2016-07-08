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
import "io"
import "os/exec"
import "strings"

var keys []string

func Init(keys_spec string) error {
    keys = strings.Split(keys_spec, " ")

    return nil
}

func Encrypt(data []byte) ([]byte, error) {
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

    return ExecGPG(args, bytes.NewBuffer(data))
}

func DecryptFile(path string) ([]byte, error) {
    args := []string{ "--batch", "--decrypt", path }
    return ExecGPG(args, nil)
}

func ExecGPG(args []string, in io.Reader) ([]byte, error) {
    var cmd *exec.Cmd
    var stdout, stderr bytes.Buffer

    for _, name := range []string{ "gpg2", "gpg" } {
        exe, err := exec.LookPath(name)
        if err != nil {
            continue
        }

        cmd = exec.Command(exe, args...)
        break
    }

    if cmd == nil {
        return nil, fmt.Errorf("Could not find gpg executable")
    }

    cmd.Stdin  = in
    cmd.Stdout = &stdout
    cmd.Stderr = &stderr

    err := cmd.Run()
    if err != nil {
        return nil, fmt.Errorf("%s", stderr.String())
    }

    return stdout.Bytes(), nil
}
