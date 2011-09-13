CFLAGS=-Wall -pedantic -g
LDFLAGS=-ljansson

OBJS=src/db.o src/interface.o src/item.o src/main.o src/security.o

all: safely

safely: $(OBJS)
	cc $(CFLAGS) -o safely $(OBJS) $(LDFLAGS)

clean:
	rm -rf safely src/*.o

src/db.o: src/db.c \
	src/db.h
src/interface.o: src/interface.c
src/item.o: src/item.c \
	src/db.h
src/main.o: src/main.c \
	src/db.h \
	src/item.h \
	src/interface.h \
	src/security.h
src/security.o: src/security.c
