CC = g++
INC = ../include
FLAGS = -std=c++11
OBJECTS = daytimecli daytimesrv

all: $(OBJECTS)

%: %.cc $(INC)/ipv4_socket.hpp
	$(CC) $(FLAGS) $< -o $@

clean:
	rm $(OBJECTS)
