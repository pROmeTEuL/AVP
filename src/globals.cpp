#include "globals.h"

Globals &Globals::instance()
{
    static Globals glb;
    return glb;
}

Globals::Globals() {}
