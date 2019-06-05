#ifndef HASHGRID_HEADER
#define HASHGRID_HEADER 

#include <vector>
#include <cstdint>
#include <unordered_set>

#include "basicStruct.h"
#include "particle.h"

class hashGrid{
public:
    hashGrid(int gn, float s); 
    
    int getGridNum(){return gridNum; }     
    int computeGridId(float3 pos );
    int computeGridId(std::int64_t x, std::int64_t y, std::int64_t z);
    void updateParticle(int pId, int curId, int preId=-1); 
    std::vector<int> query(int pId, int gridId, 
            std::vector<particle> particleArr );
    
private: 
    std::vector<std::unordered_set<int> > pTable; 
    int gridNum; 
    float size; 

    static const std::int64_t p1 = 73856093; 
    static const std::int64_t p2 = 19349663; 
    static const std::int64_t p3 = 83492791;
};

#endif
