target: read_j2k write_j2k

read_j2k: libmamj2k.dylib read_j2k.c
	gcc -o read_j2k \
		-I/usr/local/anaconda3/include/openjpeg-2.3 \
		-L/Users/mike/MJ2K \
		read_j2k.c -lmamj2k

write_j2k: libmamj2k.dylib write_j2k.c
	gcc -o write_j2k \
		-I/usr/local/anaconda3/include/openjpeg-2.3 \
		-L/Users/mike/MJ2K \
		write_j2k.c -lmamj2k

libmamj2k.dylib: mamj2k.o
	gcc -shared -o libmamj2k.dylib \
		-I/usr/local/anaconda3/include/openjpeg-2.3 \
		-L/usr/local/anaconda3/lib \
		mamj2k.o -lopenjp2

mamj2k.o: mamj2k.c mamj2k.h
	gcc -c -fpic \
		-I/usr/local/anaconda3/include/openjpeg-2.3 \
		mamj2k.c

clean:
	@rm -f *.o
	@rm -f *.dylib
	@rm -f read_j2k hello_test

hello_test: hello_test.c libhello.dylib
	gcc -o hello_test -L. hello_test.c -lhello

libhello.dylib: hello.o
	gcc -shared -o libhello.dylib hello.o

hello.o: hello.c hello.h
	gcc -c -fpic hello.c

