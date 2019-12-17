
ReactorPath := ../../net/
Reactor := $(ReactorPath)EventLoop.o 	\
					 $(ReactorPATH)Channel.o \
					 $(ReactorPATH)EPoller.o \
					 ../../base/Thread.o

testConnection: testConnection.cpp ../HttpConnection.o 	\
								../../net/EventLoop.o ../../net/Channel.o 	\
								../../net/EPoller.o ../../base/Thread.o
	g++ $^ -o $@ 

client: client.cpp 
	g++ client.cpp -o client

parseURI: parseURI.cpp
	g++ $^ -o $@
	
parseHeader: parseHeader.cpp
	g++ $^ -o $@
	
analysis: analysis.cpp 
	g++ $^ -o $@
	
Server: Server.cpp
	g++ $(Reactor) $^ ../*.o -o $@ -lpthread
	