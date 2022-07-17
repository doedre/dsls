#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>

const char *bfmt_human(char *buf, uint64_t num, int base);

const char *fmt_human(uint64_t num, int base);

#endif	// __UTIL_H__
