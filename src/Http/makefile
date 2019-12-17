
Reactor := ../net/EventLoop.o ../net/Channel.o

HttpConnection.o: HttpConnection.cpp
	g++ -c $^ -o $@

HttpData.o: HttpData.cpp
	g++ -c $^ -o $@
	
all: HttpData.o HttpConnection.o
