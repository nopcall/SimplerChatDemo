CC = gcc
CXXFLAGS = -std=c++11 -l stdc++ -pthread -g \
	-I/usr/include/mysql -I/usr/include/mysql++ -lmysqlpp -lmysqlclient

all: server client

.PHONY: clean

clean:
	rm *.o

server: server.cc server.H Timer.o
	${CC} ${CXXFLAGS} -o $@ $^

client: client.cc client.H
	${CC} ${CXXFLAGS} -o $@ $^

Timer.o: Timer.cc
	${CC} ${CXXFLAGS} -c $^
