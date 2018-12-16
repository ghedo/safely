# safely Makefile
# Copyright (C) 2014 Alessandro Ghedini <alessandro@ghedini.me>
# This file is released under the 2 clause BSD license, see COPYING

man: docs/safely.1.md
	ronn -r $<

html: docs/safely.1.md
	ronn -h $<
