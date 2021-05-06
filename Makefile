ifeq ($(shell uname),Linux)
  MJ2K = /home/mamayer/workbench/MJ2K
  OPJ  = $(MJ2K)/openjpeg-2.4.0
  EXT  = so
else
  MJ2K = /Users/mike/MJ2K
  OPJ  = $(MJ2K)/openjpeg-2.4.0
  EXT  = dylib
endif

INC += -I$(OPJ)/src/lib/openjp2
INC += -I$(OPJ)/build/src/lib/openjp2
LIB += -L$(OPJ)/build/bin
LIB += -L$(MJ2K)

LIBMAMJ2K = libmamj2k.$(EXT)

target: read_j2k write_j2k rd_to_j2k

read_j2k: $(LIBMAMJ2K) read_j2k.c
	gcc -g -o read_j2k $(INC) $(LIB) read_j2k.c -lmamj2k

write_j2k: $(LIBMAMJ2K) write_j2k.c
	gcc -g -o write_j2k $(INC) $(LIB) write_j2k.c -lmamj2k

rd_to_j2k: rd_to_j2k.c
	gcc -g -o rd_to_j2k $(INC) $(LIB) rd_to_j2k.c -lopenjp2

$(LIBMAMJ2K): mamj2k.o
	gcc -g -shared -o $(LIBMAMJ2K) $(INC) $(LIB) mamj2k.o -lopenjp2

mamj2k.o: mamj2k.c mamj2k.h
	gcc -g -c -fpic $(INC) mamj2k.c

clean:
	@rm -f *.o
	@rm -f *.$(EXT)
	@rm -f read_j2k write_j2k rd_to_j2k hello_test

hello_test: hello_test.c libhello.$(EXT)
	gcc -o hello_test -L. hello_test.c -lhello

libhello.$(EXT): hello.o
	gcc -shared -o libhello.$(EXT) hello.o

hello.o: hello.c hello.h
	gcc -c -fpic hello.c

