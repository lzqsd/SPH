#ifndef SOLVER_HEADER
#define SOLVER_HEADER 

#include "basicStruct.h"
#include "eulerIntegrator.h"
#include "hashGrid.h"
#include "particle.h"
#include "loader.h"

#include <vector>
#include <cmath>

class solver{
public:
    solver( float S, const std::vector<objLoader::shape_t>& shs,  
            const std::vector<float3>& box, 
            int gridNum = 10000, int fps = 30, 
            float3 G = float3(0.0f, 9.8f, 0.0f), 
            float d = 1e3f, float stiff = 1119e3f, float v=1 );
    // Init grid 
    void init(std::vector<particle>& particleArr );

    // Update the velocity and location 
    // Return true when finish simulating a new frame 
    // Return false otherwise
    virtual bool update(std::vector<particle>& particleArr ) = 0;

    // Get the parameters of solver 
    float getFps() {return framePerSec; }
    float getTimeStep() {return timeStep; }
    float getTimeElapse() {return timeElapse; }
    int getFrameCount() {return frameCount; }

    float getDensityRest() {return densityRest; }
    float getStiffness() {return stiffness; }
    float getViscosity() {return viscosity; }
    float3 getGravityAcce() { return gravityAcce; }

    const hashGrid& getGrid() { return grid; }

protected:
    // Steps to compute velocity
    void computeDensity(int pId, std::unordered_set<int>& neighborArr, 
            std::vector<particle>& particleArr );
    void computePressure(int pId, std::vector<particle>& particleArr );
    float3 computePressureForce(int pId, std::unordered_set<int>& neighborArr, 
            std::vector<particle>& particleArr );
    float3 computeViscosityForce(int pId, std::unordered_set<int>& neighborArr, 
            std::vector<particle>& particleArr );
    float3 computeGravityForce(int pId, std::vector<particle>& particleArr ); 

    // check if a point is in the box
    bool checkValid(const float3& pos );

    // Reflect the velocity and location if hit a boundary 
    bool reflectAtBoundary(const float3& pos1, float3& pos2, float3& velocity);

    // Set time step according to CFL rule 
    // Return true if we render a new frame 
    // Return false otherwise 
    bool CFLRule(std::vector<particle> pArr );

    // Kernel function 
    float kernel(float3 pos1, float3 pos2);

    // Derivative of kernel 
    float3 kernelGrad(float3 pos1, float3 pos2);
    
    float size;

    int framePerSec; 
    float timeStep;
    float timeElapse;
    int frameCount; 

    float densityRest;
    float stiffness;
    float viscosity; 
    float3 gravityAcce;
    hashGrid grid;
    eulerIntegrator integrator;
    
    std::vector<objLoader::shape_t> shapes;
    std::vector<float3> volumeBox;
};

#endif
