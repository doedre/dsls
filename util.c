#include <stdio.h>
#include <unistd.h>

#include "util.h"

extern char ubuf[];

const char *
bfmt_human(char *buf, uintmax_t num, int base)
{
	double scaled;
	size_t i, prefixlen;
	const char **prefix;
	const char *prefix_1000[] = { "", "k", "M", "G", "T", "P", "E", "Z",
	                              "Y" };
	const char *prefix_1024[] = { "", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei",
	                              "Zi", "Yi" };

	switch (base) {
	case 1000:
		prefix = prefix_1000;
		prefixlen = sizeof(prefix_1000) / sizeof(prefix_1000[0]);
		break;
	case 1024:
		prefix = prefix_1024;
		prefixlen = sizeof(prefix_1024) / sizeof(prefix_1024[0]);
		break;
	default:
		return NULL;
	}

	scaled = num;
	for (i = 0; i < prefixlen && scaled >= base; ++i)
		scaled /= base;

	snprintf(buf, 10, "%.0f%s", scaled, prefix[i]);
	return buf;
}

const char *
fmt_human(uintmax_t num, int base)
{
	double scaled;
	size_t i, prefixlen;
	const char **prefix;
	const char *prefix_1000[] = { "", "k", "M", "G", "T", "P", "E", "Z",
	                              "Y" };
	const char *prefix_1024[] = { "", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei",
	                              "Zi", "Yi" };

	switch (base) {
	case 1000:
		prefix = prefix_1000;
		prefixlen = sizeof(prefix_1000) / sizeof(prefix_1000[0]);
		break;
	case 1024:
		prefix = prefix_1024;
		prefixlen = sizeof(prefix_1024) / sizeof(prefix_1024[0]);
		break;
	default:
		return NULL;
	}

	scaled = num;
	for (i = 0; i < prefixlen && scaled >= base; ++i)
		scaled /= base;

	snprintf(ubuf, 5, "%.0f%s", scaled, prefix[i]);
	return ubuf;
}

