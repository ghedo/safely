Safely
======

![Travis CI](https://secure.travis-ci.org/AlexBio/Safely.png)

**Safely** is a command-line tool to manage passwords. It stores all your
login information (usernames and passwords) in a simple, JSON-formatted
file, encrypted and signed using your OpenPGP key: this makes it pretty
secure and allows manual decryption of the database when needed.

safely was implemented with the following design goals in mind:

 * Simple database format and schema (to ease manual manipulation)
 * Secure, GPG-based encryption which allows manual decryption (see above)
 * Fool-proof command-line based and non interactive interface
 * Featureless ("do one thing, do it well")

## GETTING STARTED

 * Create a new database:

```
$ safely --create
```

 * Add a new account:

```
$ safely --add google.com
```

 * Show password of a given account:

```
$ safely --passwd google.com
```

 * Save password to X clipboard:

```
$ safely --passwd google.com | xclip -loops 1
```

 * Search accounts matching to a string:

```
$ safely --search google
```

 * Remove account:

```
$ safely --remove google.com
```

See the [man page](http://alexbio.github.com/Safely/safely.1.html) for more information.

## SECURITY

safely checks, at startup, that the following conditions are true:

 * Running as non-root
 * Core dumps are disabled
 * Memory is locked (via `mlockall()`)
 * stdin, stdout, stderr are valid

If any of them fails, safely refuses to work, unless the **--nosecure**
option is used (see the man page for more info).

## DEPENDENCIES

 * `gpgme`
 * `jansson`
 * `liblockfile` (optional)

## BUILDING

Safely is distributed as source code. Install with:

```
$ mkdir build && cd build
$ cmake ..
$ make
# make install
```

## COPYRIGHT

Copyright (C) 2011-2012 Alessandro Ghedini <al3xbio@gmail.com>

See COPYING for the license.
