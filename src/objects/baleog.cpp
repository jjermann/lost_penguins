#include "common.h"
#include "events.h"
#include "anim.h"
#include "imgcache.h"
#include "sndcache.h"
#include "baleog.h"


Baleog::Baleog(string imagename, Uint16 xcord, Uint16 ycord, string vname):
  Viking(imagename,xcord,ycord,vname) {
    im_left=new Animation(imgcache->loadImage("baleog_right.bmp"));
    im_right=im_left;
    im_fall_left=im_left;
    im_fall_right=im_left;
    im_krit_left=im_left;
    im_krit_right=im_left;
    im_land_left=new Animation(imgcache->loadImage("olaf_land.bmp"),1,T_IRR,true);
    im_land_right=im_land_left;
    au_hit=NULL;
}

Baleog::~Baleog() {
    delete im_left;
    delete im_land_left;
}
