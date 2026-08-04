CC ?= cc
CFLAGS ?= -O3
CPPFLAGS ?=
WARNFLAGS = -Wall -Wextra -Wpedantic -Wconversion -Wshadow
PREFIX ?= $(HOME)/local
BINDIR ?= $(PREFIX)/bin

.PHONY: all clean install

all: dupes

dupes: dupes.c dupes.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(WARNFLAGS) -std=c11 dupes.c -o $@

install: dupes
	mkdir -p "$(BINDIR)"
	install -m 755 dupes "$(BINDIR)/dupes"

clean:
	$(RM) dupes
