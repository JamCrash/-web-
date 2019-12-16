
Reactor := EventLoop.o EPoller.o Channel.o
all: $(Reactor)

EventLoop.o: EventLoop.cpp 
	g++ -c $^ -o $@

EPoller.o: EPoller.cpp
	g++ -c $^ -o $@
	
Channel.o: Channel.cpp 
	g++ -c $^ -o $@
	
cleanAll: 
	rm -f $(Reactor)
