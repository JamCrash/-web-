
#ReactorPath := ../../net/
Reactor := ../../net/EventLoop.o ../../net/EPoller.o ../../net/Channel.o	\
					../../base/Thread.o

testConnection: testConnection.cpp ../HttpConnection.o 	../HttpData.o \
								$(Reactor)
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

testServer: testServer.cpp
	g++ $(Reactor) $^ ../HttpServer.o -o $@ -lpthread
	