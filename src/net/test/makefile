
Reactor := ../EventLoop.o ../EPoller.o ../Channel.o

testEventLoop: testEventLoop.cpp $(Reactor) \
							../../base/Thread.o
	g++ $^ -lpthread -o $@

testChannel: testChannel.cpp $(Reactor) \
							../../base/Thread.o
	g++ $^ -lpthread -o $@

testReactor: testReactor.cpp $(Reactor) \
						../../base/Thread.o
	g++ $^ -lpthread -o $@

testEventLoopThread: testEventLoopThread.cpp $(Reactor) ../EventLoopThread.o \
						../../base/Thread.o 
	g++ $^ -lpthread -o $@

testEventLoopThreadPool: testEventLoopThreadPool.cpp $(Reactor)	\
						../EventLoopThread.o ../EventLoopThreadPool.o ../../base/Thread.o
	g++ $^ -lpthread -o $@

server: server.cpp $(Reactor)	\
				../../base/Thread.o
	g++ $^ -lpthread -o $@

client: client.cpp $(Reactor)	\
				../../base/Thread.o
	g++ $^ -lpthread -o $@
	
