# Securely Makefile
# Copyright (C) 2011 Alessandro Ghedini <al3xbio@gmail.com>
# This file is released under the BSD license, see the COPYING file

INSTALL=install
RM=rm

CFLAGS=-Wall -pedantic -g
LDFLAGS=-ljansson -lgpgme

PREFIX=$(DESTDIR)/usr

BINDIR?=$(PREFIX)/bin
MANDIR?=$(PREFIX)/share/man/man1

OBJS=src/db.o src/gpg.o src/interface.o src/item.o src/main.o src/security.o

all: safely

safely: $(OBJS)
	$(CC) $(CFLAGS) -o safely $(OBJS) $(LDFLAGS)

install:
	$(INSTALL) -m 4755 -o 0 -g 0 ./safely $(BINDIR)/safely

clean:
	$(RM) -rf safely src/*.o

src/db.o: src/db.c \
	src/db.h \
	src/interface.h
src/gpg.o: src/gpg.c \
	src/interface.h \
	src/security.h
src/interface.o: src/interface.c \
	src/interface.h
src/item.o: src/item.c \
	src/db.h \
	src/item.h \
	src/interface.h
src/main.o: src/main.c \
	src/db.h \
	src/item.h \
	src/interface.h \
	src/security.h
src/security.o: src/security.c \
	src/security.h \
	src/interface.h
