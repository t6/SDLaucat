# On Windows this is a Cygwin and console app.  Don't use SDL_main and
# don't mark it as a GUI app.  This makes sure this runs in either a Cygwin
# terminal or on the regular Windows command prompt.
SDL2_CFLAGS =	`sdl2-config --cflags | sed -e 's/-Dmain=SDL_main//g'`
SDL2_LIBS =	`sdl2-config --libs | sed -e 's/-mwindows//g' -e 's/-lSDL2main//g'`

CFLAGS +=	-Wall -Isndio/bsd-compat -Isndio/libsndio ${SDL2_CFLAGS}
CFLAGS +=	-DDEBUG -DHAVE_ARC4RANDOM
LDLIBS +=	${SDL2_LIBS}

OBJS= 	sndio/libsndio/sio_aucat.o \
	sndio/libsndio/sio.o \
	sndio/libsndio/aucat.o \
	sndio/libsndio/debug.o \
	sndio/bsd-compat/issetugid.o \
	sndio/bsd-compat/strlcpy.o \
	sndio/bsd-compat/strtonum.o \
	SDLaucat.o

SDLaucat: ${OBJS}
	${CC} -o SDLaucat ${LDFLAGS} ${OBJS} ${LDLIBS}

clean:
	rm -f SDLaucat SDLaucat.o SDLaucat.exe \
		sndio/bsd-compat/*.o sndio/libsndio/*.o
