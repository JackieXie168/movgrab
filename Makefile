# Generated automatically from Makefile.in by configure.
CC = gcc
VERSION = 1.0.2
CFLAGS = -g -O2
LIBS = 
FLAGS=$(CFLAGS)  -DSTDC_HEADERS=1 
INSTALL=/bin/install -c
prefix=/usr/local
bindir=$(prefix)${exec_prefix}/bin

all: 
	@cd libUseful-1.0; $(MAKE)
	$(CC) $(FLAGS) $(LIBS) -g -omovgrab main.c libUseful-1.0/libUseful-1.0.a

clean:
	@rm -f movgrab libUseful-1.0/*.o libUseful-1.0/*.a libUseful-1.0/*.so

install:
	$(INSTALL) movgrab $(bindir)
