CC = gcc

CFLAGS = -g -Wall -ansi

LD = gcc

LDFLAGS =

MAIN = huff-e

OBJS = huff-e.o utility.o

all: $(MAIN)
	
$(MAIN): $(OBJS)
	echo Building $(MAIN)
	$(LD) $(LDFLAGS) -o $(MAIN) $(OBJS)

huff-e.o: huff-e.c utility.h
	echo Building huff-e
	$(CC) $(CFLAGS) -c huff-e.c
	
utility.o: utility.c utility.h
	echo Building utility
	$(CC) $(CFLAGS) -c utility.c

clean: 
	rm *.o


test: all
	echo Testing $(MAIN)
	./$(MAIN) <input >output
	$(MAIN) <input >reference
	diff output reference
	echo done.
