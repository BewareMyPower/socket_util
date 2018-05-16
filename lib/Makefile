ROOT = ..
include ${ROOT}/Make.defines

OBJECTS = signal.o sig_chld.o

all: $(LIB_SIG) $(OBJECTS)

$(LIB_SIG): $(OBJECTS)
	ar rcs $(LIB_SIG) $^

%.o: %.cc
	$(CC) $(FLAGS) -c $<

clean:
	rm -f *.o *.out $(LIB_SIG)
