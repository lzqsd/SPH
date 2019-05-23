#ifndef PARTICLE_HEADER
#define PARTICLE_HEADER

#include "basicStruct.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>

class particle{
public:
    float3 pos;
    float3 vel;
    float mass;
    float pressure;
    float density;

    int gridId;

    int computeGridId(float size, 
            float length, float width, float height, 
            float minX, float minY, float minZ );

private:
};

#endif
