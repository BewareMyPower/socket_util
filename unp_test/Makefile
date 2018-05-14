include ../Make.defines

OBJECTS = daytimecli daytimesrv tcpechosrv tcpechocli

all: $(OBJECTS)

%: %.cc $(INC)/ipv4_socket.hpp
	$(CC) $(FLAGS) $< -o $@

clean:
	rm $(OBJECTS)
