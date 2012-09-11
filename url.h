#ifndef _H_URL_
#define _H_URL_

#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <syslog.h>

struct string {
	char *ptr;
	size_t len;
};

struct URL_Request {
	CURL *curl;
	CURLcode res;
	struct curl_httppost *formpost;
	struct curl_httppost *lastptr;
	struct curl_slist *headerlist;
};

void URL_init(struct URL_Request *u);
void init_string(struct string *s);
void URL_add_form(struct URL_Request *u, char *name, char *val);
void URL_add_header(struct URL_Request *u, char *header);
int URL_GET_request(struct URL_Request *u, char *url, struct string *s);
int URL_POST_request(struct URL_Request *u, char *url, struct string *s);
void URL_cleanup(struct URL_Request *u);

#endif

