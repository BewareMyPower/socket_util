ROOT = ..
include $(ROOT)/Make.defines

OBJECTS = daytimecli daytimesrv tcpechosrv tcpechocli

all: $(OBJECTS) $(LIB_SIG)

%: %.cc $(INC)/ipv4_socket.hpp $(LIB_SIG)
	$(CC) $(FLAGS) $< $(LIB_SIG) -o $@

clean:
	rm -rf $(OBJECTS)

include $(ROOT)/Make.inc
