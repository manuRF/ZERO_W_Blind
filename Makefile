
server_objects = ServerSocket.o Socket.o server.o TSL2561.o

all : server 

server: $(server_objects)
	g++ -o server $(server_objects) -lwiringPi -lrt -llirc_client

Socket: Socket.cpp
ServerSocket: ServerSocket.cpp
server: server.cpp

clean:
	rm -f *.o server 