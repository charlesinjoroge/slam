.PHONY: all
all:
	g++ -I/home/daedalus/Documents -o server server.cpp -lboost_system -lboost_timer -lboost_chrono -lrt -w
clean:
	@rm server
	@rm client
client:
	g++ -o client tcpClient.cpp -lzmq -std=c++11 -l jsoncpp
echoServer:
	g++ -o echoServer echoServer.cpp -lzmq -std=c++11

