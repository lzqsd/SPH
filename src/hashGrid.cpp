#include "hashGrid.h"

hashGrid::hashGrid(int gn, float s){
    size = s; 
    gridNum = gn;  
    pTable.resize(gridNum );
}

int hashGrid::computeGridId(float3 pos){
    std::int64_t x = std::int64_t(pos.x / size );
    std::int64_t y = std::int64_t(pos.y / size );
    std::int64_t z = std::int64_t(pos.z / size );
    int gridId = computeGridId(x, y, z );
    return gridId; 
}

int hashGrid::computeGridId(std::int64_t x, std::int64_t y, std::int64_t z){
    int gridId = ( (x ^ y) ^ z) % std::int64_t(gridNum );
    return gridId; 
}

void hashGrid::updateParticle(int pId, int curId, int preId ){
    // Insert pId into new grid
    if(curId < 0 || curId >= gridNum ){
        std::cout<<"Error: current grid Id beyond the range when updating particle!"<<std::endl;
        exit(1);
    }
    std::unordered_set<int>::iterator it;
    it = pTable[curId].find( pId );
    if(it != pTable[curId].end() ){
        std::cout<<"Error: particle Id has already beein in current grid when updating particle!"<<std::endl;
        exit(1);
    }
    pTable[curId].insert(pId );

    // Remove pId from previous grid
    if(preId >= 0){
        if(preId >= gridNum ){
            std::cout<<"Error: previous grid Id beyond the range when updating particle!"<<std::endl;
            exit(1);
        }
        it = pTable[preId ].find(pId ); 
        if(it == pTable[preId].end() ){
            std::cout<<"Error: particle Id does not in previous grid when updating particle!"<<std::endl;
            exit(1);
        }
        pTable[preId].erase(it );
    }
}

std::vector<int> hashGrid::query(int pId, int gridId, std::vector<particle> pArr ){
    particle P = pArr[pId ];

    std::int64_t x = std::int64_t(P.pos.x / size );
    std::int64_t y = std::int64_t(P.pos.y / size );
    std::int64_t z = std::int64_t(P.pos.z / size );

    std::vector<int> pIdArr; 
    for(std::size_t i=-1; i<=1; i++){
        for(std::size_t j=-1; j<=1; j++){
            for(std::size_t k=-1; k<=1; k++){
                gridId = computeGridId(x+i, y+j, z+k);
                for(auto it = pTable[gridId].begin(); it != pTable[gridId].end(); it++){
                    int newPId = *it;
                    if(newPId == pId ){
                        continue;
                    }
                    float3 diff = (pArr[pId].pos - pArr[newPId].pos);
                    if(diff.norm2() < size){
                        pIdArr.push_back(newPId);
                    }
                }   
            }
        }
    }
    return pIdArr;
}
