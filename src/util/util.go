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

package util

import "bufio"
import "bytes"
import "fmt"
import "log"
import "os"
import "os/user"
import "strings"

import "code.google.com/p/go.crypto/ssh/terminal"

func ReadLine(prompt string) string {
	fmt.Fprint(os.Stderr, prompt);

	tmp, err := bufio.NewReader(os.Stdin).ReadBytes('\n');
	if err != nil {
		log.Fatal("Input error: ", err);
	}

	tmp = bytes.TrimSuffix(tmp, []byte("\n"));

	return bytes.NewBuffer(tmp).String();
}

func ReadPass(prompt string) string {
	fmt.Fprint(os.Stderr, prompt);

	tmp, err := terminal.ReadPassword(0);
	if err != nil {
		log.Fatal("Input error: ", err);
	}

	fmt.Fprintln(os.Stderr, "");

	return bytes.NewBuffer(tmp).String();
}

func ExpandUser(path string) string {
	user, err := user.Current();
	if err != nil {
		log.Fatal("Could not find current user: ", err);
	}

	if strings.HasPrefix(path, "~/") {
		return strings.Replace(path, "~", user.HomeDir, 1);
	}

	return path;
}
