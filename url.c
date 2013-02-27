#include "url.h"

//#define URL_DEBUG 1
#define SKIP_PEER_VERIFICATION 1
#define SKIP_HOSTNAME_VERIFICATION 1

void init_string(struct string *s) {
	s->len = 0;
	s->ptr = malloc(s->len+1);
	if (s->ptr == NULL)
		return;
	
	s->ptr[0] = '\0';
}

void URL_init(struct URL_Request *u) {
	u->formpost = NULL;
	u->lastptr = NULL;
	u->headerlist = NULL;
	u->post_data = NULL;
	u->redirect_url = NULL;
	
	u->curl = curl_easy_init();
}

void URL_add_form(struct URL_Request *u, char *name, char *content) {
	
	// creating x-www-urlencoded string
	char *ptr,
	   *encontent;
	encontent = curl_easy_escape(u->curl, content, strlen(content));
	
	int plus_length = strlen(name) + strlen(encontent) + 1;
	
	if (NULL == u->post_data)
    	u->post_data = (char *) calloc(plus_length + 1, sizeof(char));
	else
    	u->post_data = (char *) realloc(u->post_data, (strlen(u->post_data) + plus_length + 2) * sizeof(char));

	ptr = u->post_data + strlen(u->post_data);
	if (ptr != u->post_data)
    	*ptr++ = '&';
	memcpy(ptr, name, strlen(name) * sizeof(char));
	ptr[strlen(name)] = '=';
	ptr += strlen(name) + 1;
	memcpy(ptr, content, strlen(encontent) * sizeof(char));
	ptr[strlen(encontent)] = '\0';

	free(encontent);

	// adding data for multipart/form-data
	curl_formadd(&u->formpost, &u->lastptr, CURLFORM_COPYNAME, name, CURLFORM_COPYCONTENTS, content, CURLFORM_END);
}

void URL_add_header(struct URL_Request *u, char *str) {
	u->headerlist = curl_slist_append(u->headerlist, str);	
}

size_t URL_writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
	size_t new_len = s->len + size*nmemb;
	s->ptr = realloc(s->ptr, new_len+1);
	if (s->ptr == NULL)
		return -1;
	memcpy(s->ptr+s->len, ptr, size*nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;
	
	return size*nmemb;
}

int URL_GET_request(struct URL_Request *u, char *url, struct string *out) {
	int len = 0;
#ifdef URL_DEBUG
	LOG_MSG(LOG_INFO, "GET URL: %s\n", url);
#endif
	
	curl_easy_setopt(u->curl, CURLOPT_URL, url);
	curl_easy_setopt(u->curl, CURLOPT_WRITEFUNCTION, URL_writefunc);
	curl_easy_setopt(u->curl, CURLOPT_WRITEDATA, out);
	curl_easy_setopt(u->curl, CURLOPT_FOLLOWLOCATION, 1); 

#ifdef SKIP_PEER_VERIFICATION
	curl_easy_setopt(u->curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
	curl_easy_setopt(u->curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif		

    curl_easy_setopt(u->curl, CURLOPT_COOKIEFILE, "");

	u->res = curl_easy_perform(u->curl);

	return len; 
}

int URL_POST_request(struct URL_Request *u, char *url, struct string *out) {
        int len = 0;
        curl_easy_setopt(u->curl, CURLOPT_URL, url);

#ifdef URL_DEBUG
	LOG_MSG(LOG_INFO, "POST URL:\t%s\n", url);
#endif

	if (u->formpost != NULL) {
//		curl_easy_setopt(u->curl, CURLOPT_HTTPPOST, u->formpost);
		curl_easy_setopt(u->curl, CURLOPT_POST, 1);
		curl_easy_setopt(u->curl, CURLOPT_POSTFIELDS, u->post_data);
	}
	
	if (u->headerlist != NULL)
		curl_easy_setopt(u->curl, CURLOPT_HTTPHEADER, u->headerlist);	

		curl_easy_setopt(u->curl, CURLOPT_FOLLOWLOCATION, 1); 
        curl_easy_setopt(u->curl, CURLOPT_WRITEFUNCTION, URL_writefunc);
        curl_easy_setopt(u->curl, CURLOPT_WRITEDATA, out);
#ifdef SKIP_PEER_VERIFICATION
        curl_easy_setopt(u->curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
        curl_easy_setopt(u->curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

        u->res = curl_easy_perform(u->curl);
        curl_easy_getinfo(u->curl, CURLINFO_EFFECTIVE_URL, &u->redirect_url);

        return len;
}

void URL_cleanup(struct URL_Request *u) {
	if (u->headerlist != NULL)
		curl_slist_free_all(u->headerlist);
	if (u->formpost != NULL)
		curl_formfree(u->formpost);
	curl_easy_cleanup(u->curl);
}


