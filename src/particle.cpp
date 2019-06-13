#include "particle.h"

particle::particle(){
    pos.make_float3(0.0f);
    vel.make_float3(0.0f);
    mass = 0.0f;
    pressure = 0.0f;
    density = 0.0f;

    gridId = -1;
    isValid = true;
}
