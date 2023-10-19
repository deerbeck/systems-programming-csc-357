CC = gcc

CFLAGS = -g -Wall -ansi

LD = gcc

LDFLAGS =

MAIN = uniq

OBJS = main.o read_long_line.o

all: $(MAIN)
	
$(MAIN): main.o read_long_line.o
	echo Building $(MAIN)
	$(LD) $(LDFLAGS) -o $(MAIN) $(OBJS)

main.o: main.c read_long_line.h
	echo Building main
	$(CC) $(CFLAGS) -c main.c
	
read_long_line.o: read_long_line.c read_long_line.h
	echo Building read_long_line
	$(CC) $(CFLAGS) -c read_long_line.c

clean: 
	rm *.o

test: all
	echo Testing $(MAIN)
	./$(MAIN) <input >output
	$(MAIN) <input >reference
	diff output reference
	echo done.
