CC         = gcc
CC_FLAGS   = -pedantic -W -Wall
CC_DEBUG   = -g #-fno-inline
CC_OPT     = -O2 -march=pentium4 -ffast-math
CC_LVL     = `Magick-config --ldflags --libs`

SLV        = lvlextract
SLV_OBJS   = slvextract.o
LVL        = lvl2magick
LVL_OBJS   = lvl2magick.o

default: $(SLV) $(LVL)

all: $(SLV) $(LVL)

$(SLV): $(SLV_OBJS)
	$(CC) $(SLV_OBJS) -o $(SLV)

$(LVL): $(LVL_OBJS)
	$(CC) $(CC_LVL) $(LVL_OBJS) -o $(LVL)

%.o: %.c
	$(CC) $(CC_FLAGS) $(CC_DEBUG) $(CC_OPT) -c $< -o $@

clean:
	rm -f $(SLV)
	rm -f $(LVL)
	rm -f *.o *.a

distclean: clean
	rm -f *~
