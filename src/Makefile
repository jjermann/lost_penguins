CXX = g++
CXX_FLAGS = -W -Wall -ansi -pedantic
CXX_DEBUG = -g #-fno-inline
CXX_OPT = -O2 -march=pentium4 -ffast-math
CXX_GAME = -DSDL_MIXER -DSDL_IMAGE -DALPHA -I ./
SDL = `sdl-config --cflags`
SDL_LINK = `sdl-config --libs` -lSDL_mixer -lSDL_image -ldl -rdynamic

OBJS  = common.o anim.o gfxeng.o input.o font.o lost_penguins.o map.o\
        objects_common.o sfxeng.o players_common.o monsters_common.o\
        characters_common.o objectpools.o weapons.o events.o imgcache.o\
        sndcache.o
PLUGS = objects.a
BIN   = ../lost_penguins


$(BIN): $(OBJS) $(PLUGS)
	$(CXX) $(OBJS) $(PLUGS) $(SDL_LINK) -o $(BIN)

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_DEBUG) $(CXX_GAME) $(SDL) -c $< -o $@

$(PLUGS):
	+$(MAKE) -C objects

clean:
	rm -f *.o *.a
	+$(MAKE) -C objects clean

distclean: clean
	rm -f *~
	+$(MAKE) -C objects distclean
