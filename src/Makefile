CXX = g++
CXX_FLAGS = -ansi -pedantic
CXX_DEBUG = -g -W -Wall
#-fno-inline
CXX_OPT = #-O2 -march=pentium4 -ffast-math
CXX_GAME = -DSDL_MIXER -DSDL_IMAGE -DALPHA -I ./
SDL = `sdl-config --cflags`
SDL_LINK = `sdl-config --libs` -lSDL_mixer -lSDL_image #-ldl -rdynamic

OBJS  = common.o gfxeng.o input.o font.o lost_penguins.o scenario.o\
        objects_common.o sfxeng.o players_common.o monsters_common.o\
        characters_common.o objectpools.o weapons.o events.o imgcache.o\
        sndcache.o physics.o animation.o menu.o editor.o SDL_rotozoom.o
PLUGS = objects.a
BIN   = ../lost_penguins


default:
	+$(MAKE) depend
	+$(MAKE) -C objects
	+$(MAKE) $(BIN)

$(BIN): $(OBJS) $(PLUGS)
	$(CXX) $(OBJS) $(PLUGS) $(SDL_LINK) -o $(BIN)

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_DEBUG) $(CXX_GAME) $(CXX_OPT) $(SDL) -c $< -o $@

$(PLUGS):
	+$(MAKE) -C objects

clean:
	rm -f *.o *.a
	+$(MAKE) -C objects clean

distclean: clean
	rm -f *~
	rm -f Makefile.dep
	rm -f Makefile.dep.bak
	+$(MAKE) -C objects distclean

depend:
	touch Makefile.dep
	makedepend *.cpp -f Makefile.dep 2>/dev/null
-include Makefile.dep
