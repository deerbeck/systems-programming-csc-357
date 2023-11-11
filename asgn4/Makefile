CC = gcc

CFLAGS = -g -Wall

LD = gcc

LDFLAGS =

OBJS = mytar.o utility.o

all: mytar
	
mytar: $(OBJS)
	echo Building mytar
	$(LD) $(LDFLAGS) -o mytar $(OBJS)

mytar.o: mytar.c utility.h
	echo Building mytar object
	$(CC) $(CFLAGS) -c mytar.c

utility.o: utility.c utility.h
	echo Building utility
	$(CC) $(CFLAGS) -c utility.c

clean: 
	rm *.o

test: all
	echo Testing mytar
