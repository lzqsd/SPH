#ifndef EOSSOLVER_HEADER 
#define EOSSOLVER_HEADER

#include "solver.h"

class eosSolver: public solver{
public:
    eosSolver( float S, std::vector<objLoader::shape_t> shs, 
            int gridNum = 10000, int fps = 30, 
            float3 G = float3(0.0f, 9.8f, 0.0f), 
            float d = 1e3f, float stiff = 1119e3f, float v=1);
    
    bool update(std::vector<particle>& particleArr);
    void computeDensityCorrection(int pId, std::unordered_set<int>& neighborArr, 
            std::vector<particle>& particleArr, float3* velocities );
};

#endif
