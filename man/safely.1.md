safely(1) -- lightweight command-line password manager
======================================================

## SYNOPSIS

`safely [OPTIONS] COMMAND [ACCOUNT]`

## DESCRIPTION

**Safely** is a command-line tool to manage passwords. It stores all your login
information (usernames and passwords) in a simple, JSON-formatted file, encrypted
and signed using your OpenPGP key: this makes it pretty secure and allows manual
decryption of the database when needed.

## COMMANDS ##

`-c, --create`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Create a new password db.

`-a, --add`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Add a new account.

`-p, --passwd`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Show given account's password.

`-u, --user`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Show given account's username.

`-e, --edit`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Edit the given account.

`-r, --remove`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Remove given account.

`-s, --search`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Remove the given account.

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

`-Q, --quiet`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Output errors only (i.e. no "[ok]" messages).

`-S, --nosecure`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Ignore any security test failure.

`-B, --nobackup`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Do not create db file backups.

`-A, --noagent`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Do not use the default gpg agent.

## ENVIRONMENT ##

`GPG_AGENT_INFO`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Used to locate the gpg-agent. This is only used when `--noagent` is not set. The
value consists of 3 colon delimited fields: the path to the Unix Domain Socket,
the PID of the gpg-agent and the protocol version which should be set to 1.

## FILES ##

`~/.safely.db`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
The default password database file. The option `--db` can be used to override it.

`~/.safely.db~`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
The default password database backup file. If the `--db` option is set, this
will change accordingly. If the `--nobackup` option is used, this file is not
created/modified.

`~/.safely.db.lock`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
The default password database lock file. If the `--db` option is set, this
will change accordingly.

## AUTHOR ##

Alessandro Ghedini <al3xbio@gmail.com>

## COPYRIGHT ##

Copyright (C) 2011-2012 Alessandro Ghedini <al3xbio@gmail.com>

This program is released under the 3 clause BSD license.
