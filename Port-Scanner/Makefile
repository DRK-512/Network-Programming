CC = g++
CFLAGS = -g -lsfml-network -lsfml-system -Iinclude
FILES = ./src/functions.cpp ./src/main.cpp

all: portscanner

portscanner: 
	${CC} ${FILES} ${CFLAGS} -o portscanner
	
clean: 
	rm portscanner
