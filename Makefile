# safely Makefile
# Copyright (C) 2014 Alessandro Ghedini <alessandro@ghedini.me>
# This file is released under the 2 clause BSD license, see COPYING

export GOPATH:=$(GOPATH):$(CURDIR)

BUILDTAGS=debug

all: safely

safely:
	go get -tags '$(BUILDTAGS)' -d -v main/safely
	go install -tags '$(BUILDTAGS)' main/safely

vet:
	go vet ./...

man: docs/safely.1.md
	ronn -r $<

html: docs/safely.1.md
	ronn -h $<

release-all: BUILDTAGS=release
release-all: all

clean:
	go clean -i main/safely db gpg oath util

.PHONY: all safely deps clean
