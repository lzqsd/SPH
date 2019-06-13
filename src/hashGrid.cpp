#include "hashGrid.h"

hashGrid::hashGrid(int gn, float s){
    size = s;
    gridNum = gn;

    pTable.erase(pTable.begin(), pTable.end() );
    for(int i = 0; i < gridNum; i++){
        pTable.push_back(std::unordered_set<int>() );
    }
}

int hashGrid::computeGridId(float3 pos) const {
    long long x = (long long)(pos.x / size );
    long long y = (long long)(pos.y / size );
    long long z = (long long)(pos.z / size );
    int gridId = computeGridId(x, y, z );
    return gridId; 
}

int hashGrid::computeGridId(long long x, long long y, long long z) const{
    int gridId = ( (x ^ y) ^ z) % (long long)(gridNum );
    gridId = (gridId < 0) ? gridNum + gridId : gridId;
    return gridId; 
}

void hashGrid::updateParticle(int pId, int curId, int preId ){
    // Insert pId into new gridNum 
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

std::unordered_set<int> hashGrid::query(int pId, const std::vector<particle>& pArr ) const{
    particle P = pArr[pId ];

    long long x = (long long)(P.pos.x / size );
    long long y = (long long)(P.pos.y / size );
    long long z = (long long)(P.pos.z / size );

    std::unordered_set<int> pIdArr; 
    for(long long i=-1; i<=1; i++){
        for(long long j=-1; j<=1; j++){
            for(long long k=-1; k<=1; k++){
                int gridId = computeGridId(x+i, y+j, z+k);
                for(auto it = pTable[gridId].begin(); it != pTable[gridId].end(); it++){
                    int newPId = *it;
                    if(newPId == pId ){
                        continue;
                    }
                    float3 diff = (pArr[pId].pos - pArr[newPId].pos);
                    if(diff.norm2() < size){
                        pIdArr.insert(newPId);
                    }
                }   
            }
        }
    }
    return pIdArr;
}


std::unordered_set<int> hashGrid::query(float3 pos, const std::vector<particle>& pArr ) const {

    long long x = (long long)(pos.x / size );
    long long y = (long long)(pos.y / size );
    long long z = (long long)(pos.z / size );

    std::unordered_set<int> pIdArr; 
    for(long long i=-1; i<=1; i++){
        for(long long j=-1; j<=1; j++){
            for(long long k=-1; k<=1; k++){
                int gridId = computeGridId(x+i, y+j, z+k);
                for(auto it = pTable[gridId].begin(); it != pTable[gridId].end(); it++){
                    int newPId = *it;
                    float3 diff = (pos - pArr[newPId].pos);
                    if(diff.norm2() < size){
                        pIdArr.insert(newPId);
                    }
                }   
            }
        }
    }
    return pIdArr;
}
