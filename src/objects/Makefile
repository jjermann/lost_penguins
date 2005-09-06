CXX = g++
CXX_FLAGS = -W -Wall -ansi -pedantic
CXX_DEBUG = -g #-fno-inline
CXX_OPT = #-O2 -march=pentium4 -ffast-math
CXX_GAME = -DSDL_MIXER -DSDL_IMAGE -DALPHA -I ../
SDL = `sdl-config --cflags`
SDL_LINK = `sdl-config --libs` -lSDL_mixer -lSDL_image #-ldl -rdynamic
AR = ar

OBJS  = baleog.o bomb.o door.o erik.o exit.o fang.o geyser.o heart.o\
        key.o olaf.o scorch.o teleport.o trigger.o triggered_bomb.o\
        wall.o water.o wind.o zombie.o plant.o spike.o
PLUGS = ../objects.a


default:
	+$(MAKE) depend
	+$(MAKE) $(OBJS)
	+$(MAKE) $(PLUGS)

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_DEBUG) $(CXX_GAME) $(CXX_OPT) $(SDL) -c $< -o $@

$(PLUGS): $(OBJS)
	$(AR) r $(PLUGS) $(OBJS)

clean:
	rm -f *.o

distclean: clean
	rm -f *~
	rm -f Makefile.dep
	rm -f Makefile.dep.bak

depend:
	touch Makefile.dep
	makedepend *.cpp -f Makefile.dep 2>/dev/null
-include Makefile.dep