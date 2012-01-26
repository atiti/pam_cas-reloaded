CFLAGS=-g -Wall -O3 -fPIC
LDFLAGS=-lcurl

all:
	gcc $(CFLAGS) -c pam_cas.c
	gcc $(CFLAGS) -c cas.c
	gcc $(CFLAGS) -c url.c
	gcc $(CFLAGS) -c ini.c
	gcc $(CFLAGS) -c config.c
	ld -shared $(LDFLAGS) -o pam_cas.so pam_cas.o cas.o url.o ini.o config.o
test:
	gcc $(CFLAGS) -c test.c
	gcc $(CFLAGS) -c cas.c
	gcc $(CFLAGS) -c url.c
	gcc $(CFLAGS) -c ini.c
	gcc $(CFLAGS) -c config.c
	gcc $(CFLAGS) $(LDFLAGS) -o test test.o cas.o url.o ini.o config.o

install:
	cp pam_cas.so /lib/security/

clean:
	rm *.o *.so test
