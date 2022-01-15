# Makefile modified from Hui Yuan (https://youtu.be/bYRZVG9ea5M)
# Might have to change location of driver.cpp file as per your file location

CC=g++
CFLAGS=-c -Wall -g
LDFLAGS = -lpq

all: driver

driver: driver.o
	$(CC) driver.o $(LDFLAGS) -o driver -std=c++11
driver.o: driver.cpp
	$(CC) $(CFLAGS) driver.cpp -o driver.o

clean:
	rm -f *.o driver
