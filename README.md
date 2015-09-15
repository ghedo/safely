safely
======

![Travis CI](https://secure.travis-ci.org/ghedo/safely.png)

**safely** is a command-line tool to manage passwords. It stores all your
login information (usernames and passwords) in a simple JSON-formatted
file, encrypted and signed using your OpenPGP key: this makes it pretty
secure and allows manual decryption of the database when needed.

## Features

 * Simple database format and schema (to ease manual manipulation)
 * Secure, GPG-based encryption which allows manual decryption (see above)
 * Fool-proof command-line based and non interactive interface
 * Support for generating google-authenticator compatible 2-factor
   authentication tokens (TOTP).

## Getting Started

Before starting, note that a running gpg-agent is required for secret key
operations, which is generally a good thing since this way safely doesn't
need to know your GPG key's passphrase.

You may also want to add the following definition to your `gpg.conf`:

```
default-key <your_key_id>
```

Where `<your_key_id>` is the ID of your key. This way the `--keys <your_key_id>`
argument used below can be omitted.

## Usage

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

 * Generate a 2-factor authentication token (you need to provide the 2-factor
   secret key during --add or --edit):

```bash
$ safely --2fa google.com
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

## Building

safely is distributed as source code. Build with:

```bash
$ make
```

## COPYRIGHT

Copyright (C) 2014 Alessandro Ghedini <alessandro@ghedini.me>

See COPYING for the license.
