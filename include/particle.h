#ifndef PARTICLE_HEADER
#define PARTICLE_HEADER

#include "basicStruct.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>

class particle{
public:
    particle();

    float3 pos;
    float3 vel;
    float mass;
    float pressure;
    float density;

    int gridId;
    bool isValid;
private:
};

#endif
