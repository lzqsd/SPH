#include "eulerIntegrator.h"
#include "basicStruct.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

eulerIntegrator::eulerIntegrator(float T){
    if(T <= 0){
        std::cout<<"Error: time step for euler integrator must be "
            <<"larger than 0"<<std::endl;
        exit(1);
    }
    timeStep = T;
}

// Set new time step and return the previous timeStep 
float eulerIntegrator::setTimeStep(float T){
    float T_ = timeStep;
    timeStep = T;
    return T_;
}

// Return current time step 
float eulerIntegrator::getTimeStep(){
    return timeStep;
}

// Apply semi implicit euler rule to compute next velocity and position 
void eulerIntegrator::update(float3& v, float3& x, float3 acce){
    v = v + timeStep * acce;
    x = x + timeStep * v;
    
}

