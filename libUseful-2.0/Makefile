# Generated automatically from Makefile.in by configure.
CC = gcc
VERSION = 0.0.1
CFLAGS = -g -O2
LIBS = -lz 
FLAGS=$(CFLAGS) -fPIC  -DSTDC_HEADERS=1 -DHAVE_LIBZ=1  
prefix=/usr/local
OBJ=string.o list.o socket.o unix_socket.o file.o GeneralFunctions.o DataProcessing.o EncryptedFiles.o ConnectManager.o sound.o pty.o Log.o http.o inet.o expect.o base64.o crc32.o md5c.o sha1.o Hash.o Compression.o


all: $(OBJ)
	$(CC) $(FLAGS) $(LIBS) -shared -o libUseful-2.0.so $(OBJ)
	#ld -i -o libUseful-2.0.a $(OBJ)
	ar rcs libUseful-2.0.a $(OBJ)

string.o: string.h string.c
	$(CC) $(FLAGS) -c string.c

list.o: list.h list.c
	$(CC) $(FLAGS) -c list.c

socket.o: socket.h socket.c
	$(CC) $(FLAGS) -c socket.c

sound.o: sound.h sound.c
	$(CC) $(FLAGS) -c sound.c

pty.o: pty.h pty.c
	$(CC) $(FLAGS) -c pty.c

http.o: http.h http.c
	$(CC) $(FLAGS) -c http.c


Log.o: Log.h Log.c
	$(CC) $(FLAGS) -c Log.c

unix_socket.o: unix_socket.h unix_socket.c
	$(CC) $(FLAGS) -c unix_socket.c

file.o: file.h file.c
	$(CC) $(FLAGS) -c file.c

inet.o: inet.h inet.c
	$(CC) $(FLAGS) -c inet.c

expect.o: expect.h expect.c
	$(CC) $(FLAGS) -c expect.c

GeneralFunctions.o: GeneralFunctions.h GeneralFunctions.c
	$(CC) $(FLAGS) -c GeneralFunctions.c

DataProcessing.o: DataProcessing.h DataProcessing.c
	$(CC) $(FLAGS) -c DataProcessing.c

EncryptedFiles.o: EncryptedFiles.h EncryptedFiles.c
	$(CC) $(FLAGS) -c EncryptedFiles.c

Hash.o: Hash.h Hash.c
	$(CC) $(FLAGS) -c Hash.c

Compression.o: Compression.h Compression.c
	$(CC) $(FLAGS) -c Compression.c

ConnectManager.o: ConnectManager.h ConnectManager.c
	$(CC) $(FLAGS) -c ConnectManager.c

base64.o: base64.c base64.h
	$(CC) $(FLAGS) -c base64.c

crc32.o: crc32.c crc32.h
	$(CC) $(FLAGS) -c crc32.c

md5c.o: md5c.c md5-global.h md5.h
	$(CC) $(FLAGS) -c md5c.c

sha1.o: sha1.c sha1.h
	$(CC) $(FLAGS) -c sha1.c

clean:
	@rm -f *.o *.so *.a

install:
	@ln -s libUseful-2.0.so libUseful.so
	@ln -s libUseful-2.0.a libUseful.a
	@cp *.so *.a $(prefix)/lib ; mkdir $(prefix)/include/libUseful-$(VERSION) ; cp *.h $(prefix)/include/libUseful-$(VERSION)
