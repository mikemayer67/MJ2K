ifeq ($(shell uname),Linux)
  EXT  = so
else
  EXT  = dylib
endif

all: hello_test

clean:
	@rm -f *.o
	@rm -f *.$(EXT)
	@rm -f hello_test

hello_test: hello_test.c libhello.$(EXT)
	gcc -o hello_test -L. hello_test.c -lhello

libhello.$(EXT): hello.o
	gcc -shared -o libhello.$(EXT) hello.o

hello.o: hello.c hello.h
	gcc -c -fpic hello.c

