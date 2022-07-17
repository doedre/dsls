#ifndef __STATUS_H__
#define __STATUS_H__

#include "config.h"

extern char buf[];
extern char essid[];

const char *separator(const char *sep);

const char *null(void);

const char *datetime(const char *fmt);

const char *battery(void);

const char *cpu(void);

const char *ram(void);

const char *netstat(void);

#endif	// __STATUS_H__
