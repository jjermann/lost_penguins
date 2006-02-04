#include "common.h"
#include "objects_common.h"
#include "wall.h"


Wall::Wall(Sint16 xcord, Sint16 ycord, const ParameterMap& param):
  Object(xcord,ycord,param) {
    otype=OTYPE_DENSE;
}
Wall::~Wall() { }
