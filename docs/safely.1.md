safely(1) -- pretty command-line password manager
=================================================

## SYNOPSIS

`safely [OPTIONS] COMMAND [ACCOUNT]`

## DESCRIPTION

**safely** is a command-line tool to manage passwords. It stores all your
login information (usernames and passwords) in a simple JSON-formatted
file, encrypted and signed using your OpenPGP key: this makes it pretty
secure and allows manual decryption of the database when needed.

## COMMANDS ##

`-c, --create`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Create a new password database.

`-a, --add`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Add a new account to the database.

`-p, --pass`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Print the password of the given account.

`-u, --user`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Print the user name of the given account.

`-2, --2fa`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Print 2-factor auth token for the given account.

`-e, --edit`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Modify the given account.

`-r, --remove`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Remove the given account.

`-s, --search`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Search the database for the given query.

`-C, --check`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Estimate the given password's strength.

`-d, --dump`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Dump the database in JSON format.

`-h, --help`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Show the program's help message and exit.

## OPTIONS ##

`-D, --db`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Use this database file [default: ~/.config/safely/passwords.db].

`-K, --keys`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Use these space-separated GPG keys [default: ].

`-F, --fuzzy`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Enable non-exact (fuzzy) matches.

`-N, --print-newline`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Force printing a trailing newline even if the output doesn't go to a terminal.

This is useful for piping safely output to programs that expect a trailing
newline.

`-Q, --quiet`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Quiet output.

`-B, --no-backup`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Do not create a backup of the database.

## ENVIRONMENT ##

`GPG_AGENT_INFO`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Used to locate the gpg-agent.

## FILES ##

`~/.config/safely/passwords.db`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
The default password database file. The option `--db` can be used to
override it.

`~/.config/safely/passwords.db~`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
The default password database backup file. If the `--db` option is set, this
will change accordingly. If the `--no-backup` option is used, this file is
not created/modified.

`~/.config/safely/passwords.db.lock`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
The default password database lock file. If the `--db` option is set, this
will change accordingly.

## AUTHOR ##

Alessandro Ghedini <alessandro@ghedini.me>

## COPYRIGHT ##

Copyright (C) 2014 Alessandro Ghedini <alessandro@ghedini.me>

This program is released under the 2 clause BSD license.
