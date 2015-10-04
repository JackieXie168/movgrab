CC=gcc
all: 
	@cd libUseful-1.0; make
	$(CC) -g -omovgrab main.c libUseful-1.0/libUseful-1.0.sa

make clean:
	@rm -f movgrab libUseful-1.0/*.o libUseful-1.0/*.sa libUseful-1.0/*.so
