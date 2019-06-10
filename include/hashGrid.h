#ifndef HASHGRID_HEADER
#define HASHGRID_HEADER 

#include <vector>
#include <cstdint>
#include <algorithm>
#include <unordered_set>

#include "basicStruct.h"
#include "particle.h"

class hashGrid{
public:
    hashGrid(int gn, float s); 
    
    int getGridNum(){return gridNum; }     
    int computeGridId(float3 pos );
    int computeGridId(long long x, long long y, long long z);
    void updateParticle(int pId, int curId, int preId=-1); 
    std::unordered_set<int> query(int pId, int gridId, 
            std::vector<particle> particleArr );
    
private: 
    std::vector<std::unordered_set<int> > pTable; 
    int gridNum; 
    float size; 

    static const long long p1 = 73856093; 
    static const long long p2 = 19349663; 
    static const long long p3 = 83492791;
};

#endif
