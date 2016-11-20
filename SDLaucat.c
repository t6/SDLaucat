/*
 * Copyright (c) 2016 Tobias Kortkamp <t@tobik.me>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/resource.h>
#include <err.h>
#include <errno.h>
#include <SDL.h>
#include <signal.h>
#include <sndio.h>
#include <unistd.h>

#include "bsd-compat.h"

#define NCHAN_MAX 16
#define RATE_MIN 4000
#define RATE_MAX 192000

#ifndef MAX_PATH
#define MAX_PATH 1024
#endif

#ifndef nitems
#define nitems(x) (sizeof((x)) / sizeof((x)[0]))
#endif

/* Something defines main=SDL_main.  Do not want for this program! */
#ifdef main
#undef main
#endif

struct sdl_fmt {
	SDL_AudioFormat fmt;
	unsigned int bits;
	unsigned int bps;
	unsigned int sig;
	unsigned int le;
	unsigned int msb;
	const char *desc;
};

static struct sdl_fmt formats[] = {
	{ AUDIO_S8,	8, 1, 1, 0, 0,	"s8" },
	{ AUDIO_S8,	8, 1, 1, 1, 0,	"s8" },
	{ AUDIO_S8,	8, 1, 1, 0, 1,	"s8" },
	{ AUDIO_S8,	8, 1, 1, 1, 1,	"s8" },
	{ AUDIO_U8,	8, 1, 0, 0, 0,	"u8" },
	{ AUDIO_U8,	8, 1, 0, 1, 0,	"u8" },
	{ AUDIO_U8,	8, 1, 0, 0, 1,	"u8" },
	{ AUDIO_U8,	8, 1, 0, 1, 1,	"u8" },

	{ AUDIO_U16MSB, 16, 2, 0, 0, 1, "u16be" },
	{ AUDIO_U16MSB, 16, 2, 0, 0, 1, "u16be" },
	{ AUDIO_U16LSB, 16, 2, 0, 1, 0, "u16le" },
	{ AUDIO_U16LSB, 16, 2, 0, 1, 0, "u16le" },
	{ AUDIO_S16MSB, 16, 2, 1, 0, 1, "s16be" },
	{ AUDIO_S16MSB, 16, 2, 1, 0, 1, "s16be" },
	{ AUDIO_S16LSB, 16, 2, 1, 1, 0, "s16le" },
	{ AUDIO_S16LSB, 16, 2, 1, 1, 0, "s16le" },

	{ AUDIO_S32MSB, 32, 4, 1, 0, 1, "s32be" },
	{ AUDIO_S32LSB, 32, 4, 1, 1, 0, "s32le" },
};

static SDL_sem *quitsem;

void
sigint(int sig)
{
	SDL_SemPost(quitsem);
}

void
audio_cb(void* userdata, Uint8* stream, int len)
{
	struct sio_hdl *hdl = (struct sio_hdl*)userdata;

	if (sio_write(hdl, stream, len) == 0)
		SDL_SemPost(quitsem);
}

unsigned int
xparsenum(const char *str, int min, int max)
{
	const char *errstr;
	unsigned int v;

	v = strtonum(str, min, max, &errstr);
	if (errstr != NULL)
		errx(1, "%s: %s", errstr, str);
	return v;
}

int
list_capture_devs(void)
{
	int i, count;

	if (SDL_Init(SDL_INIT_AUDIO) != 0)
		errx(1, "SDL_Init: %s", SDL_GetError());

	count = SDL_GetNumAudioDevices(1);
	for (i = 0; i < count; ++i) {
		printf("%d: %s\n", i, SDL_GetAudioDeviceName(i, 1));
	}

	SDL_Quit();
	return 0;
}

int
main(int argc, char *argv[])
{
	char ch;
	unsigned int i, found, daemonize;
	SDL_AudioSpec want, have;
	struct sio_par par;
	struct sio_hdl *hdl;
	const char *sio_devname, *sdl_devname;
	char aucat_cookie_dir[MAX_PATH];
	int sdl_devid;
	SDL_AudioDeviceID rdev;

	/* Default parameters */
	daemonize = 1;
	sio_devname = SIO_DEVANY;
	sdl_devid = 0;
	SDL_zero(want);
	want.channels = 2;
	want.format = AUDIO_S16LSB;
	want.freq = 44100;
	want.samples = 1024;

	while ((ch = getopt(argc, argv, "a:b:c:de:f:g:lr:s:")) != -1) {
		switch (ch) {
		case 'a':
			if (snprintf(aucat_cookie_dir, sizeof(aucat_cookie_dir), "HOME=%s", optarg) < 0)
				err(1, "asprintf");
			if (putenv(aucat_cookie_dir) < 0)
				err(1, "putenv");
			break;
		case 'b':
			want.size = xparsenum(optarg, 1, RATE_MAX);
			break;
		case 'c':
			want.channels = xparsenum(optarg, 1, NCHAN_MAX);
			break;
		case 'd':
			daemonize = 0;
			break;
		case 'e':
			found = 0;
			for (i = 0; i < nitems(formats); i++) {
				if (strcmp(optarg, formats[i].desc) == 0) {
					want.format = formats[i].fmt;
					found = 1;
					break;
				}
			}
			if (!found)
				errx(1, "unknown format: %s", optarg);
			break;
		case 'f':
			sio_devname = strdup(optarg);
			break;
		case 'g':
			sdl_devid = xparsenum(optarg, 0, 256);
			break;
		case 'l':
			return list_capture_devs();
		case 'r':
			want.freq = xparsenum(optarg, RATE_MIN, RATE_MAX);
			break;
		case 's':
			want.samples = xparsenum(optarg, 1, RATE_MAX);
			break;
		case '?':
		default:
			fprintf(stderr,
				"usage: SDLaucat [-b size] [-c chan] [-d] [-e enc] [-f sndiodev] [-g sdldev]\n"
				"\t[-l] [-r rate] [-s samples] [-a aucat_cookie_dir]\n");
			return 1;
		}
	}

	if (daemonize && daemon(0, 0) < 0)
		err(1, "daemon");

	if (daemonize && setpriority(PRIO_PROCESS, 0, -20) < 0)
		err(1, "setpriority");

	quitsem = SDL_CreateSemaphore(0);

	hdl = sio_open(sio_devname, SIO_PLAY, 0);
	if (hdl == NULL)
		errx(1, "sio_open: %s", sio_devname);

	if (SDL_Init(SDL_INIT_AUDIO) != 0)
		errx(1, "SDL_Init: %s", SDL_GetError());

	want.callback = audio_cb;
	want.userdata = hdl;

	sdl_devname = SDL_GetAudioDeviceName(sdl_devid, 1);
	if (sdl_devname == NULL)
		errx(1, "SDL_GetAudioDeviceName: %s", SDL_GetError());
	rdev = SDL_OpenAudioDevice(sdl_devname, 1, &want, &have,
				   SDL_AUDIO_ALLOW_ANY_CHANGE);
	if (rdev == 0)
		errx(1, "SDL_OpenAudioDevice: %s", SDL_GetError());

	sio_initpar(&par);
	found = 0;
	for (i = 0; i < nitems(formats); i++) {
		if (formats[i].fmt == have.format) {
			par.sig = formats[i].sig;
			par.le = formats[i].le;
			par.bits = formats[i].bits;
			par.bps = formats[i].bps;
			par.msb = formats[i].msb;
			found = 1;
			break;
		}
	}
	if (!found)
		errx(1, "unknown format: %d", have.format);
	par.appbufsz = have.size;
	par.round = have.samples;
	par.rchan = have.channels;
	par.rate = have.freq;

	if (!sio_setpar(hdl, &par))
		errx(1, "sio_setpar");
	if (!sio_getpar(hdl, &par))
		errx(1, "sio_getpar");

	for (i = 0; i < nitems(formats); i++) {
		if (formats[i].bits == par.bits &&
		    formats[i].le == par.le &&
		    formats[i].sig == par.sig &&
		    formats[i].msb == par.msb &&
		    formats[i].fmt != have.format)
			errx(1, "parameter mismatch");
	}
	if (par.rchan != have.channels && par.rate != have.freq)
		errx(1, "parameter mismatch");

	if (!sio_start(hdl))
		errx(1, "sio_start");

	SDL_PauseAudioDevice(rdev, 0);

	if (signal(SIGINT, sigint) == SIG_ERR)
		err(1, "signal");
	if (signal(SIGTERM, sigint) == SIG_ERR)
		err(1, "signal");

	SDL_SemWait(quitsem);

	SDL_DestroySemaphore(quitsem);
	SDL_Quit();
	sio_close(hdl);

	return 0;
}
