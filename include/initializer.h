#ifndef NITIALIZER_HEADER 
#define INITIALIZER_HEADER

#include "particle.h"
#include "basicStruct.h"

#include <vector>
#include <cmath>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <random>

// Particles will always be initialized in a box region 
// Make sure each grid will have around 8 particles in total 
class initializer{
public:
    initializer( 
            int n, 
            float d = 1e3f );
    
    // Initialize the 
    void initialize(
                std::vector<particle>& particleArr, 
                const std::vector<float3>& waterBox,
                float& size
            );
private:
    void computeGridParameters(
            const std::vector<float3>& waterBox, 
            float& size
            );
    void initializeParticles(
            float size, 
            std::vector<particle>& particleArr, 
            const std::vector<float3>& waterBox );

    float numOfParticles; 
    float densityRest; 
};

#endif
