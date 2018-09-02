CC=gcc
CFLAGS=-Wall
LDFLAGS=-lcurl -lmyhtml

communicator: bin communicator.o utils.o continuum.o
	$(CC) communicator.o utils.o continuum.o -o bin/continuum $(LDFLAGS)

communicator.o:
	$(CC) -c src/communicator.c

utils.o:
	$(CC) -c src/utils.c

continuum.o:
	$(CC) -c src/continuum.c

bin:
	mkdir -p bin

clean:
	rm -rf bin/continuum
	rm -rf *.o
