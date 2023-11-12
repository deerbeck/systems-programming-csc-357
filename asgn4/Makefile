CC = gcc

CFLAGS = -g -Wall

LD = gcc

LDFLAGS =

OBJS = mytar.o utility.o create_util.o list_util.o extract_util.o

all: mytar
	
mytar: $(OBJS)
	echo Building mytar
	$(LD) $(LDFLAGS) -o mytar $(OBJS)

mytar.o: mytar.c utility.h create_util.h list_util.h extract_util.h
	echo Building mytar object
	$(CC) $(CFLAGS) -c mytar.c

utility.o: utility.c utility.h
	echo Building utility
	$(CC) $(CFLAGS) -c utility.c

create_util.o: create_util.c create_util.h utility.h
	echo Building create_util
	$(CC) $(CFLAGS) -c create_util.c

list_util.o: list_util.c list_util.h utility.h
	echo Building list_util
	$(CC) $(CFLAGS) -c list_util.c

extract_util.o: extract_util.c extract_util.h utility.h
	echo Building extract_util
	$(CC) $(CFLAGS) -c extract_util.c

clean: 
	rm *.o

test: all
	echo Testing mytar
