#include "eosSolver.h"

eosSolver::eosSolver( float S, std::vector<objLoader::shape_t> shs, 
        int gridNum, int fps, float3 G, 
        float d, float stiff, float v)
    :solver(S, shs, gridNum, fps, G, d, stiff, v)
{
}

bool eosSolver::update(std::vector<particle>& particleArr )
{
    float timeStart = getTimeElapse();
    std::unordered_set<int>* neighborArrs = new std::unordered_set<int>[particleArr.size()];

    // Find neighbors, compute densities and pressures
    for(unsigned pId = 0; pId < particleArr.size(); pId++){
        // Find neighbors 
        int gridId = particleArr[pId].gridId;
        std::unordered_set<int> neighborArr = grid.query(pId, gridId, particleArr );
        
        // Compute density 
        computeDensity(pId, neighborArr, particleArr);

        neighborArrs[pId] = neighborArr;
    }

    bool isNewFrame = CFLRule(particleArr );
    integrator.setTimeStep(timeStep );

    // Compute forces, velocities without pressure
    float3* newVelocities = new float3[particleArr.size()]; 
    for(unsigned pId = 0; pId < particleArr.size(); pId++){
        std::unordered_set<int> neighborArr = neighborArrs[pId];

        // Compute forces 
        float3 vForce = computeViscosityForce(pId, neighborArr, particleArr );
        float3 gForce = computeGravityForce(pId, particleArr );

        float3 force = vForce + gForce;

        // Compute new velocities and positinons 
        float3 acce = force / particleArr[pId].mass;
        
        float3 vel = particleArr[pId].vel;
        float3 pos = particleArr[pId].pos;
        integrator.update(vel, pos, acce);

        newVelocities[pId] = vel;
    }

    // Compute divergence free pressure 
    float3* newPositions = new float3[particleArr.size()];
    for(unsigned pId = 0; pId < particleArr.size(); pId++){
        std::unordered_set<int> neighborArr = neighborArrs[pId];
        
        // Compute density 
        computeDensityCorrection(pId, neighborArr, particleArr, newVelocities); 
        
        // Compute pressure 
        computePressure(pId, particleArr);
        
        // Compute forces 
        float3 pForce = computePressureForce(pId, neighborArr, particleArr );
        
        // Compute new velocities and positinons 
        float3 acce = pForce / particleArr[pId].mass;

        float3 vel = newVelocities[pId];
        float3 pos = particleArr[pId].pos;
        integrator.update(vel, pos, acce);

        particleArr[pId].vel = vel;
        newPositions[pId] = pos;
    }

    // Update particle and grid 
    for(unsigned pId = 0; pId < particleArr.size(); pId++){
        float3 pos = newPositions[pId];
        float3 vel = particleArr[pId].vel;

        float3 oldPos = particleArr[pId].pos;

        reflectAtBoundary(oldPos, pos, vel);
        
        particleArr[pId].pos = pos;
        particleArr[pId].vel = vel;
        
        // Update grid 
        int preGridId = particleArr[pId].gridId;
        int curGridId = grid.computeGridId(particleArr[pId].pos );
        if(preGridId != curGridId){
            grid.updateParticle(pId, curGridId, preGridId);
            particleArr[pId].gridId = curGridId;
        }
    }

    float timeEnd = getTimeElapse();
    printf("From %.4f to %.4f\n", timeStart, timeEnd );

    delete [] neighborArrs;
    delete [] newVelocities;
    delete [] newPositions;

    return isNewFrame;
}


void eosSolver::computeDensityCorrection(int pId, std::unordered_set<int>& neighborArr, 
        std::vector<particle>& particleArr, float3* velocities )
{
    float3 vec1 = velocities[pId]; 
    float3 pos1 = particleArr[pId].pos; 
    float densityCorrection = 0;

    for(const auto& nId : neighborArr){
        if(nId == pId)
            continue;
        float3 pos2 = particleArr[nId].pos;
        float3 vec2 = velocities[nId]; 
        densityCorrection += timeStep * (vec1-vec2).dot(kernelGrad(pos1, pos2) );
    }

    particleArr[pId].density += densityCorrection;
}


