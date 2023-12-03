CC = gcc

CFLAGS = -g -Wall

LD = gcc

LDFLAGS = -g -Wall

OBJS = mush2.o utility.o

LIBS = -lmush

all: mush2
	
mush2: $(OBJS)
	echo Building mush2
	$(LD) $(LDFLAGS) -o mush2 -L ~/Documents/systems-programming-csc-357/asgn6/Mush/lib64 $(OBJS) $(LIBS)

mush2.o: mush2.c utility.h 
	echo Building mush2 object
	$(CC) $(CFLAGS) -c -I ~/Documents/systems-programming-csc-357/asgn6/Mush/include mush2.c

utility.o: utility.c utility.h
	echo Building utility
	$(CC) $(CFLAGS) -c -I ~/Documents/systems-programming-csc-357/asgn6/Mush/include utility.c

clean: 
	rm *.o

test: all
	echo Testing mush2
