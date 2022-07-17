#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "status.h"
#include "types.h"

#define STATUS_LEN 1024
#define MAX_ELEM_LEN 32

static const uint64_t wait = 1000000;

static const char *def_val = "n/a";

static const struct functor args[] = {
	{ netstat,	"%s",		NULL	},
	{ cpu,		"%4s%%",	NULL	},
	{ ram, 		"%5s",		NULL	},
	{ battery,	"%s%%",		NULL	},
	{ datetime,	" %s",		"%a %d %b%T"	},
};

#endif	// __CONFIG_H__
