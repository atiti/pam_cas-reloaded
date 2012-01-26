#include <string.h>
#include "config.h"

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

static int cfg_handler(void *user, const char *section, const char *name, const char *value) {
	CAS_configuration *cfg = (CAS_configuration *)user;

	if (MATCH("General", "SERVICE_URL")) {
		cfg->SERVICE_URL = strdup(value);
	} else if (MATCH("General", "CAS_BASE_URL")) {
		cfg->CAS_BASE_URL = strdup(value);
	} else if (MATCH("General", "ENABLE_ST")) {
		cfg->ENABLE_ST = atoi(value);
	} else if (MATCH("General", "ENABLE_PT")) {
		cfg->ENABLE_PT = atoi(value);
	} else if (MATCH("General", "ENABLE_UP")) {
		cfg->ENABLE_UP = atoi(value);
	} else
		return 0;
	return 1;
}

void init_config(CAS_configuration *c) {
	c->SERVICE_URL = NULL;
	c->CAS_BASE_URL = NULL;
	c->ENABLE_ST = 0;
	c->ENABLE_PT = 0;
	c->ENABLE_UP = 0;
}

int load_config(CAS_configuration *c, char *file) {
	init_config(c);

	if (ini_parse(file, cfg_handler, c) < 0) {
		return 0;
	}
	
	return 1;
}

