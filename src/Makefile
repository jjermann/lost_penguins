CXX = g++
CXX_FLAGS = -W -Wall -ansi -pedantic
CXX_DEBUG = -g #-fno-inline
CXX_OPT = -O2 -march=pentium4 -ffast-math
CXX_GAME = -DSDL_MIXER -DSDL_IMAGE -DALPHA
SDL = `sdl-config --cflags`
SDL_LINK = `sdl-config --libs` -lSDL_mixer -lSDL_image

OBJS = common.o anim.o gfxeng.o input.o font.o lost_vikings.o map.o\
       objects_common.o objects.o sfxeng.o vikings_common.o\
       monsters_common.o characters_common.o characters.o objectpools.o\
       weapons.o events.o imgcache.o
BIN = ../lost_vikings


$(BIN): $(OBJS)
		$(CXX) $(OBJS) $(SDL_LINK) -o $(BIN)

%.o: %.cpp
		$(CXX) $(CXX_FLAGS) $(CXX_DEBUG) $(CXX_GAME) $(SDL) -c $< -o $@

clean:
		rm -f *.o
		rm -f $(BIN)

distclean: clean
		rm -f *~