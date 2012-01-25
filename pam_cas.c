/*

Author: Attila Sukosd <attila@cc.dtu.dk>
License: BSD

Changelog:

- 25/Jan/2012 v0.2 CAS full user+pass login and serviceTicket login implementation
- 25/Jan/2012 v0.1 Initial version

*/

// CONFIGURATION


//#define CAS_DEBUG 1
#define MIN_TICKET_LEN 20
#define CAS_LOGIN_URL "http://www.mysite.com/cas/"
//#define CAS_SERVICE "http://mail.mysite.com/"
#define CAS_SERVICE "ssh"

// Support authentication against CAS
#define PAM_SM_AUTH

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <sys/param.h>

#include <security/pam_modules.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>

#include "url.h"
#include "cas.h"

int pam_sm_authenticate(pam_handle_t *pamhandle, int flags, int arg, const char **argv) {
	
	char *user, *pw;
        struct CAS cas;
        int ret = 0;

	if (pam_get_user(pamhandle, (const char**)&user, NULL) != PAM_SUCCESS) {
		syslog(LOG_ERR, "User does not exist!");
		return PAM_AUTH_ERR;
	}

	if (pam_get_item(pamhandle, PAM_OLDAUTHTOK, (const void**)&pw)  != PAM_SUCCESS) {
		syslog(LOG_ERR, "Cannot get the password!");
		return PAM_AUTH_ERR;
	}

	if (pw == NULL) {
		if (pam_get_item(pamhandle, PAM_AUTHTOK, (const void**)&pw) != PAM_SUCCESS) {
			syslog(LOG_ERR, "Cannot get  the password 2!");
			return PAM_AUTH_ERR;
		}
	}

#ifdef CAS_DEBUG
	syslog(LOG_NOTICE, "got user: %s pass: %s\n", user, pw);
#endif

	CAS_init(&cas, CAS_LOGIN_URL, CAS_SERVICE);


	if (strncmp(pw, "ST-", 3) == 0 && strlen(pw) > MIN_TICKET_LEN) { // Possibly serviceTicket?
		ret = CAS_serviceValidate(&cas, pw, user);		
	} else if (strncmp(pw, "PT-", 3) && strlen(pw) > MIN_TICKET_LEN) { // Possibly a proxyTicket?
		ret = CAS_proxyValidate(&cas, pw);
	} else {
        	ret = CAS_login(&cas, user, pw);
	}

#ifdef CAS_DEBUG
	if (ret > 0)
                syslog(LOG_INFO, "CAS user %s logged in successfully! ret: %d", user, ret);
        else
                syslog(LOG_INFO, "Failed to authenticate CAS user %s. ret: %d", user, ret);
#endif


        CAS_cleanup(&cas);
	
	if (ret > 0)
		return PAM_SUCCESS;

	return PAM_AUTH_ERR;
}

int pam_sm_setcred(pam_handle_t *pamhandle, int flags, int argc, const char **argv) {
	return PAM_SUCCESS;
}

