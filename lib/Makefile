include ../Make.defines

LIB_SIG = libsig.a
OBJECTS = signal.o

all: $(LIB_SIG) $(OBJECTS)

$(LIB_SIG): signal.o
	ar rcs $(LIB_SIG) $?

signal.o: signal.cc
	$(CC) $(FLAGS) -c $<

clean:
	rm -f *.o *.out
