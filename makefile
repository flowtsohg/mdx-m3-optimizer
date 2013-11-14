CC=g++
CFLAGS=-c -Wall -O1 -I include

all: copt

copt: common.o main.o m3.o mdx.o
	$(CC) common.o main.o m3.o mdx.o -o copt
	strip copt

common.o: src/common.cpp
	$(CC) $(CFLAGS) src/common.cpp

main.o: src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp

m3.o: src/m3.cpp
	$(CC) $(CFLAGS) src/m3.cpp

mdx.o: src/mdx.cpp
	$(CC) $(CFLAGS) src/mdx.cpp

clean:
	rm -rf *o copt