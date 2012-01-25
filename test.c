#include <stdio.h>
#include <stdlib.h>

#include "cas.h"
#include "url.h"


int main(int argv, char **argc) {
	struct CAS cas;
	int ret = 0;

	// Initialize the CAS backend
	CAS_init(&cas, "https://myserver/cas/", "ssh");

	// Attempt a full login with user/pass
	ret = CAS_login(&cas, "myuser", "weakpass");

	// Ret > 1 means we are authenticated 
	if (ret > 1)
		printf("We are now authenticated!\n");
	else
		printf("Failed to authenticate!\n");
	// Clean up resources
	CAS_cleanup(&cas);

	return 0;
}
