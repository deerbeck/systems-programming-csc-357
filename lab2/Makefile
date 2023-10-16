CC = gcc

CFLAGS = -g -Wall -ansi

LD = gcc

LDFLAGS=

all: uniq 
	
uniq: main.o read_long_line.o
	echo Building uniq
	$(LD) $(LDFLAGS) -o uniq main.o read_long_line.o 

main.o: main.c
	echo Building main
	$(CC) $(CFLAGS) -c -o main.o main.c
	
read_long_line.o: read_long_line.c
	echo Building read_long_line
	$(CC) $(CFLAGS) -c -o read_long_line.o read_long_line.c

clean: 
	rm main.o read_long_line.o

test: all
	echo Testing uniq
	./uniq <input >output
	uniq <input >reference
	diff output reference
	echo done.
