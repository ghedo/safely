safely(1) -- pretty command-line password manager
=================================================

## SYNOPSIS

`safely [OPTIONS] COMMAND [ACCOUNT]`

## DESCRIPTION

**Safely** is a command-line tool to manage passwords. It stores all your
login information (usernames and passwords) in a simple, JSON-formatted 
file, encrypted and signed using your OpenPGP key: this makes it pretty 
secure and allows manual decryption of the database when needed.

## COMMANDS ##

`-c, --create`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Create a new password database.

`-a, --add`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Add the given account.

`-p, --passwd`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Show the given account's password.

`-u, --user`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Show the given account's username.

`-e, --edit`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Edit the given account.

`-r, --remove`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Remove the given account.

`-s, --search`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Search for the given pattern.

`-d, --dump`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Dump JSON database.

`-t, --tests`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Run the security tests only.

`-v, --version`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Show program's version and exit.

`-h, --help`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Show program's help and exit.

## OPTIONS ##

`-D, --db`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Specifies a custom path to the password database.

`-K, --keys`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Space-separated list of keys to be used.

`-H, --hide`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Print the password in red on red background to avoid shoulder surfing.

`-Q, --quiet`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Output errors only (i.e. no "[ok]" messages).

`-F, --fuzzy`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Search for non-exact (fuzzy) matches with `--passwd` and `--user`.

`-S, --nosecure`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Ignore any security test failure.

`-B, --nobackup`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Do not create database file backups.

`-A, --noagent`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Do not use the default gpg agent.

## ENVIRONMENT ##

`GPG_AGENT_INFO`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Used to locate the gpg-agent. This is only used when `--noagent` is not set.

## FILES ##

`~/.safely.db`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
The default password database file. The option `--db` can be used to 
override it.

`~/.safely.db~`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
The default password database backup file. If the `--db` option is set, this
will change accordingly. If the `--nobackup` option is used, this file is 
not created/modified.

`~/.safely.db.lock`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
The default password database lock file. If the `--db` option is set, this
will change accordingly.

## AUTHOR ##

Alessandro Ghedini <al3xbio@gmail.com>

## COPYRIGHT ##

Copyright (C) 2011-2012 Alessandro Ghedini <al3xbio@gmail.com>

This program is released under the 3 clause BSD license.
