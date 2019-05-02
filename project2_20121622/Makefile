#Makefile for TINY

CC = gcc
CFLAGS = -Wall

OBJS = cm.tab.o lex.yy.o main.o util.o
TARGET = 20121622

$(TARGET): $(OBJS)
				$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lfl

cm.tab.o: cm.tab.c cm.tab.h
				$(CC) $(CFLAGS) -c cm.tab.c
lex.yy.o: lex.yy.c
				$(CC) $(CFLAGS) -c lex.yy.c
lex.yy.c: tiny.l globals.h util.h scan.h
				flex tiny.l
cm.tab.h:
cm.tab.c: cm.y
				bison -d -v cm.y
main.o: main.c globals.h util.h scan.h cm.tab.h
				$(CC) $(CFLAGS) -c main.c
util.o: util.c util.h globals.h
				$(CC) $(CFLAGS) -c util.c

clean:
				rm -rf $(TARGET) $(OBJS)

all: $(TARGET)
