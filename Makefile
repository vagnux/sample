# Generated automatically from Makefile.in by configure.
# Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh

srcdir = .
top_srcdir = .
prefix = /usr/local
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin


CC = gcc
CFLAGS = -g -O2
CPPFLAGS = 
DEFS = -DHAVE_CONFIG_H
LIBS = -lnsl  -lmpg123 -lssl -lcrypto -lsqlite3 -pthread -lmysqlclient

LDFLAGS = 

SUBDIRS = src

MDEFS = 'SHELL=$(SHELL)' 'prefix=$(prefix)' 'exec_prefix=$(exec_prefix)' \
	'bindir=$(bindir)' 'MAKE=$(MAKE)' 'CC=$(CC)' 'CFLAGS=$(CFLAGS)' \
	'DEFS=$(DEFS)' 'LIBS=$(LIBS)' 'LDFLAGS=$(LDFLAGS)' 'CPPFLAGS=$(CPPFLAGS)'

all:
	@for i in $(SUBDIRS); do \
	  echo "Making all in $$i ..."; \
	  cd $$i; $(MAKE) $(MDEFS) all; \
	  cd $(top_srcdir); \
	done

clean:
	@for i in $(SUBDIRS); do \
	  echo "Making clean in $$i ..."; \
	  cd $$i; $(MAKE) clean; \
	  cd $(top_srcdir); \
	done

maint-clean:
	@for i in $(SUBDIRS); do \
	  echo "Making maint-clean in $$i ..."; \
	  cd $$i; $(MAKE) maint-clean; \
	  cd $(top_srcdir); \
	done
	rm -f config.cache config.h config.log config.status *~ Makefile

dist-clean:
	@for i in $(SUBDIRS); do \
	  echo "Making dist-clean in $$i ..."; \
	  cd $$i; $(MAKE) dist-clean; \
	  cd $(top_srcdir); \
	done
	rm -f config.cache config.h config.log config.status *~ Makefile
	rm -rf CVS

.PHONY: all clean dist-clean maint-clean

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
