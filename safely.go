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

package main

import "fmt"
import "log"
import "os"
import "os/signal"
import "regexp"
import "time"

import "golang.org/x/crypto/ssh/terminal"
import "github.com/docopt/docopt-go"
import "github.com/nbutton23/zxcvbn-go"

import "github.com/ghedo/safely/db"
import "github.com/ghedo/safely/gpg"
import "github.com/ghedo/safely/oath"
import "github.com/ghedo/safely/term"
import "github.com/ghedo/safely/util"

func main() {
    log.SetFlags(0)

    usage := `safely [options]

Usage:
  safely [options] [--keys <keys>] --create
  safely [options] [--keys <keys>] --add <name>
  safely [options] --user <name>
  safely [options] --pass <name>
  safely [options] --2fa <name>
  safely [options] [--keys <keys>] --edit <name>
  safely [options] [--keys <keys>] --remove <name>
  safely [options] --search <query>
  safely [options] --check <pass>
  safely [options] --dump

Options:
  -c, --create                  Create a new password database.
  -a <name>, --add <name>       Add a new account to the database.
  -u <name>, --user <name>      Print the user name of the given account.
  -p <name>, --pass <name>      Print the password of the given account.
  -2 <name>, --2fa <name>       Print 2-factor auth token for the given account.
  -e <name>, --edit <name>      Modify the given account.
  -r <name>, --remove <name>    Remove the given account.
  -s <query>, --search <query>  Search the database for the given query.
  -C <pass>, --check <pass>     Estimate the given password's strength.
  -d, --dump                    Dump the database in JSON format.
  -D <file>, --db <file>        Use this database file [default: ~/.config/safely/passwords.db].
  -K <keys>, --keys <keys>      Use these space-separated GPG keys [default: ].
  -F, --fuzzy                   Enable non-exact (fuzzy) matches.
  -N, --print-newline           Force printing a trailing newline.
  -Q, --quiet                   Quiet output.
  -B, --no-backup               Do not create a backup of the database.
  -h, --help                    Show the program's help message and exit.`

    args, err := docopt.Parse(usage, nil, true, "", false)
    if err != nil {
        log.Fatalf("Invalid arguments: %s", err)
    }

    db_file, err := util.ExpandUser(args["--db"].(string))
    if err != nil {
        log.Fatalf("Error expanding home directory: %s", err)
    }

    keys_spec := args["--keys"].(string)

    fuzzy := args["--fuzzy"].(bool)
    quiet := args["--quiet"].(bool)
    no_backup := args["--no-backup"].(bool)

    signal.Ignore(os.Interrupt)

    gpg.Init(keys_spec)

    switch {
    case args["--create"].(bool) == true:
        mydb, err := db.Create(db_file)
        if err != nil {
            mydb.Remove()
            log.Fatalf("Error creating database: %s", err)
        }

        err = mydb.Sync(false)
        if err != nil {
            log.Fatalf("Error syncing DB: %s", err)
        }

        if quiet != true {
            log.Printf("Database '%s' created", db_file)
        }

    case args["--add"] != nil:
        account := args["--add"].(string)

        mydb, err := db.Open(db_file)
        if err != nil {
            log.Fatalf("Error opening database: %s", err)
        }

        if mydb.Search(account, false) != nil {
            log.Fatalf("Account '%s' already exists", account)
        }

        user, err := term.ReadLine(fmt.Sprintf(
            "Enter user name for '%s': ", account,
        ))

        if err != nil {
            log.Fatalf("Error reading input: %s", err)
        }

        pass, err := term.ReadPass(fmt.Sprintf(
            "Enter password for '%s': ", account,
        ))

        if err != nil {
            log.Fatalf("Error reading input: %s", err)
        }

        score := zxcvbn.PasswordStrength(pass, nil)

        log.Printf("It would take %v seconds (%s) to crack the password\n",
                   score.CrackTime, score.CrackTimeDisplay)

        tfkey, err := term.ReadLine(fmt.Sprintf(
            "Enter 2-factor auth key for '%s': ", account,
        ))

        if err != nil {
            log.Fatalf("Error reading input: %s", err)
        }

        mydb.Accounts[account] = &db.Account{
            user, pass, tfkey,
            time.Now().Format(time.RFC3339),
        }

        err = mydb.Sync(!no_backup)
        if err != nil {
            log.Fatalf("Error syncing DB: %s", err)
        }

        if quiet != true {
            log.Printf("Account '%s' added", account)
        }

    case args["--user"] != nil:
        query := args["--user"].(string)

        mydb, err := db.Open(db_file)
        if err != nil {
            log.Fatalf("Error opening database: %s", err)
        }

        account := mydb.Search(query, fuzzy)
        if account == nil {
            log.Fatalf("Account '%s' not found", query)
        }

        fmt.Print(account.User)

        if args["--print-newline"].(bool) ||
           terminal.IsTerminal(int(os.Stdout.Fd())) {
            fmt.Println("")
        }

    case args["--pass"] != nil:
        query := args["--pass"].(string)

        mydb, err := db.Open(db_file)
        if err != nil {
            log.Fatalf("Error opening database: %s", err)
        }

        account := mydb.Search(query, fuzzy)
        if account == nil {
            log.Fatalf("Account '%s' not found", query)
        }

        fmt.Print(account.Pass)

        if args["--print-newline"].(bool) ||
           terminal.IsTerminal(int(os.Stdout.Fd())) {
            fmt.Println("")
        }

    case args["--2fa"] != nil:
        query := args["--2fa"].(string)

        mydb, err := db.Open(db_file)
        if err != nil {
            log.Fatalf("Error opening database: %s", err)
        }

        account := mydb.Search(query, fuzzy)
        if account == nil {
            log.Fatalf("Account '%s' not found", query)
        }

        if account.TFKey == "" {
            log.Fatalf("No 2-factor auth key for '%s'", query)
        }

        secret, err := oath.Base32Secret(account.TFKey)

        totp := oath.TOTP(secret)

        fmt.Print(totp)

        if args["--print-newline"].(bool) ||
           terminal.IsTerminal(int(os.Stdout.Fd())) {
            fmt.Println("")
        }

    case args["--edit"] != nil:
        query := args["--edit"].(string)

        mydb, err := db.Open(db_file)
        if err != nil {
            log.Fatalf("Error opening database: %s", err)
        }

        account := mydb.Search(query, false)
        if account == nil {
            log.Fatalf("Account '%s' not found", query)
        }

        name, err := term.ReadLine(fmt.Sprintf(
            "Enter new account name [%s]: ",
            query,
        ))

        if err != nil {
            log.Fatalf("Error reading input: %s", err)
        }

        if name == "" {
            name = query
        }

        user, err := term.ReadLine(fmt.Sprintf(
            "Enter new user name for '%s' [%s]: ",
            query, account.User,
        ))

        if err != nil {
            log.Fatalf("Error reading input: %s", err)
        }

        if user == "" {
            user = account.User
        }

        pass, err := term.ReadPass(fmt.Sprintf(
            "Enter new password for '%s' [%s]: ",
            query, account.Pass,
        ))

        if err != nil {
            log.Fatalf("Error reading input: %s", err)
        }

        if pass == "" {
            pass = account.Pass
        }

        tfkey, err := term.ReadLine(fmt.Sprintf(
            "Enter new 2-factor auth key for '%s' [%s]: ",
            query, account.TFKey,
        ))

        if err != nil {
            log.Fatalf("Error reading input: %s", err)
        }

        if tfkey == "" {
            tfkey = account.TFKey
        }

        mydb.Accounts[name] = &db.Account{
            user, pass, tfkey,
            time.Now().Format(time.RFC3339),
        }

        if name != query {
            delete(mydb.Accounts, query)
        }

        err = mydb.Sync(!no_backup)
        if err != nil {
            log.Fatalf("Error syncing DB: %s", err)
        }

        if quiet != true {
            log.Printf("Account '%s' edited", query)
        }

    case args["--remove"] != nil:
        account := args["--remove"].(string)

        mydb, err := db.Open(db_file)
        if err != nil {
            log.Fatalf("Error opening database: %s", err)
        }

        if mydb.Search(account, false) == nil {
            log.Fatalf("Account '%s' not found", account)
        }

        delete(mydb.Accounts, account)

        err = mydb.Sync(!no_backup)
        if err != nil {
            log.Fatalf("Error syncing DB: %s", err)
        }

        if quiet != true {
            log.Printf("Account '%s' removed", account)
        }

    case args["--search"] != nil:
        query := args["--search"].(string)

        mydb, err := db.Open(db_file)
        if err != nil {
            log.Fatalf("Error opening database: %s", err)
        }

        for name, _ := range mydb.Accounts {
            if m, _ := regexp.MatchString(query, name); m {
                fmt.Println(name)
            }
        }

    case args["--check"] != nil:
        pass := args["--check"].(string)

        score := zxcvbn.PasswordStrength(pass, nil)

        log.Printf("It would take %v seconds (%s) to crack the password\n",
                   score.CrackTime, score.CrackTimeDisplay)

    case args["--dump"].(bool) == true:
        plain, err := db.Dump(db_file)
        if err != nil {
            log.Fatalf("Error dumping database: %s", err)
        }

        fmt.Printf("%s\n", plain)

    default:
        log.Fatal("Invalid command")
    }
}
