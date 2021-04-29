target: c_timeout

c_timeout: libinf.dylib c_timeout.c
	gcc -g -o c_timeout -L/Users/mike/MJ2K/c_timeout c_timeout.c -linf

libinf.dylib: inf_func.o
	gcc -g -shared -o libinf.dylib inf_func.o

inf_func.o: inf_func.h inf_func.c
	gcc -c inf_func.c

clean:
	rm -rf *.o
	rm -rf *.dylib
	rm -rf c_timeout
