CC = gcc
CFLAGS = -Wall -Wextra -g

simple_compiler: lexer.o main.o parser.o dstr.o emit.o list.o
	$(CC) $(CFLAGS) lexer.o main.o parser.o emit.o dstr.o list.o -o simple_compiler

lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

dstr.o: dstr.c dstr.h
	$(CC) $(CFLAGS) -c dstr.c

emit.o: emit.c emit.h
	$(CC) $(CFLAGS) -c emit.c

list.o: list.c list.h
	$(CC) $(CFLAGS) -c list.c
