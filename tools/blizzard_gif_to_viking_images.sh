#!/bin/sh

convert $1.gif $1_right.png
convert -flop $1.gif $1_left.png

montage\
 -crop 34x34+6+6\
 -geometry 80x80+0+0\
 -tile 100x1\
 -background transparent\
 $1_right.png.* $1_right.png

montage\
 -crop 34x34+6+6\
 -geometry 80x80+0+0\
 -tile 100x1\
 -background transparent\
 $1_left.png.* $1_left.png

# -background \#ff00ff\
# -geometry +0+0
# -colors 32\
# -colorspace transparent\

echo -n "$1 : "
ls $1_right.png.* | wc -l
rm $1_*.png.*
