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
LIB += -L$(MJ2K)/src
LIB += -L$(MJ2K)

J2KSRC = $(shell find j2k -name '*.cc')
J2KOBJ = $(J2KSRC:.cc=.o)

LIBMAMJ2K = libmamj2k.$(EXT)

TARGETS = read_j2k write_j2k rd_to_j2k j2k_to_rd rd_to_j2k_cs j2k_parser

target: $(TARGETS)

read_j2k: $(LIBMAMJ2K) read_j2k.c
	gcc -g -o $@ $(INC) $(LIB) read_j2k.c -lmamj2k

write_j2k: $(LIBMAMJ2K) write_j2k.c
	gcc -g -o $@ $(INC) $(LIB) write_j2k.c -lmamj2k

rd_to_j2k: rd_to_j2k.c
	gcc -g -o $@ $(INC) $(LIB) $^ -lopenjp2

j2k_to_rd: j2k_to_rd.c
	gcc -g -o $@ $(INC) $(LIB) $^ -lopenjp2

rd_to_j2k_cs: rd_to_j2k_cs.c
	gcc -g -o $@ $(INC) $(LIB) $^ -lopenjp2

j2k_parser: $(J2KOBJ)
	g++ -g $(J2KOBJ) -o $@

$(LIBMAMJ2K): mamj2k.o
	gcc -g -shared -o $(LIBMAMJ2K) $(INC) $(LIB) mamj2k.o -lopenjp2

mamj2k.o: mamj2k.c mamj2k.h
	gcc -g -c -fpic $(INC) mamj2k.c

clean:
	@rm -f *.o j2k/*.o
	@rm -f *.$(EXT)
	@rm -f $(TARGETS)
	@rm -rf *.dSYM
	@echo All build products removed from $(MJ2K)/src

install: $(TARGETS) $(LIBMAMJ2K)
	@install -m 755 $(TARGETS) $(MJ2K)
	@install -m 644 mamj2k.h $(MJ2K)
	@install -m 644 $(LIBMAMJ2K) $(MJ2K)
	@echo Libraries, Executables, and Headers installed into $(MJ2K)

uninstall:
	@rm -f $(foreach tgt, $(TARGETS), $(MJ2K)/$(tgt))
	@rm -f $(MJ2K)/mamj2k.h
	@rm -f $(MJ2K)/$(LIBMAMJ2K)
	@echo Libraries, Executables, and Headers removed from $(MJ2K)

clobber: clean uninstall

