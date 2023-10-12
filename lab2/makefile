CC = gcc

CFLAGS = -g -Wall -ansi

LD = gcc

LDFLAGS=

all: uniq 
	
uniq: main.o
	echo Building uniq
	$(LD) $(LDFLAGS) -o uniq main.o

main.o: main.c
	echo Building main
	$(CC) $(CFLAGS) -c -o main.o main.c

clean: 
	rm main.o

test: all
	echo Testing uniq
	./uniq input output
	uniq <input >reference
	diff output reference
	echo done.
