CXX = gcc
CXX_FLAGS = -W -Wall -ansi -pedantic
CXX_DEBUG = -g #-fno-inline
CXX_OPT = -O2 -march=pentium4 -ffast-math
CXX_TOOL = -lpng

OBJS  = png_out.o xpm_out.o pgm_out.o png_xpm_out.o lvlextract.o
BIN   = ../lvlextract

default: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(CXX_TOOL) $(OBJS) -o $(BIN)

%.o: %.c
	$(CXX) $(CXX_FLAGS) $(CXX_DEBUG) -c $< -o $@

clean:
	rm -f $(BIN)
	rm -f *.o *.a

distclean: clean
	rm -f *~
