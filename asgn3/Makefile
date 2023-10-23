CC = gcc

CFLAGS = -g -Wall -ansi

LD = gcc

LDFLAGS =

OBJSE = hencode.o utility.o
OBJSD = hdecode.o utility.o

all: hencode hdecode 
	
hencode: $(OBJSE)
	echo Building hencode
	$(LD) $(LDFLAGS) -o hencode $(OBJSE)

hdecode: $(OBJSD)
	echo Building hdecode
	$(LD) $(LDFLAGS) -o hdecode $(OBJSD)

hencode.o: hencode.c utility.h
	echo Building hencode
	$(CC) $(CFLAGS) -c hencode.c
	
hdecode.o: hdecode.c utility.h
	echo Building hdecode
	$(CC) $(CFLAGS) -c hdecode.c

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
