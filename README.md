Safely
======

**Safely** is a command-line tool to manage passwords. It stores all your login
information (usernames and passwords) in simple, JSON-formatted file, encrypted
and signed using your OpenPGP key: this makes it pretty secure and allows manual
decryption of the database when needed.

safely was implemented with the following design goals in mind:

 * Simple database format and schema (to ease manual manipulation)
 * Secure, GPG-based encryption which allows manual decryption (see above)
 * Fool-proof command-line based and non interactive interface
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
$ safely --search google
~~~~

 * Remove account:

~~~~
$ safely --remove google.com
~~~~

## SECURITY

safely checks, at startup, that the following conditions are true:

 * Running as non-root
 * Core dumps are disabled
 * Memory is locked (via `mlockall()`)
 * `ptrace()` is disabled
 * stdin, stdout, stderr are valid

If any of them fails, safely refuses to work, unless the **--nosecure** option
is used (see the manpage for more info). Note that the ptrace and memlock checks
need to be run as suid (mode 4755) to pass; if `make install` is executed (with
root privileges) everything should be ok.

## DEPENDENCIES

 * `gpgme`
 * `jansson`
 * `liblockfile` (optional)

## BUILDING

Safely is distributed as source code. Install with:

~~~~
$ git clone git://github.com/AlexBio/Safely.git && cd Safely
$ cmake .
$ make
# make install
~~~~

## COPYRIGHT

Copyright (C) 2011 Alessandro Ghedini <al3xbio@gmail.com>

See COPYING for the license.
