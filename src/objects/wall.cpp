#include "common.h"
#include "objects_common.h"
#include "wall.h"


Wall::Wall(string imagename, Sint16 xcord, Sint16 ycord, string oname):
  Object(imagename,xcord,ycord,oname) {
    otype=OTYPE_DENSE;
}
Wall::~Wall() { }
