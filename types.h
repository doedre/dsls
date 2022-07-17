
#ifndef __TYPES_H__
#define __TYPES_H__

struct functor
{
	const char *(*func)();
	const char *fmt;
	void *arg;
};

#endif	// __TYPES_H__
