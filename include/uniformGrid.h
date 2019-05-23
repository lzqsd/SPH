#ifndef GRID_HERADER
#define GRID_HERADER

#include <unordered_set>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

#include "particle.h"

class uniformGrid{
public:
    uniformGrid(float L, float W, float H, float S); 
    ~uniformGrid(){
        delete [] pTable;
    }

    // Access to the parameter of grid  
    float getHeight(){ return height; }
    float getWidth(){ return width; }
    float getLength(){ return length; }
    float getSize(){ return size; }
    int getGridSizeW(){ return wNum; }
    int getGridSizeH(){ return hNum; }
    int getGridSizeL(){ return lNum; }
    int getGridSize(){ return gridSize; }

    // Add particle to grid 
    void updateParticle(int pId, int curGridId, int preGridId = -1);

    // Search for neighbors 
    std::vector<int> query(int pId, int gridId, const std::vector<particle>& pArr);

private:
    float width;
    float height;
    float length;
    // Size will be the same value of the support of kernel
    float size;

    int wNum;
    int hNum;
    int lNum; 
    int gridSize;

    std::unordered_set<int>* pTable;
};

#endif
