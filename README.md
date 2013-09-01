safely
======

![Travis CI](https://secure.travis-ci.org/ghedo/safely.png)

**safely** is a command-line tool to manage passwords. It stores all your
login information (usernames and passwords) in a simple, JSON-formatted
file, encrypted and signed using your OpenPGP key: this makes it pretty
secure and allows manual decryption of the database when needed.

safely was implemented with the following design goals in mind:

 * Simple database format and schema (to ease manual manipulation)
 * Secure, GPG-based encryption which allows manual decryption (see above)
 * Fool-proof command-line based and non interactive interface
 * Featureless ("do one thing, do it well")

## GETTING STARTED

Before starting, you may want to add the following config to your `gpg.conf`:

```
default-key <MYKEY>
default-recipient-self
```

Where `<MYKEY>` is the ID of your key. Otherwise the `--keys` option must be
provided for the following commands.

 * Create a new database:

```bash
$ safely --create
```

 * Add a new account:

```bash
$ safely --add google.com
```

 * Show password of a given account:

```bash
$ safely --passwd google.com
```

 * Save password to X clipboard:

```bash
$ safely --passwd google.com | xclip -loops 1
```

 * Search accounts matching to a string:

```bash
$ safely --search google
```

 * Remove account:

```bash
$ safely --remove google.com
```

See the [man page](http://ghedo.github.io/safely/) for more information.

## SECURITY

safely checks, at startup, that the following conditions are true:

 * Running as non-root
 * Core dumps are disabled
 * Memory is locked (via `mlockall()`)

If any of them fails, safely refuses to work, unless the **--nosecure**
option is used (see the man page for more info).

## DEPENDENCIES

 * `gpgme`
 * `jansson`
 * `liblockfile` (optional)

## BUILDING

safely is distributed as source code. Install with:

```bash
$ mkdir build && cd build
$ cmake ..
$ make
$ [sudo] make install
```

## COPYRIGHT

Copyright (C) 2011-2012 Alessandro Ghedini <alessandro@ghedini.me>

See COPYING for the license.
