CC = gcc

CFLAGS = -g -Wall -ansi

LD = gcc

LDFLAGS =

MAIN = hencode

OBJS = hencode.o utility.o

all: $(MAIN)
	
$(MAIN): $(OBJS)
	echo Building $(MAIN)
	$(LD) $(LDFLAGS) -o $(MAIN) $(OBJS)

hencode.o: hencode.c utility.h
	echo Building hencode
	$(CC) $(CFLAGS) -c hencode.c
	
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
