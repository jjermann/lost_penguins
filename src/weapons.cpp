#include "common.h"
#include "weapons.h"

using namespace std;

Weapon::Weapon(Sint16 dmg, Uint16 weaptype, Uint16 subtype) : wtype(weaptype), stype(subtype), damage(dmg) { }
Weapon::~Weapon() { }

