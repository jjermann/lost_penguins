CXX = g++
CXX_FLAGS = -W -Wall -ansi -pedantic
CXX_DEBUG = -g #-fno-inline
CXX_OPT = -O2 -march=pentium4 -ffast-math
CXX_GAME = -DSDL_MIXER -DSDL_IMAGE -DALPHA
SDL = `sdl-config --cflags`
SDL_LINK = `sdl-config --libs` -lSDL_mixer -lSDL_image

OBJS = slvextract.o
BIN = ../slvextract


$(BIN): $(OBJS)
		$(CXX) $(OBJS) $(SDL_LINK) -o $(BIN)

%.o: %.cpp
		$(CXX) $(CXX_FLAGS) $(CXX_DEBUG) $(CXX_GAME) $(SDL) -c $< -o $@

clean:
		rm -f *.o

distclean: clean
		rm -f *~
