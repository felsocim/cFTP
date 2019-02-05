CC = gcc
LEX = flex
YACC = bison -d
CFLAGS = -Wall -g
LDFLAGS = -lfl -ly -lm

client: bin prompt include/common.h include/main.h src/main.c
	$(CC) $(CFLAGS) obj/ftp.o src/lexer.c src/prompt.c src/main.c -o bin/client $(LDFLAGS)

prompt: ftp lexer include/common.h include/main.h src/prompt.y
	$(YACC) -o src/prompt.c --defines="include/prompt.h" src/prompt.y

lexer: include/common.h src/lexer.l
	$(LEX) -o src/lexer.c --header-file="include/lexer.h" src/lexer.l

ftp: obj include/common.h include/ftp.h src/ftp.c
	$(CC) $(CFLAGS) -c src/ftp.c -o obj/ftp.o

obj:
	test -d obj || mkdir obj

bin:
	test -d bin || mkdir bin

clean:
	rm -f obj bin
