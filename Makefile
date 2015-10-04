CC = gcc
VERSION = 1.0.16
CFLAGS = -g -O2
LIBS = -lz 
INSTALL=/usr/bin/install -c
prefix=/usr/local
bindir=$(prefix)${exec_prefix}/bin
FLAGS=$(CFLAGS) -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DSTDC_HEADERS=1 -DHAVE_LIBZ=1 

all: 
	@cd libUseful-2.0; $(MAKE)
	$(CC) $(FLAGS) -o movgrab main.c libUseful-2.0/libUseful-2.0.a $(LIBS)
clean:
	@rm -f movgrab libUseful-2.0/*.o libUseful-2.0/*.a libUseful-2.0/*.so

install:
	$(INSTALL) movgrab $(bindir)
