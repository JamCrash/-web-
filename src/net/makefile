
Reactor := EventLoop.o EPoller.o Channel.o
all: $(Reactor) EventLoopThread.o

EventLoop.o: EventLoop.cpp 
	g++ -c $^ -o $@

EPoller.o: EPoller.cpp
	g++ -c $^ -o $@
	
Channel.o: Channel.cpp 
	g++ -c $^ -o $@

EventLoopThread.o: EventLoopThread.cpp
	g++ -c $^ -o $@
	
EventLoopThreadPool.o: EventLoopThreadPool.cpp
	g++ -c $^ -o $@

cleanAll: 
	rm -f $(Reactor)
