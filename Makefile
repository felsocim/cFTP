CC = gcc
LEX = flex
YACC = bison -d
CFLAGS = -Wall -g
LDFLAGS = -lfl -ly -lm

client: bin prompt include/main.h src/main.c
	$(CC) $(CFLAGS) obj/common.o obj/ftp.o src/lexer.c src/prompt.c src/main.c -o bin/client $(LDFLAGS)

prompt: common ftp lexer include/main.h src/prompt.y
	$(YACC) -o src/prompt.c --defines="include/prompt.h" src/prompt.y

lexer: common src/lexer.l
	$(LEX) -o src/lexer.c src/lexer.l

ftp: common include/ftp.h src/ftp.c
	$(CC) $(CFLAGS) -c src/ftp.c -o obj/ftp.o

common: obj include/common.h src/common.c
	$(CC) $(CFLAGS) -c src/common.c -o obj/common.o

obj:
	test -d obj || mkdir obj

bin:
	test -d bin || mkdir bin

clean:
	rm -f obj bin
