Safely
======

![status](http://stillmaintained.com/AlexBio/Safely.png)

**safely** is a command-line based, tool to manage password in an extremely
easy to use and secure way. The passwords database is a simple GnuPG encrypted
JSON document; this makes manual access and manipulation of the database very
simple.

safely's configuration is entirely handled via environment variables in order
to avoid yet another dotfile under your $HOME. Every config option is optional,
and safely provides sane default values for them.

It was implemented with the following "design goals" in mind:

 * Secure, GPG-based encryption (see next point)
 * Simple database format and schema (to ease manual manipulation)
 * Simple, command-line based and non interactive interface
 * Featureless ("do one thing, do it well")

## GETTING STARTED

Create a new database:

~~~~
$ safely --create
[ok] Running as non-root
[ok] Disabled core dumps
[ok] Memory locked
[ok] Protection from ptrace()
[ok] Valid stdin, stdout, stderr
Enter password for GPG key:
[ok] Database '/home/ale/.safely.db' created
~~~~

Add a new account:

~~~~
$ safely --add google.com
[...]
Enter user name  [google.com]: user
Enter password   [google.com]:
[...]
[ok] Added new item
~~~~

Show password/username of an account:

~~~~
$ safely --passwd google.com
[...]
mygooglepassword
~~~~

Save password to X clipboard:

~~~~
$ safely --passwd google.com | xclip -loops 1
[...]
~~~~

Search accounts matching to a string:

~~~~
$ safely --search '.com'
[...]
instapaper.com
github.com
google.com
[ok] 3 items match
~~~~

Remove account:

~~~~
$ safely --remove 'google.com'
[...]
[ok] Removed item
~~~~

## SECURITY

safely checks, at startup, that the following conditions are true:

 * Running as non-root
 * Core dump are disabled
 * Memory is locked (via `mlockall()`)
 * ptrace() is not working
 * stdin, stdout, stderr are valid

If any of them fails, safely refuses to work. Note that the ptrace() and memlock
checks need to be run as euid 0 (mode 4755).

## DEPENDENCIES

 * `gpgme`
 * `jansson`

## BUILDING

Securely is distributed as source code. Install with:

~~~~
$ git clone git://github.com/AlexBio/Securely.git && cd Securely
$ make
# make install
~~~~

## COPYRIGHT

Copyright (C) 2011 Alessandro Ghedini <al3xbio@gmail.com>

See COPYING for the license.
