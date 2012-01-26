#include <stdio.h>
#include <stdlib.h>

#include "cas.h"
#include "url.h"
#include "config.h"

int main(int argv, char **argc) {
	struct CAS cas;
	int ret = 0;
	CAS_configuration c;
		
	ret = load_config(&c, "/etc/pam_cas.conf");
	if (!ret) {
		printf("Failed to load configuration!");
		exit(0);
	}

	printf("Loaded config:\n");
	printf("BASE_URL = %s\n", c.CAS_BASE_URL);
	printf("SERVICE_URL = %s\n", c.SERVICE_URL);
	printf("Enable serviceValidate = %d\n", c.ENABLE_ST);
	printf("Enable proxyValidate = %d\n", c.ENABLE_PT);
	printf("Enable user+pass = %d\n", c.ENABLE_UP);


	// Initialize the CAS backend
	CAS_init(&cas, c.CAS_BASE_URL, c.SERVICE_URL);

	// Attempt a full login with user/pass
	ret = CAS_login(&cas, "myuser", "mypass");

	// Ret > 1 means we are authenticated 
	if (ret > 0)
		printf("We are now authenticated!\n");
	else
		printf("Failed to authenticate!\n");
	// Clean up resources
	CAS_cleanup(&cas);

	return 0;
}
