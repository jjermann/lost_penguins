CC         = gcc
CC_FLAGS   = -pedantic -I /usr/include/ImageMagick/ -w
CC_DEBUG   = -g # -W -Wall -fno-inline
CC_OPT     = -O2 -ffast-math
CC_TOOL    = -lpng

OBJS        = png_out.o xpm_out.o pgm_out.o png_xpm_out.o lvlextract.o
BIN         = ../lvlextract

default: $(BIN)

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $(BIN) $(CC_TOOL)

%.o: %.c
	$(CC) $(CC_FLAGS) $(CC_DEBUG) -c $< -o $@

clean:
	rm -f $(BIN)
	rm -f *.o *.a

distclean: clean
	rm -f *~
