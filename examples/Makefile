ROOT = ..
CC = g++
FLAGS = -g -std=c++11 #-O2
SOURCE = $(ROOT)/socket_util

LIBSOCKUTIL = $(SOURCE)/libsockutil.a
PROGS = oob-client oob-server address-test cgi-server simple-client \
		writev sendfile splice echo-client

all: $(PROGS)

%: %.cc $(LIBSOCKUTIL)
	$(CC) $(FLAGS) -I$(SOURCE) $^ -o $@

$(LIBSOCKUTIL):
	cd $(SOURCE) && make && cd -

clean:
	rm -rf a.out $(PROGS)
