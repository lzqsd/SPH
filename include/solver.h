#ifndef SOLVER_HEADER
#define SOLVER_HEADER 

#include "basicStruct.h"
#include "eulerIntegrator.h"
#include "uniformGrid.h"
#include "particle.h"

#include <vector>

class solver{
public:
    solver( float L, float W, float H, float S, 
            float mX, float mY, float maZ, 
            int fps = 30, 
            float3 G = float3(0.0f, 9.8f, 0.0f), 
            float d = 1e3f, float stiff = 1119e3f, float v=1);
    // Init grid 
    void init(std::vector<particle>& particleArr );

    // Update the velocity and location 
    // Return true when finish simulating a new frame 
    // Return false otherwise
    bool update(std::vector<particle>& particleArr );

    // Get the parameters of solver 
    float getFps() {return framePerSec; }
    float getTimeStep() {return timeStep; }
    float getTimeElapse() {return timeElapse; }
    int getFrameCount() {return frameCount; }

    float getDensityRest() {return densityRest; }
    float getStiffness() {return stiffness; }
    float getViscosity() {return viscosity; }
    float3 getGravityAcce() { return gravityAcce; }

private:
    // Steps to compute velocity
    void computeDensity(int pId, std::vector<int>& neighborArr, 
            std::vector<particle>& particleArr );
    void computePressure(int pId, std::vector<particle>& particleArr );
    float3 computePressureForce(int pId, std::vector<int>& neighborArr, 
            std::vector<particle>& particleArr );
    float3 computeViscosityForce(int pId, std::vector<int>& neighborArr, 
            std::vector<particle>& particleArr );
    float3 computeGravityForce(int pId, std::vector<particle>& particleArr ); 

    // Set time step according to CFL rule 
    // Return true if we render a new frame 
    // Return false otherwise 
    bool CFLRule(std::vector<particle> pArr );

    // Kernel function 
    float kernel(float3 pos1, float3 pos2);

    // Derivative of kernel 
    float3 kernelGrad(float3 pos1, float3 pos2);

    int framePerSec; 
    float timeStep;
    float timeElapse;
    int frameCount; 

    float densityRest;
    float stiffness;
    float viscosity; 
    float3 gravityAcce;
    uniformGrid grid;
    float length, width, height, size;
    float minX, minY, minZ; 
    eulerIntegrator integrator;
};

#endif
