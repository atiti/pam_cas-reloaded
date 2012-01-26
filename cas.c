#include "cas.h"

//#define DEBUG 1

int CAS_init(struct CAS *c, char *CAS_URL, char *service) {
	strcpy(c->CAS_URL, CAS_URL);
	URL_init(&c->u);	
	c->service = service;
	return 1;
}

/* This is one nasty function to parse HTML/XML code to with, and the best part is: no need for a complex parser!
  */
int CAS_find_part(struct string *s, char *startmatch, char endchar, char *dest, int dstsize) {
	char *ptr = NULL;
	char *sp = NULL;
	char *ep = NULL;
	ptr = strstr(s->ptr, startmatch);
	if (ptr) {
		sp = ptr + strlen(startmatch);
		ep = strchr(sp, endchar);
		if ((ep-sp) < dstsize) {
			strncpy(dest, sp, ep-sp);
			return 1;
		}
		return -1;
	}
	return 0;
}

int CAS_find_loginticket(struct string *s, char *ticket, int size) {
	return CAS_find_part(s, "id=\"lt\" value=\"" , '"', ticket, size);
}
int CAS_find_serviceticket(struct string *s, char *ticket, int size) {
	return CAS_find_part(s, "?ticket=", '\'', ticket, size);
}

int CAS_find_user(struct string *s, char *user, int size) {
	return CAS_find_part(s, "<cas:user>", '<', user, size);
}


int CAS_login(struct CAS *c, char *uname, char *pass) {
	char URL[1000];
	char lt[512];
	int ret = 0;
	struct string content;
	init_string(&content);
	
	if (c->service != NULL)
		sprintf(URL, "%s/login?service=%s", c->CAS_URL, c->service);
	else
		sprintf(URL, "%s/login", c->CAS_URL);	

	URL_GET_request(&c->u, URL, &content);

	ret = CAS_find_loginticket(&content, lt, 512);
	free(content.ptr);
	content.len = 0;

	if (!ret) { 
#ifdef DEBUG
		fprintf(stderr, "Could not get login ticket!\n");
#endif
		return -1;
	}

#ifdef DEBUG
	printf("Got LT: %s\n", lt);
	printf("Using service: %s\n", c->service);
#endif

	init_string(&content);

	URL_add_form(&c->u, "username", uname);
	URL_add_form(&c->u, "password", pass);
	URL_add_form(&c->u, "lt", lt);
	if (c->service)
		URL_add_form(&c->u, "service", c->service);


	URL_POST_request(&c->u, URL, &content); 

	ret = CAS_find_serviceticket(&content, lt, 512);
	free(content.ptr);
	content.len = 0;	
	if (!ret) {
#ifdef DEBUG
		fprintf(stderr, "Could not get service ticket!\n");
#endif
		return -2;
	}

#ifdef DEBUG
	fprintf(stderr, "Successfully logged in!\n");
#endif

	ret = CAS_serviceValidate(c, lt, uname);

	return ret;
}

int CAS_serviceValidate(struct CAS *c, char *ticket, char *u) {
	char URL[1000];
	char user[512];
	int ret = 0;
        struct string content;
        init_string(&content);

	if (c->service != NULL)
		sprintf(URL, "%s/serviceValidate?service=%s&ticket=%s", c->CAS_URL, c->service, ticket);
	else
		sprintf(URL, "%s/serviceValidate?ticket=%s", c->CAS_URL, ticket);

        URL_GET_request(&c->u, URL, &content);
	
	ret = CAS_find_user(&content, user, 512);
	if (ret) {
		if (u != NULL) { // User comparison, strictly speaking not needed, but better be safe than sorry
			if (strncmp(user, u, strlen(user)) == 0)
				return 1;
			else
				return 0;
		}
		return 2;
	} 
	free(content.ptr);
	return 0;
}

int CAS_proxyValidate(struct CAS *c, char *ticket, char *u) {
        char URL[1000];
        char user[512];
        int ret = 0;
        struct string content;
        init_string(&content);

        if (c->service != NULL)
                sprintf(URL, "%s/proxyValidate?service=%s&ticket=%s", c->CAS_URL, c->service, ticket);
        else
                sprintf(URL, "%s/proxyValidate?ticket=%s", c->CAS_URL, ticket);

        URL_GET_request(&c->u, URL, &content);

        ret = CAS_find_user(&content, user, 512);
        if (ret) {
                if (u != NULL) {
                        if (strncmp(user, u, strlen(user)) == 0)
                                return 1;
                        else
                                return 0;
                }
                return 2;
        }
        free(content.ptr);
        return 0;
}

int CAS_cleanup(struct CAS *c) {
	URL_cleanup(&c->u);	
	return 1;
}


