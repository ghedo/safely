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

Before starting, note that a running gpg-agent is required for secret key
operations, which is generally a good thing since, this way, safely doesn't
need to know your GPG key's passphrase.

Here are a few usage examples:

 * Create a new database:

```bash
$ safely --create --keys <your_key_id>
```

 * Add a new account:

```bash
$ safely --add google.com --keys <your_key_id>
```

 * Show password of a given account:

```bash
$ safely --pass google.com
```

 * Save password to X clipboard:

```bash
$ safely --pass google.com | xclip -loops 1
```

 * Search accounts matching a string:

```bash
$ safely --search google
```

 * Remove account:

```bash
$ safely --remove google.com --keys <your_key_id>
```

See the [man page](http://ghedo.github.io/safely/) for more information.

## DEPENDENCIES

 * `libgpgme`

## BUILDING

safely is distributed as source code. Install with:

```bash
$ make
```

## COPYRIGHT

Copyright (C) 2014 Alessandro Ghedini <alessandro@ghedini.me>

See COPYING for the license.
