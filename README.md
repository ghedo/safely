Safely
======

![status](http://stillmaintained.com/AlexBio/Safely.png)

**safely** is a command-line based, tool to manage password in an extremely
easy to use and secure way. The passwords database is a simple GnuPG encrypted
JSON document; this makes manual access and manipulation of the database very
simple.

Its configuration is entirely handled via environment variables in order
to avoid yet another dotfile under your $HOME. Every config option is optional,
and safely provides sane default values for them.

safely was implemented with the following "design goals" in mind:

 * Simple database format and schema (to ease manual manipulation)
 * Secure, GPG-based encryption which allows manual decryption (see above)
 * Simple, command-line based and non interactive interface
 * Featureless ("do one thing, do it well")

## GETTING STARTED

 * Create a new database:

~~~~
$ safely --create
~~~~

 * Add a new account:

~~~~
$ safely --add google.com
~~~~

 * Show password of a given account:

~~~~
$ safely --passwd google.com
~~~~

 * Save password to X clipboard:

~~~~
$ safely --passwd google.com | xclip -loops 1
~~~~

 * Search accounts matching to a string:

~~~~
$ safely --search '.com'
~~~~

 * Remove account:

~~~~
$ safely --remove 'google.com'
~~~~

## SECURITY

safely checks, at startup, that the following conditions are true:

 * Running as non-root
 * Core dump are disabled
 * Memory is locked (via `mlockall()`)
 * ptrace() is not working
 * stdin, stdout, stderr are valid

If any of them fails, safely refuses to work. Note that the ptrace() and memlock
checks need to be run as euid 0 (mode 4755). If `make install` is executed (with
root priviledges) everything will be ok.

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
