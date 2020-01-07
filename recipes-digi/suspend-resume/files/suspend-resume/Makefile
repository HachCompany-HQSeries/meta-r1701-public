#CC = gcc
CFLAGS = -Wall -g -D_GNU_SOURCE

all: suspram

suspram: suspram.o


%.o: %.c %.h

.PHONY: clean
clean:
	rm -f *.o suspram
