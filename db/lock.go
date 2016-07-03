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

package db

import "log"
import "fmt"
import "os"

func is_locked(lock_file string) bool {
    if _, err := os.Stat(lock_file); err == nil {
        return true
    }

    return false
}

func lock(db_file string) {
    lock_file := fmt.Sprintf("%s.lock", db_file)

    if is_locked(lock_file) == true {
        log.Panic("Could not create lock file: already locked")
    }

    lock, err := os.OpenFile(
        lock_file, os.O_RDWR|os.O_CREATE|os.O_TRUNC, 0600,
    )
    if err != nil {
        log.Panicf("Could not create lock file: %s", err)
    }
    defer lock.Close()
}

func unlock(db_file string) {
    lock_file := fmt.Sprintf("%s.lock", db_file)

    if is_locked(lock_file) != true {
        log.Panic("Could not remove lock file: not locked")
    }

    err := os.Remove(lock_file)
    if err != nil {
        log.Panicf("Could not remove lock file: %s", err)
    }
}
