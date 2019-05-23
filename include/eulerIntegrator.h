#ifndef EULERINTEGRATOR_HEADER
#define EULERINTEGRATOR_HEADER 

#include "basicStruct.h"

class eulerIntegrator{
public:
    eulerIntegrator(float T = 0.01);

    // Set new time step and return the previous timeStep 
    float setTimeStep(float T);

    // Return current time step 
    float getTimeStep();

    // Apply semi implicit euler rule to compute next velocity and position 
    void update(float3& v, float3& x, float3 acce);
private:
    // Time step 
    float timeStep;
};

#endif
