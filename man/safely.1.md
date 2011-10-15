safely(1) -- secure and simple command-line password store
==========================================================

## SYNOPSIS

`safely COMMAND [ACCOUNT]`

## DESCRIPTION

**safely** is a command-line based, tool to manage password in an extremely
easy to use and secure way. The passwords database is a simple GnuPG encrypted
JSON document; this makes manual access and manipulation of the database very
simple.

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

`--remove, -r`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Remove given account.

`-s, --search`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Remove the given account.

`-d, --dump`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Dump JSON database.

`-h, --help`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Show the help.

## ENVIRONMENT ##

The following environment variables affect safely behaviour:

**SAFELY_DB**

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Specifies a custom path to the password database.

**SAFELY_NOBACKUP**

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
If set, a backup of the db is not created.

**SAFELY_NOSECURE**

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
If set, any security test failure is ignored.

## AUTHOR ##

Alessandro Ghedini <al3xbio@gmail.com>

## COPYRIGHT ##

Copyright (C) 2011 Alessandro Ghedini <al3xbio@gmail.com>

This program is released under the 3 clause BSD license.
