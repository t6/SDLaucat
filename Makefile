# On Windows this is a Cygwin and console app.  Don't use SDL_main and
# don't mark it as a GUI app.  This makes sure this runs in either a Cygwin
# terminal or on the regular Windows command prompt.
SDL2_CFLAGS =	`sdl2-config --cflags | sed -e 's/-Dmain=SDL_main//g'`
SDL2_LIBS =	`sdl2-config --libs | sed -e 's/-mwindows//g' -e 's/-lSDL2main//g'`

CFLAGS +=	-Wall -I/usr/local/include ${SDL2_CFLAGS}
LDFLAGS +=	-L/usr/local/lib
LDLIBS +=	-lsndio ${SDL2_LIBS}

all: SDLcat

clean:
	rm -f SDLcat SDLcat.exe
