ROOT = ..
CC = g++
FLAGS = -std=c++11 -Wno-unused-result -g
SOURCE = $(ROOT)/socket_util

LIBSOCKUTIL = $(SOURCE)/libsockutil.a
PROGS = poller-server multi-client

all: $(PROGS)

%: %.cc $(LIBSOCKUTIL)
	$(CC) $(FLAGS) -I$(SOURCE) $^ -pthread -o $@

$(LIBSOCKUTIL):
	cd $(SOURCE) && make && cd -

clean:
	rm -rf a.out $(PROGS)
