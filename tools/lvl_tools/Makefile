CC         = gcc
CC_FLAGS   = -ansi -pedantic -W -Wall
CC_DEBUG   = -g #-fno-inline
CC_OPT     = -O2 -march=pentium4 -ffast-math
CC_TOOL    = -lpng
CC_MAGICK  = `Magick-config --ldflags --libs`

OBJS        = png_out.o xpm_out.o pgm_out.o png_xpm_out.o lvlextract.o
OBJS_MAGICK = lvl2magick.o
BIN         = ../lvlextract
BIN_MAGICK  = ../lvl2magick

default: $(BIN_MAGICK)

all: $(BIN) $(BIN_MAGICK)

$(BIN): $(OBJS)
	$(CC) $(CC_TOOL) $(OBJS) -o $(BIN)

$(BIN_MAGICK): $(OBJS_MAGICK)
	$(CC) $(CC_MAGICK) $(OBJS_MAGICK) -o $(BIN_MAGICK)

%.o: %.c
	$(CC) $(CC_FLAGS) $(CC_DEBUG) -c $< -o $@

clean:
	rm -f $(BIN)
	rm -f *.o *.a

distclean: clean
	rm -f *~
