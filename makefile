CC=gcc
CFLAGS=-Wall -g
CLIBS=

client: bin ftp src/main.c
	$(CC) $(CFLAGS) obj/common.o obj/ftp.o src/main.c -o bin/client $(CLIBS)

ftp: common include/ftp.h src/ftp.c
	$(CC) $(CFLAGS) -c src/ftp.c -o obj/ftp.o

common: obj include/common.h src/common.c
	$(CC) $(CFLAGS) -c src/common.c -o obj/common.o

obj:
	test -d obj || mkdir obj

bin:
	test -d bin || mkdir bin

clean:
	rm -f obj/* bin/*
