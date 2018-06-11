CC = g++
FLAGS = -std=c++11 -Wall -DNULL=nullptr

LIB_SOCK_UTIL = libsockutil.a
OBJECTS = inet_address.o socket_base.o

all: $(LIB_SOCK_UTIL) $(OBJECTSJ)

$(LIB_SOCK_UTIL): $(OBJECTS)
	ar -rcs $@ $^

inet_address.o: inet_address.cc inet_address.h
	$(CC) $(FLAGS) -c $< -o $@

socket_base.o: socket_base.cc socket_base.h noncopyable.hpp
	$(CC) $(FLAGS) -c $< -o $@

test: test.cc $(LIB_SOCK_UTIL)
	$(CC) $(FLAGS) $^ -o $@

clean:
	rm -rf a.out $(LIB_SOCK_UTIL) $(OBJECTS)
