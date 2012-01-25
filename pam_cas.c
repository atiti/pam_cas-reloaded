/*

Author: Attila Sukosd <attila@cc.dtu.dk>
License: BSD

Changelog:
- v0.1 Initial version


*/

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


int pam_sm_authenticate(pam_handle_t *pamhandle, int flags, int arg, const char **argv) {
	
	char *user, *pw;
	
	if (pam_get_user(pamhandle, (const char**)&user, NULL) != PAM_SUCCESS) {
		syslog(LOG_ERR, "User does not exist!");
		return PAM_AUTH_ERR;
	}

	if (pam_get_item(pamhandle, PAM_AUTHTOK, (const void**)&pw)  != PAM_SUCCESS) {
		syslog(LOG_ERR, "Cannot get the password!");
		return PAM_AUTH_ERR;
	}
	
	syslog(LOG_NOTICE, "got user: %s pass: %s\n", user, pw);
	return PAM_AUTH_ERR; //PAM_SUCCESS;
}

int pam_sm_setcred(pam_handle_t *pamhandle, int flags, int argc, const char **argv) {
	return PAM_SUCCESS;
}

static int _get_authtok(pam_handle_t *pamhandle) {
	return PAM_SUCCESS;
}


