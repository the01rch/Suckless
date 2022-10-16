/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>

#include "arg.h"
#include "slstatus.h"
#include "util.h"

struct arg {
	const char *(*func)();
	const char *fmt;
	const char *args;
};

/*
static const char *color[2][3] = {
    [Scheme1] = { "#d65d0e", "#ebdbb2", "#ffffff" },
    [Scheme2] = { "#ebdbb2", "#504945", "#ffffff" },
    [Scheme3] = { "#504945", "#282828", "#ffffff" },
}

enum { Scheme1, Scheme2, Scheme3};

enum { ColFg, ColBg, ColBorder };
*/

char buf[1024];
static volatile sig_atomic_t done;
static Display *dpy;

#include "config.h"

static void
terminate(const int signo)
{
	if (signo != SIGUSR1)
		done = 1;
}

static void
difftimespec(struct timespec *res, struct timespec *a, struct timespec *b)
{
	res->tv_sec = a->tv_sec - b->tv_sec - (a->tv_nsec < b->tv_nsec);
	res->tv_nsec = a->tv_nsec - b->tv_nsec +
	               (a->tv_nsec < b->tv_nsec) * 1E9;
}

static void
usage(void)
{
	die("usage: %s [-s] [-1]", argv0);
}

/*
unsigned long _RGB(int r, int g, int b)
{
    return b + (g<<8) + (r<<16);
}

void
drw_arrow(Display *dpy, int x, int y, unsigned int w, unsigned int h, int direction)
{
        if (!dpy)
                return;

        int screen = DefaultScreen(dpy);
       // char cl[7];
        Window root = RootWindow(dpy, screen);
        GC gc = XCreateGC(dpy, root, 0, NULL);
        Drawable draw = XCreatePixmap(dpy, root, w, h, DefaultDepth(dpy, screen));
        
        x = direction ? x : x + w;
        w = direction ? w : -w;
        
        XPoint points[] = {
                {x    , y      },
                {x + w, y      },
                {x    , y + h  },
        };

        //XSetForeground(dpy, gc, 2);
        //XFillRectangle(dpy, draw, gc, x, y, w, h);
        //XSetForeground(dpy, gc, 4);
        //XFillPolygon(dpy, draw, gc, points, 3, Nonconvex, CoordModeOrigin);
}
*/
int
main(int argc, char *argv[])
{
	struct sigaction act;
	struct timespec start, current, diff, intspec, wait;
	size_t i, len;
	int sflag, ret;
	char status[MAXLEN];
	const char *res;

	sflag = 0;
	ARGBEGIN {
		case '1':
			done = 1;
			/* fallthrough */
		case 's':
			sflag = 1;
			break;
		default:
			usage();
	} ARGEND

	if (argc) {
		usage();
	}

	memset(&act, 0, sizeof(act));
	act.sa_handler = terminate;
	sigaction(SIGINT,  &act, NULL);
	sigaction(SIGTERM, &act, NULL);
	act.sa_flags |= SA_RESTART;
	sigaction(SIGUSR1, &act, NULL);

	if (!sflag && !(dpy = XOpenDisplay(NULL))) {
		die("XOpenDisplay: Failed to open display");
	}

	do {
		if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) {
			die("clock_gettime:");
		}

		status[0] = '\0';
		for (i = len = 0; i < LEN(args); i++) {
			if (!(res = args[i].func(args[i].args))) {
				res = unknown_str;
			}
            //drw_arrow(dpy, 3, 0, 1, 0, 0);
			if ((ret = esnprintf(status + len, sizeof(status) - len,
			                    args[i].fmt, res)) < 0) {
				break;
			}
			len += ret;
		}

		if (sflag) {
			puts(status);
			fflush(stdout);
			if (ferror(stdout))
				die("puts:");
		} else {
			if (XStoreName(dpy, DefaultRootWindow(dpy), status)
                            < 0) {
				die("XStoreName: Allocation failed");
			}
			XFlush(dpy);
		}

		if (!done) {
			if (clock_gettime(CLOCK_MONOTONIC, &current) < 0) {
				die("clock_gettime:");
			}
			difftimespec(&diff, &current, &start);

			intspec.tv_sec = interval / 1000;
			intspec.tv_nsec = (interval % 1000) * 1E6;
			difftimespec(&wait, &intspec, &diff);

			if (wait.tv_sec >= 0) {
				if (nanosleep(&wait, NULL) < 0 &&
				    errno != EINTR) {
					die("nanosleep:");
				}
			}
		}
	} while (!done);

	if (!sflag) {
		XStoreName(dpy, DefaultRootWindow(dpy), NULL);
		if (XCloseDisplay(dpy) < 0) {
			die("XCloseDisplay: Failed to close display");
		}
	}

	return 0;
}
