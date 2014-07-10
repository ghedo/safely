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
import "regexp"
import "strings"
import "syscall"
import "time"

import "code.google.com/p/go.crypto/ssh/terminal"
import "github.com/docopt/docopt-go"

import "db"
import "gpg"
import "util"

func main() {
	log.SetFlags(0);

	usage := `safely [options]

Usage:
  safely [options] --keys <keys> --create
  safely [options] --keys <keys> --add <name>
  safely [options] --user <name>
  safely [options] --pass <name>
  safely [options] --keys <keys> --edit <name>
  safely [options] --keys <keys> --remove <name>
  safely [options] --search <query>
  safely [options] --dump

Options:
  -c, --create                  Create a new password database.
  -a <name>, --add <name>       Add a new account to the database.
  -u <name>, --user <name>      Print the user name of the given account.
  -p <name>, --pass <name>      Print the password of the given account.
  -e <name>, --edit <name>      Modify the given account.
  -r <name>, --remove <name>    Remove the given account.
  -s <query>, --search <query>  Search the database for the given query.
  -d, --dump                    Dump the database in JSON format.
  -D <file>, --db <file>        Use this database file [default: ~/.safely.db].
  -K <keys>, --keys <keys>      Use these space-separated GPG keys [default: ].
  -F, --fuzzy                   Enable non-exact (fuzzy) matches.
  -Q, --quiet                   Quiet output.
  -B, --no-backup               Do not create a backup of the database.
  -h, --help                    Show the program's help message and exit.`

	args, err := docopt.Parse(usage, nil, true, "", false)
	if err != nil {
		log.Fatal("Invalid arguments: ", err);
	}

	db_file   := util.ExpandUser(args["--db"].(string));
	keys_spec := args["--keys"].(string);

	fuzzy     := args["--fuzzy"].(bool);
	quiet     := args["--quiet"].(bool);
	no_backup := args["--no-backup"].(bool);

	SecurityCheck();

	gpg.Init();

	switch {
		case args["--create"].(bool) == true:
			keys := strings.Split(keys_spec, " ");

			mydb, err := db.Create(db_file);
			if err != nil {
				log.Fatal("Error creating database: ", err);
			}

			defer func() {
				if r := recover(); r != nil {
					mydb.Remove();
					os.Exit(1);
				}

				if quiet != true {
					log.Printf("Database '%s' created",
					           db_file);
				}
			}();

			defer mydb.DeferredSync(keys, false);

			break;

		case args["--add"] != nil:
			account := args["--add"].(string);

			keys := strings.Split(keys_spec, " ");

			mydb, err := db.Open(db_file);
			if err != nil {
				log.Fatal("Error opening database: ", err);
			}

			defer func() {
				if r := recover(); r != nil {
					os.Exit(1);
				}

				if quiet != true {
					log.Printf("Account '%s' added",
					           account);
				}
			}();

			defer mydb.DeferredSync(keys, !no_backup);

			if mydb.Search(account, false) != nil {
				log.Fatalf("Account '%s' already exists",
				           account);
			}

			user := util.ReadLine(fmt.Sprintf(
				"Enter user name for '%s': ", account,
			));

			pass := util.ReadPass(fmt.Sprintf(
				"Enter password for '%s': ", account,
			));

			mydb.Accounts[account] = &db.Account{
				user, pass, time.Now().Format(time.RFC3339),
			};

			break;

		case args["--user"] != nil:
			query := args["--user"].(string);

			mydb, err := db.Open(db_file);
			if err != nil {
				log.Fatal("Error opening database: ", err);
			}

			account := mydb.Search(query, fuzzy);
			if account == nil {
				log.Fatalf("Account '%s' not found", query);
			}

			fmt.Print(account.User);

			if terminal.IsTerminal(int(os.Stdout.Fd())) {
				fmt.Println("");
			}

			break;

		case args["--pass"] != nil:
			query := args["--pass"].(string);

			mydb, err := db.Open(db_file);
			if err != nil {
				log.Fatal("Error opening database: ", err);
			}

			account := mydb.Search(query, fuzzy);
			if account == nil {
				log.Fatalf("Account '%s' not found", query);
			}

			fmt.Print(account.Pass);

			if terminal.IsTerminal(int(os.Stdout.Fd())) {
				fmt.Println("");
			}

			break;

		case args["--edit"] != nil:
			query := args["--edit"].(string);

			keys := strings.Split(keys_spec, " ");

			mydb, err := db.Open(db_file);
			if err != nil {
				log.Fatal("Error opening database: ", err);
			}

			defer func() {
				if r := recover(); r != nil {
					os.Exit(1);
				}

				if quiet != true {
					log.Printf("Account '%s' edited",
					           query);
				}
			}();

			defer mydb.DeferredSync(keys, !no_backup);

			account := mydb.Search(query, false);
			if account == nil {
				log.Fatalf("Account '%s' not found", query);
			}

			user := util.ReadLine(fmt.Sprintf(
				"Enter new user name for '%s' [%s]: ",
				query, account.User,
			));

			pass := util.ReadPass(fmt.Sprintf(
				"Enter new password for '%s' [%s]: ",
				query, account.Pass,
			));

			mydb.Accounts[query] = &db.Account{
				user, pass, time.Now().Format(time.RFC3339),
			};

			break;

		case args["--remove"] != nil:
			account := args["--remove"].(string);

			keys := strings.Split(keys_spec, " ");

			mydb, err := db.Open(db_file);
			if err != nil {
				log.Fatal("Error opening database: ", err);
			}

			defer func() {
				if r := recover(); r != nil {
					os.Exit(1);
				}

				if quiet != true {
					log.Printf("Account '%s' removed",
					           account);
				}
			}();

			defer mydb.DeferredSync(keys, !no_backup);

			if mydb.Search(account, false) == nil {
				log.Fatalf("Account '%s' not found", account);
			}

			delete(mydb.Accounts, account);

			break;

		case args["--search"] != nil:
			query := args["--search"].(string);

			mydb, err := db.Open(db_file);
			if err != nil {
				log.Fatal("Error opening database: ", err);
			}

			for name, _ := range mydb.Accounts {
				if m, _ := regexp.MatchString(query, name); m {
					fmt.Println(name);
				}
			}

			break;

		case args["--dump"].(bool) == true:
			plain, err := db.Dump(db_file);
			if err != nil {
				log.Fatal("Error dumping database: ", err);
			}

			fmt.Printf("%s\n", plain);

			break;

		default:
			log.Fatal("Invalid command");
			break;
	}
}

func SecurityCheck() {
	var lim syscall.Rlimit;

	if os.Getuid() == 0 || os.Getgid() == 0 {
		log.Fatal("Running as root");
	}

	err := syscall.Getrlimit(syscall.RLIMIT_CORE, &lim);
	if err != nil {
		log.Fatal("Could not get rlimit: ", err);
	}

	if lim.Cur != 0 {
		log.Fatal("Core dumps are enabled");
	}
}
