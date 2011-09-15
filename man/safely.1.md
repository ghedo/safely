safely(1) -- secure and simple command-line password store
==========================================================

## SYNOPSIS

`safely [COMMAND] args...`

## DESCRIPTION

**safely** is a command-line based, tool to manage password in an extremely
easy to use and secure way. The passwords database is a simple GnuPG encrypted
JSON document; this makes manual access and manipulation of the database very
simple.

safely's configuration is entirely handled via environment variables in order
to avoid yet another dotfile under your $HOME. Every config option is optional,
and safely provides sane default values for them.

## COMMANDS ##

`--create`
&nbsp;&nbsp;&nbsp;Create a new password db.

`--add`
&nbsp;&nbsp;&nbsp;Add a new account.

`--passwd`
&nbsp;&nbsp;&nbsp;Show given account's password.

`--user`
&nbsp;&nbsp;&nbsp;Show given account's username.

`--remove`
&nbsp;&nbsp;&nbsp;Remove given account.

`--search`
&nbsp;&nbsp;&nbsp;Remove given account.

`--dump`
&nbsp;&nbsp;&nbsp;Dump JSON database.

`--help`
&nbsp;&nbsp;&nbsp;Show the help.

## ENVIRONMENT ##

The following environment variables affect safely behaviour:

 * **SAFELY_DB** Specifies the path to the password database to be used by
   safely (default **~/.safely.db**).

## BUGS ##

Please report any bugs or feature requests to GitHub issues tracker at
`<http://github.com/AlexBio/Safely/issues>`.

## AUTHOR ##

Alessandro Ghedini <al3xbio@gmail.com>

## COPYRIGHT ##

Copyright (C) 2011 Alessandro Ghedini <al3xbio@gmail.com>

This program is released under the BSD license (see `COPYING`).
