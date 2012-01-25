CFLAGS=-g -Wall -O3 -fPIC

all:
	gcc $(CFLAGS) -c pam_cas.c
	ld -x --shared -o pam_cas.so pam_cas.o
clean:
	rm *.o *.so
