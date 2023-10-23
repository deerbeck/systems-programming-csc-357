CC = gcc

CFLAGS = -g -Wall -ansi

LD = gcc

LDFLAGS =

OBJST = htable.o utility.o

all: htable
	
	
htable: $(OBJST)
	echo Building htable
	$(LD) $(LDFLAGS) -o htable $(OBJST)


htable.o: htable.c utility.h
	echo Building htable
	$(CC) $(CFLAGS) -c htable.c

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
