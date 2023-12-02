CC = gcc

CFLAGS = -g -Wall

LD = gcc

LDFLAGS =

all: forkit tryit
	
forkit: forkit.o
	echo Building forkit
	$(LD) $(LDFLAGS) -o forkit forkit.o


forkit.o: forkit.c 
	echo Building forkit
	$(CC) $(CFLAGS) -c forkit.c


tryit: tryit.o
	echo Building tryit
	$(LD) $(LDFLAGS) -o tryit tryit.o


tryit.o: tryit.c 
	echo Building tryit
	$(CC) $(CFLAGS) -c tryit.c

clean: 
	rm *.o
