#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <X11/Xlib.h>

#include "config.h"

char buf[MAX_ELEM_LEN];

void write_args(char *sl)
{
	char fmt[STATUS_LEN];
	const char *elem;
	const size_t args_len = sizeof(args) / sizeof(struct functor);

	for (size_t i = 0; i < args_len; ++i) {
		if (strlen(sl) + MAX_ELEM_LEN >= STATUS_LEN)
			break;

		if (!(elem = args[i].func(args[i].arg)))
			elem = def_val;

		snprintf(fmt, STATUS_LEN, args[i].fmt, elem);

		strcat(sl, fmt);
	}
}

int
main(void)
{
	char sl[STATUS_LEN];
	Display *dpy;

	if (!(dpy = XOpenDisplay(NULL)))
		return -1;

	while (1)
	{
		sl[0] = '\0';
		write_args(sl);
		if (XStoreName(dpy, DefaultRootWindow(dpy), sl) < 0)
			return -1;
		XFlush(dpy);
		usleep(wait);
	}

	return 0;
}
