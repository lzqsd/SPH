#ifndef DENSITYVOLUME_HEADER
#define DENSITYVOLUME_HEADER 

#include "basicStruct.h"
#include "hashGrid.h"
#include "particle.h" 

#include <vector>
#include <cmath> 
#include <iostream>
#include <thread>

class densityVolume{
public:
    densityVolume(const std::vector<float3>& b, int numOfVolumes, float ks, float d, int th = 12);  

    const std::vector<float>& getVolume() const{ return volume; }
    void getResolution(unsigned& rX, unsigned& rY, unsigned& rZ) const {
        rX = resX; rY = resY; rZ = resZ;
    }
    void getAxis(float3& x, float3& y, float3& z) const {
        x = xAxis; y = yAxis; z = zAxis;
    }
    float getSize() const { return size; }
    float getKernelSize() const { return kernelSize; } 
    float3 getOrigin() const{ return box[0]; }

    void buildVolume(const std::vector<particle>& particleArr, const hashGrid& grid);
    
    // Multi thread build volum 
    void buildVolumeUnit(unsigned zs, unsigned ze, const std::vector<particle>& particleArr, const hashGrid& grid);
    
protected: 
    unsigned resX, resY, resZ;
    float3 xAxis, yAxis, zAxis;
    std::vector<float> volume; 
    std::vector<float3> box;
    int numOfVolumes;
    float size; 
    float kernelSize; 
    float densityRest;
    unsigned threadNum;

    // Kernel function 
    float kernel(float3 pos1, float3 pos2);

    
};

#endif
