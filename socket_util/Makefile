CC = g++
FLAGS = -std=c++11 -Wall -g

LIB_SOCKUTIL = libsockutil.a
OBJECTS = inet_address.o inet_socket.o poller.o

all: $(LIB_SOCKUTIL)

$(LIB_SOCKUTIL): $(OBJECTS)
	ar -rcs $@ $^

%.o: %.cc
	$(CC) $(FLAGS) -c $<

clean:
	rm -rf $(OBJECTS) $(LIB_SOCKUTIL) a.out
