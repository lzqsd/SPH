#include "uniformGrid.h"

uniformGrid::uniformGrid(float L, float W, float H, float S ){
    width = W;
    height = H;
    length = L;
    size = S;

    lNum = int(ceil( length / size) );
    wNum = int(ceil( width / size) );
    hNum = int(ceil( height / size) );
    gridSize = lNum * wNum * hNum;

    pTable = new std::unordered_set<int> [gridSize ];
}

// Add particle to grid 
void uniformGrid::updateParticle(int pId, int curGridId, int preGridId){  

    // Insert pId into new grid
    if(curGridId < 0 || curGridId >= gridSize){
        std::cout<<"Error: current grid Id beyond the range when updating particle!"<<std::endl;
        exit(1);
    }
    std::unordered_set<int>::iterator it;
    it = pTable[curGridId].find( pId );
    if(it != pTable[curGridId].end() ){
        std::cout<<"Error: particle Id has already beein in current grid when updating particle!"<<std::endl;
        exit(1);
    }
    pTable[curGridId].insert(pId );

    // Remove pId from previous grid
    if(preGridId >= 0){
        if(preGridId >= gridSize ){
            std::cout<<"Error: previous grid Id beyond the range when updating particle!"<<std::endl;
            exit(1);
        }
        it = pTable[preGridId ].find(pId ); 
        if(it == pTable[preGridId].end() ){
            std::cout<<"Error: particle Id does not in previous grid when updating particle!"<<std::endl;
            exit(1);
        }
        pTable[preGridId].erase(it );
    }
}

// Search for neighbors 
std::vector<int> uniformGrid::query(int pId, int gridId, const std::vector<particle>& pArr){

    std::vector<int> neighborArr;

    int hId = int(gridId / (wNum * lNum) );
    int wId = int(int(gridId % (wNum * lNum) ) / lNum );
    int lId = int(int(gridId % (wNum * lNum) ) % lNum );
    
    // Check neighboring cells 
    for(int h = hId-1; h <= hId+1; h++){
        if(h < 0 || h >= hNum){
            continue;
        }
        for(int w = wId-1; w <= wId+1; w++){
            if(w < 0 || w >= wNum){
                continue;
            }
            for(int l = lId-1; l <= lId+1; l++){
                if(l < 0 || l >= lNum){
                    continue;
                }
                int newGridId = (h * wNum + w) * lNum + l;
                for(auto it = pTable[newGridId].begin(); it != pTable[newGridId].end(); it++){
                    int newPId = *it;
                    float3 diff = (pArr[pId].pos - pArr[newPId].pos);
                    if(diff.norm2() < size){
                        neighborArr.push_back(newPId);
                    }
                }
            }
        }
    }

    return neighborArr;
}
