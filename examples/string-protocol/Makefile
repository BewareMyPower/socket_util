ROOT = ../..
CC = g++
FLAGS = -std=c++11 -Wno-unused-result -g
SOURCE = $(ROOT)/socket_util

LIBSOCKUTIL = $(SOURCE)/libsockutil.a
PROGS = server client

all: $(PROGS)

%: %.cc $(LIBSOCKUTIL) protocol.h
	$(CC) $(FLAGS) -I$(SOURCE) $< $(LIBSOCKUTIL) -pthread -o $@

$(LIBSOCKUTIL):
	cd $(SOURCE) && make && cd -

clean:
	rm -rf a.out $(PROGS)
