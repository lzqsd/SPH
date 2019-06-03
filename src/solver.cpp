#include "solver.h"

solver::solver( float L, float W, float H, float S, 
        float mX, float mY, float mZ, 
        int fps,
        float3 G, float d, float stiff, float v): 
    grid(uniformGrid(L, W, H, S) ), integrator(eulerIntegrator( 1.0f / float(fps) ) )
{
    densityRest = d; 
    stiffness = stiff;
    viscosity = v;
    gravityAcce = G;
    length = L;
    width = W; 
    height = H;
    size = S;

    // min X, Y, Z
    minX = mX;
    minY = mY;
    minZ = mZ;

    framePerSec = fps;
    timeStep = 1.0f / framePerSec;
    timeElapse = 0.0f;
    frameCount = 0.0f;
}
    

// Init grid 
void solver::init(std::vector<particle>& particleArr )
{
    for(unsigned pId = 0; pId < particleArr.size(); pId++){
        int gridId = particleArr[pId].computeGridId(size, length, width, height, 
                minX, minY, minZ );
        grid.updateParticle(pId, gridId );
    }
}

// Set time step according to CFL rule 
bool solver::CFLRule(std::vector<particle > pArr )
{   
    float vMin = 0;
    for(unsigned i = 0; i < pArr.size(); i++){
        float3 v = pArr[i].vel;
        if(v.norm2() > vMin ){
            vMin = v.norm2();
        }
    }
    float h = size / 2.0;
    float t = 0.4 * h / std::max(vMin, 1e-10f);
    if(t + timeElapse > float(frameCount +1.0f) / float(framePerSec) ){
        timeStep = float(frameCount + 1.0f) / float(framePerSec) - timeElapse;
        timeElapse = float(frameCount + 1.0f) / float(framePerSec );
        frameCount += + 1;
        return true;
    }
    else{
        timeElapse += t;
        timeStep = t;
        return false;
    }
    return true;
}

// Update the velocity 
bool solver::update(std::vector<particle>& particleArr )
{
    float timeStart = getTimeElapse();
    std::vector<int>* neighborArrs = new std::vector<int>[particleArr.size()];

    // Find neighbors, compute densities and pressures
    for(unsigned pId = 0; pId < particleArr.size(); pId++){
        // Find neighbors 
        int gridId = particleArr[pId].gridId;
        std::vector<int> neighborArr = grid.query(pId, gridId, particleArr );
        
        // Compute density 
        computeDensity(pId, neighborArr, particleArr);

        // Compute pressure 
        computePressure(pId, particleArr );
        neighborArrs[pId] = neighborArr;
    }

    bool isNewFrame = CFLRule(particleArr );
    integrator.setTimeStep(timeStep );


    // Compute forces, velocities and positinons (Currently we do not handle boundary issues)
    float3* newVelocities = new float3[particleArr.size()]; 
    float3* newPositions = new float3[particleArr.size()];
    for(unsigned pId = 0; pId < particleArr.size(); pId++){
        std::vector<int> neighborArr = neighborArrs[pId];

        // Compute forces 
        float3 pForce = computePressureForce(pId, neighborArr, particleArr );
        float3 vForce = computeViscosityForce(pId, neighborArr, particleArr );
        float3 gForce = computeGravityForce(pId, particleArr );

        float3 force = pForce + vForce + gForce;

        // Compute new velocities and positinons 
        float3 acce = force / particleArr[pId].mass;
        
        float3 vel = particleArr[pId].vel;
        float3 pos = particleArr[pId].pos;
        integrator.update(vel, pos, acce);

        newVelocities[pId] = vel;
        newPositions[pId] = pos;
    }

    // Update particle and grid 
    for(unsigned pId = 0; pId < particleArr.size(); pId++){
        float3 pos = newPositions[pId];
        float3 vel = newVelocities[pId];
        
        particleArr[pId].pos = pos;
        particleArr[pId].vel = vel;
        
        // Update grid 
        int preGridId = particleArr[pId].gridId;
        int curGridId = particleArr[pId].computeGridId(
                size, length, width, height, minX, minY, minZ );
        if(preGridId != curGridId){
            grid.updateParticle(pId, curGridId, preGridId);
        }
    }

    float timeEnd = getTimeElapse();
    printf("From %.4f to %.4f", timeStart, timeEnd );

    delete [] neighborArrs;
    delete [] newVelocities;
    delete [] newPositions;

    return isNewFrame;
}


// Steps to compute velocity
void solver::computeDensity(int pId, std::vector<int>& neighborArr, 
        std::vector<particle>& particleArr )
{
    float3 pos1 = particleArr[pId].pos; 
    float mass1 = particleArr[pId].mass;
    float density = kernel(pos1, pos1 ) * mass1;

    for(const auto& nId : neighborArr){
        float3 pos2 = particleArr[nId].pos;
        float mass2 = particleArr[nId].mass;
        density += kernel(pos1, pos2) * mass2;
    }
    particleArr[pId].density = density;
}

void solver::computePressure(int pId, std::vector<particle>& particleArr )
{
    float density = particleArr[pId].density;
    float pressure = stiffness * ( pow(density / densityRest, 7.0f) - 1);
    particleArr[pId ].pressure = pressure;
}

float3 solver::computePressureForce(int pId, std::vector<int>& neighborArr, 
            std::vector<particle>& particleArr )
{
    float3 force(0, 0, 0); 
    float m1 = particleArr[pId].mass;
    float p1 = particleArr[pId].pressure;
    float d1 = particleArr[pId].density;
    float3 pos1 = particleArr[pId].pos;

    for(const auto& nId : neighborArr){
        float m2 = particleArr[nId].mass;
        float p2 = particleArr[nId].pressure;
        float d2 = particleArr[nId].density;
        float3 pos2 = particleArr[nId].pos;

        float3 grad = kernelGrad(pos1, pos2);
        float coef = m2 * ( p1 / d1 / d1 + p2 / d2 / d2);
        force = force + coef * grad;
    }
    force = - m1 * force;
    return force;
}

float3 solver::computeViscosityForce(int pId, std::vector<int>& neighborArr, 
            std::vector<particle>& particleArr )
{
    float3 force(0, 0, 0);
    float h = size / 2.0f;
    float m1 = particleArr[pId].mass;
    float3 pos1 = particleArr[pId].pos;
    float3 vel1 = particleArr[pId].vel;

    for(const auto& nId : neighborArr){
        float m2 = particleArr[nId].mass;
        float d2 = particleArr[nId].density;
        float3 pos2 = particleArr[nId].pos;
        float3 vel2 = particleArr[nId].vel;

        float3 x12 = pos1 - pos2;
        float3 v12 = vel1 - vel2;
        float3 grad = kernelGrad(pos1, pos2);

        float coef = 2 * m2 / d2 * x12.dot(grad ) / (x12.dot(x12) + 0.01*h*h);
        force = force + coef * (v12 );
    }
    force = m1 * viscosity * force; 
    return force;
}

float3 solver::computeGravityForce(int pId, std::vector<particle>& particleArr )
{
    float3 gForce = particleArr[pId].mass * gravityAcce;
    return gForce;
}

// Kernel function 
float solver::kernel(float3 pos1, float3 pos2){
    float dist = (pos1 - pos2).norm2();
    float h = size / 2;
    float q = dist / h;
    
    float f_q = 0;
    if( q >= 0 && q < 1){
        f_q = 2.0f / 3.0f - q * q + 0.5f * q * q * q;
    }
    else if(q >= 1 && q < 2){ 
        f_q = 1.0 / 6.0f * pow(2.0f - q, 3.0f );
    }

    f_q *= 1 / pow(h, 3.0f) * 3.0f / 2.0f / PI; 
    
    return f_q;
}

float3 solver::kernelGrad(float3 pos1, float3 pos2){
    float dist = (pos1 - pos2).norm2();
    float h = size / 2;
    float q = dist / h;

    float3 grad = (pos1 - pos2) / dist; 
    
    float f_q_grad = 0;
    if( q >= 0 && q < 1){
        f_q_grad = -2 * q + 3.0f / 2.0f * pow(q, 2.0f);
    }
    else if(q >= 1 && q < 2){ 
        f_q_grad = -0.5 * pow(2.0f - q, 2.0f);
    }
    f_q_grad = 1 / pow(h, 4.0f) * 3.0f / 2.0f / PI * f_q_grad;
    grad = f_q_grad * grad;
    
    return grad; 
}
