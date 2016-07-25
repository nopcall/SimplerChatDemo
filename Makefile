CC = gcc
CXXFLAGS = -std=c++11 -l stdc++ -pthread

all: server client

.PHONY: clean

clean:
	rm *.o

server: server.cc server.H
	${CC} ${CXXFLAGS} -o $@ $^

client: client.cc client.H
	${CC} ${CXXFLAGS} -o $@ $^
