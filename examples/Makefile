ROOT = ..
CC = g++
FLAGS = -std=c++11
SOURCE = $(ROOT)/socket_util

LIBSOCKUTIL = $(SOURCE)/libsockutil.a
PROGS = simple-connect simple-accept

all: $(PROGS)

%: %.cc $(LIBSOCKUTIL)
	$(CC) $(FLAGS) -I$(SOURCE) $^ -o $@

$(LIBSOCKUTIL):
	cd $(SOURCE) && make && cd -

clean:
	rm -rf a.out $(PROGS)
