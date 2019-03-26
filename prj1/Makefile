#Makefile for TINY

CC = gcc

OBJS = main.o util.o lex.yy.o
TARGET = 20121622

$(TARGET): $(OBJS)
				$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c globals.h util.h scan.h
				$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h globals.h
				$(CC) $(CFLAGS) -c util.c

lex.yy.o: lex.yy.c util.h globals.h scan.h
				$(CC) $(CFLAGS) -c lex.yy.c

lex.yy.c: tiny.l 
				flex tiny.l

clean:
				rm -rf $(TARGET) $(OBJS)

all: $(TARGET)
