target: read_j2k write_j2k rd_to_j2k

read_j2k: libmamj2k.dylib read_j2k.c
	gcc -g -o read_j2k \
		-I/usr/local/include/openjpeg-2.4 \
		-L/Users/mike/MJ2K \
		read_j2k.c -lmamj2k

write_j2k: libmamj2k.dylib write_j2k.c
	gcc -g -o write_j2k \
		-I/usr/local/include/openjpeg-2.4 \
		-L/Users/mike/MJ2K \
		write_j2k.c -lmamj2k

libmamj2k.dylib: mamj2k.o
	gcc -g -shared -o libmamj2k.dylib \
		-I/usr/local/include/openjpeg-2.4 \
		-L/usr/local/lib \
		mamj2k.o -lopenjp2

mamj2k.o: mamj2k.c mamj2k.h
	gcc -g -c -fpic \
		-I/usr/local/include/openjpeg-2.4 \
		mamj2k.c

rd_to_j2k: rd_to_j2k.c
	gcc -g -o rd_to_j2k\
		-I/Users/mike/MJ2K/openjpeg-2.4.0/src/lib/openjp2 \
		-I/Users/mike/MJ2K/openjpeg-2.4.0/build/src/lib/openjp2 \
		-L/Users/mike/MJ2K/openjpeg-2.4.0/build/bin \
		rd_to_j2k.c -lopenjp2

clean:
	@rm -f *.o
	@rm -f *.dylib
	@rm -f read_j2k write_j2k hello_test rd_to_j2k

hello_test: hello_test.c libhello.dylib
	gcc -o hello_test -L. hello_test.c -lhello

libhello.dylib: hello.o
	gcc -shared -o libhello.dylib hello.o

hello.o: hello.c hello.h
	gcc -c -fpic hello.c

