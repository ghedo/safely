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

import "encoding/json"
import "io"
import "io/ioutil"
import "fmt"
import "path/filepath"
import "regexp"
import "sort"
import "os"

import "github.com/ghedo/safely/gpg"

type Db struct {
    Path     string              `json:"-"`
    Accounts map[string]*Account `json:"accounts"`
}

type Account struct {
    User  string `json:"user"`
    Pass  string `json:"pass"`
    TFKey string `json:"2fakey"`
    Date  string `json:"date"`
}

func Create(db_file string) (*Db, error) {
    err := os.MkdirAll(filepath.Dir(db_file), 0700)
    if err != nil {
        return nil, fmt.Errorf("Could not create config dir '%s': %s",
            filepath.Dir(db_file), err)
    }

    lock(db_file)
    defer unlock(db_file)

    file, err := os.OpenFile(db_file, os.O_RDWR|os.O_CREATE, 0600)
    if err != nil {
        return nil, fmt.Errorf("Could not create DB: %s", err)
    }
    defer file.Close()

    db := &Db{
        Path:     db_file,
        Accounts: make(map[string]*Account),
    }

    return db, nil
}

func Open(db_file string) (*Db, error) {
    lock(db_file)
    defer unlock(db_file)

    plain, err := gpg.DecryptFile(db_file)
    if err != nil {
        return nil, fmt.Errorf("Could not decrypt DB: %s", err)
    }

    db := &Db{ Path: db_file }

    err = json.Unmarshal(plain, db)
    if err != nil {
        return nil, fmt.Errorf("Could not decode DB: %s", err)
    }

    return db, nil
}

func Dump(db_file string) ([]byte, error) {
    lock(db_file)
    defer unlock(db_file)

    plain, err := gpg.DecryptFile(db_file)
    if err != nil {
        return nil, fmt.Errorf("Could not decrypt DB: %s", err)
    }

    return plain, nil
}

func (db *Db) Sync(do_backup bool) error {
    lock(db.Path)
    defer unlock(db.Path)

    if do_backup == true {
        err := backup(db.Path)
        if err != nil {
            return err
        }
    }

    data, err := json.MarshalIndent(db, "", "  ")
    if err != nil {
        return fmt.Errorf("Could not encode DB: %s", err)
    }

    cipher, err := gpg.Encrypt(data)
    if err != nil {
        return fmt.Errorf("Could not encrypt DB: %s", err)
    }

    err = ioutil.WriteFile(db.Path, cipher, 0600)
    if err != nil {
        return fmt.Errorf("Could not write to DB: %s", err)
    }

    return nil
}

func (db *Db) Search(query string, fuzzy bool) *Account {
    if fuzzy != true {
        return db.Accounts[query]
    }

    keys := []string{}

    for name, _ := range db.Accounts {
        keys = append(keys, name)
    }

    sort.Strings(keys)

    for _, name := range keys {
        if m, _ := regexp.MatchString(query, name); m {
            return db.Accounts[name]
        }
    }

    return nil
}

func (db *Db) Remove() error {
    os.Remove(db.Path)
    return nil
}

func backup(db_file string) error {
    bkk_file := fmt.Sprintf("%s~", db_file)

    db, err := os.Open(db_file)
    if err != nil {
        return fmt.Errorf("Could not open DB: %s", err)
    }
    defer db.Close()

    bkk, err := os.OpenFile(
        bkk_file, os.O_RDWR|os.O_CREATE|os.O_TRUNC, 0600,
    )
    if err != nil {
        return fmt.Errorf("Could not create backup file: %s", err)
    }
    defer bkk.Close()

    _, err = io.Copy(bkk, db)
    if err != nil {
        return fmt.Errorf("Could not backup DB: %s", err)
    }

    return nil
}
