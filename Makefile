CFLAGS=-g -Wall -O3 -fPIC
LDFLAGS=-lcurl

all:
	gcc $(CFLAGS) -c pam_cas.c
	gcc $(CFLAGS) -c cas.c
	gcc $(CFLAGS) -c url.c
	gcc $(CFLAGS) -c ini.c
	gcc $(CFLAGS) -c config.c
	gcc -shared $(LDFLAGS) -o pam_cas.so pam_cas.o cas.o url.o ini.o config.o -L/usr/lib/x86_64-linux-gnu -lcurl
test:
	gcc $(CFLAGS) -c test.c
	gcc $(CFLAGS) -c test-pt.c
	gcc $(CFLAGS) -c cas.c
	gcc $(CFLAGS) -c url.c
	gcc $(CFLAGS) -c ini.c
	gcc $(CFLAGS) -c config.c
	gcc $(CFLAGS) $(LDFLAGS) -o test test.o cas.o url.o ini.o config.o -L/usr/lib/x86_64-linux-gnu -lcurl
	gcc $(CFLAGS) $(LDFLAGS) -o test-pt test-pt.o cas.o url.o ini.o config.o -L/usr/lib/x86_64-linux-gnu -lcurl
install:
	cp pam_cas.so /lib/security/

clean:
	rm *.o *.so test test-pt
