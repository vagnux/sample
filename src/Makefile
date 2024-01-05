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
top_srcdir = ..
prefix = /usr/local
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin


CC = gcc
CFLAGS = -g -O2
CPPFLAGS = 
EXTRACPPFLAGS = -I$(top_srcdir)
DEFS = -DHAVE_CONFIG_H
LIBS = -lnsl  
LDFLAGS = 
LIBTOOL = $(CC)

SOURCES = ample.c entries.c options.c sqlite.c md5.c mysql.c database.c configloader.c
OBJECTS = ample.o entries.o options.o sqlite.o md5.o  mysql.o database.o configloader.o
COMPILE = $(CC) $(CFLAGS) $(CPPFLAGS) $(EXTRACPPFLAGS) $(DEFS)
LINK = $(LIBTOOL) $(LDFLAGS) $(LIBS)

all: ample

ample: $(OBJECTS)
	$(LINK) $(OBJECTS) -o $@

.c.o: 
	$(COMPILE) -c $<

clean:
	rm -f *.o ample

maint-clean:
	rm -f *.o *~ Makefile ample

dist-clean:
	rm -f *.o *~ Makefile ample
	rm -rf CVS

.PHONY: all clean maint-clean dist-clean

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
