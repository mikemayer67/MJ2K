ifeq ($(shell uname),Linux)
  EXT  = so
else
  EXT  = dylib
endif

all: accumulator_test

clean:
	@rm -f *.o
	@rm -f *.$(EXT)
	@rm -f accumulator_test

accumulator_test: accumulator_test.c libaccumulator.$(EXT)
	gcc -o accumulator_test -L. accumulator_test.c -laccumulator

libaccumulator.$(EXT): accumulator.o
	gcc -shared -o libaccumulator.$(EXT) accumulator.o

accumulator.o: accumulator.c accumulator.h
	gcc -c -fpic accumulator.c

