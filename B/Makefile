CC=g++
CFLAGS=-c -g -Wall -std=c++11
LDFLAGS=-lpthread

all: trapmap

trapmap: main.o trapezoid_map.o
	$(CC) $(LDFLAGS) -o trapmap main.o trapezoid_map.o

main.o: main.cpp structures.h
	$(CC) $(CFLAGS) main.cpp -o main.o

trapezoid_map.o: trapezoid_map.cpp  structures.h
	$(CC) $(CFLAGS) trapezoid_map.cpp -o trapezoid_map.o

clean:
	rm -f *.o trapmap
