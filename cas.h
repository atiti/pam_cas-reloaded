#ifndef _H_CAS_H_
#define _H_CAS_H_

#include "url.h"


struct CAS {
	char CAS_URL[500];
	struct URL_Request u;
	char *service;
};

int CAS_init(struct CAS *c, char *CAS_URL, char *service);
int CAS_login(struct CAS *c, char *uname, char *pass);
int CAS_serviceValidate(struct CAS *c, char *ticket, char *uname);
int CAS_proxyValidate(struct CAS *c, char *ticket);
int CAS_cleanup(struct CAS *c);

#endif /* _H_CAS_H_ */
