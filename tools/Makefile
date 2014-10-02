CC         = gcc
CC_FLAGS   = -pedantic -I /usr/include/ImageMagick/ -w
CC_DEBUG   = -g #-W -Wall -fno-inline
CC_OPT     = -O2 -ffast-math
CC_LVL     = `Magick-config --ldflags --libs`

SLV        = slvextract
SLV_OBJS   = slvextract.o
LVL        = lvl2magick
LVL_OBJS   = lvl2magick.o
OLD        = lvlextract

default: $(SLV) $(LVL)

all: $(SLV) $(LVL) $(OLD)

$(OLD):
	$(MAKE) -C old/

$(SLV): $(SLV_OBJS)
	$(CC) $(SLV_OBJS) -o $(SLV)

$(LVL): $(LVL_OBJS)
	$(CC) $(LVL_OBJS) -o $(LVL) $(CC_LVL) 

%.o: %.c
	$(CC) $(CC_FLAGS) $(CC_DEBUG) $(CC_OPT) -c $< -o $@

clean:
	rm -f $(SLV)
	rm -f $(LVL)
	rm -f $(OLD)
	rm -f *.o *.a
	$(MAKE) -C old/ clean

distclean: clean
	rm -f *~
	$(MAKE) -C old/ distclean
