CC = gcc
VERSION = 1.0.8
CFLAGS = -g -O2
LIBS = -lcrypto -lssl -lz 
INSTALL=/bin/install -c
prefix=/usr/local
bindir=$(prefix)${exec_prefix}/bin
FLAGS=$(CFLAGS) -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DSTDC_HEADERS=1 -DHAVE_LIBZ=1 -DHAVE_LIBSSL=1 -DHAVE_LIBCRYPTO=1 

all: 
	@cd libUseful-1.0; $(MAKE)
	$(CC) $(FLAGS) $(LIBS) -g -omovgrab main.c libUseful-1.0/libUseful-1.0.a

clean:
	@rm -f movgrab libUseful-1.0/*.o libUseful-1.0/*.a libUseful-1.0/*.so

install:
	$(INSTALL) movgrab $(bindir)
