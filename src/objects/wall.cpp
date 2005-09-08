#include "common.h"
#include "objects_common.h"
#include "wall.h"


Wall::Wall(Sint16 xcord, Sint16 ycord, ParameterMap& parameters):
  Object(xcord,ycord,parameters) {
    otype=OTYPE_DENSE;
}
Wall::~Wall() { }
