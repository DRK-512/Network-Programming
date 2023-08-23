CC = g++
CFLAGS = -g -lsfml-network -lsfml-system -Iinclude 
FILES = ./src/functions.cpp ./src/main.cpp

all: networkscanner

networkscanner: 
	${CC} ${FILES} ${CFLAGS} -o networkscanner
	
clean: 
	rm networkscanner
