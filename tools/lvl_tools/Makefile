CC         = gcc
CC_FLAGS   = -pedantic -W -Wall
CC_DEBUG   = -g #-fno-inline
CC_OPT     = -O2 -march=pentium4 -ffast-math
CC_TOOL    = -lpng

OBJS        = png_out.o xpm_out.o pgm_out.o png_xpm_out.o lvlextract.o
BIN         = ../lvlextract

default: $(BIN)

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CC_TOOL) $(OBJS) -o $(BIN)

%.o: %.c
	$(CC) $(CC_FLAGS) $(CC_DEBUG) -c $< -o $@

clean:
	rm -f $(BIN)
	rm -f *.o *.a

distclean: clean
	rm -f *~
