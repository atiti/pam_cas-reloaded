/*

Author: Attila Sukosd <attila@cc.dtu.dk>
License: BSD

Changelog:

- 26/Jan/2012 v0.3 Configuration file support added
- 25/Jan/2012 v0.2 CAS full user+pass login and serviceTicket login implementation
- 25/Jan/2012 v0.1 Initial version

*/

// CONFIGURATION
#define CAS_DEBUG 1
#define MIN_TICKET_LEN 20
#define CAS_CONFIG_FILE "/etc/pam_cas.conf"

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
#include "config.h"

int pam_sm_authenticate(pam_handle_t *pamhandle, int flags, int arg, const char **argv) {
	
	char *user, *pw;
        struct CAS cas;
	CAS_configuration c;
        int ret = 0;

	if (pam_get_user(pamhandle, (const char**)&user, NULL) != PAM_SUCCESS) {
		LOG_MSG(LOG_ERR, "User does not exist!");
		return PAM_AUTH_ERR;
	}

	if (pam_get_item(pamhandle, PAM_OLDAUTHTOK, (const void**)&pw)  != PAM_SUCCESS) {
		LOG_MSG(LOG_ERR, "Cannot get the password!");
		return PAM_AUTH_ERR;
	}

	if (pw == NULL) {
		if (pam_get_item(pamhandle, PAM_AUTHTOK, (const void**)&pw) != PAM_SUCCESS) {
			LOG_MSG(LOG_ERR, "Cannot get  the password 2!");
			return PAM_AUTH_ERR;
		}
	}

	if (pw == NULL) {
		LOG_MSG(LOG_ERR, "Did not get password, check the PAM configuration!");
		return PAM_AUTH_ERR;
	}

#ifdef CAS_DEBUG
//	LOG_MSG(LOG_NOTICE, "Got user: %s pass: %s\n", user, pw);
#endif

        ret = load_config(&c, CAS_CONFIG_FILE);
        if (!ret) {
                LOG_MSG(LOG_ERR,  "Failed to load configuration at %s!", CAS_CONFIG_FILE);
                return PAM_AUTH_ERR;
        }

	CAS_init(&cas, c.CAS_BASE_URL, c.SERVICE_URL, c.SERVICE_CALLBACK_URL);

	if (c.ENABLE_ST && strncmp(pw, "ST-", 3) == 0 && strlen(pw) > MIN_TICKET_LEN) { // Possibly serviceTicket?
#ifdef CAS_DEBUG
		LOG_MSG(LOG_INFO, "serviceTicket found. Doing serviceTicket validation!");
#endif
		ret = CAS_serviceValidate(&cas, pw, user);		
	} else if (c.ENABLE_PT && strncmp(pw, "PT-", 3) == 0 && strlen(pw) > MIN_TICKET_LEN) { // Possibly a proxyTicket?
#ifdef CAS_DEBUG
		LOG_MSG(LOG_INFO, "proxyTicket found. Doing proxyTicket validation!");
#endif
		ret = CAS_proxyValidate(&cas, pw, user);
	} else if (c.ENABLE_PT && strncmp(pw, "PGT-",4) == 0 && strlen(pw) > MIN_TICKET_LEN) { // Possibly a proxy granting ticket
#ifdef CAS_DEBUG
		LOG_MSG(LOG_INFO, "pgTicket found. Doing proxy-ing and proxyTicket validation!");
		ret = CAS_proxy(&cas, pw, user);
#endif
	} else if (c.ENABLE_UP) {
#ifdef CAS_DEBUG
		LOG_MSG(LOG_INFO, "user+pass combo login!");
#endif
        	ret = CAS_login(&cas, user, pw);
	}

#ifdef CAS_DEBUG
	if (ret > 0)
                LOG_MSG(LOG_INFO, "CAS user %s logged in successfully! ret: %d", user, ret);
        else
                LOG_MSG(LOG_INFO, "Failed to authenticate CAS user %s. ret: %d", user, ret);
#endif


        CAS_cleanup(&cas);
	
	if (ret > 0)
		return PAM_SUCCESS;

	return PAM_AUTH_ERR;
}

int pam_sm_acct_mgmt(pam_handle_t *pamhandle, int flags, int arg, const char **argv) {
    return PAM_SUCCESS;
}

int pam_sm_setcred(pam_handle_t *pamhandle, int flags, int argc, const char **argv) {
	return PAM_SUCCESS;
}

