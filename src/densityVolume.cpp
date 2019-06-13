#include "densityVolume.h"

densityVolume::densityVolume(const std::vector<float3>& b, int numOfVolumes, float ks, float d, int th) {
    box = b;
    densityRest = d;
    kernelSize = ks;
    threadNum = th;

    if(box.size() != 4){
        std::cout<<"Wrong: the TSDF must be a cube defined by 4 points."<<std::endl;
        exit(1);
    }

    xAxis = box[1] - box[0];
    yAxis = box[2] - box[0];
    zAxis = box[3] - box[0];
    
    float L = xAxis.norm2();
    float W = yAxis.norm2();
    float H = zAxis.norm2();
    
    if(L == 0 || W == 0 || H == 0){
        std::cout<<"Wrong: simulation region can not be 0"<<std::endl;
        exit(1);
    }
    
    float volumeUnit = (L * W * H) / numOfVolumes;
    size = cbrt(volumeUnit );


    xAxis = xAxis / L;
    yAxis = yAxis / W;
    zAxis = zAxis / H;

    resX = ceil(L / size);
    resY = ceil(W / size);
    resZ = ceil(H / size);


    if(xAxis.dot(yAxis) > 1e-3 || xAxis.dot(zAxis) > 1e-3 
            || yAxis.dot(zAxis) > 1e-3){
        std::cout<<"Wrong: the TSDF should be a cube!"<<std::endl;
        exit(1);
    }
    
    volume.resize(resX * resY * resZ, -1.0f);
}

// Kernel function 
float densityVolume::kernel(float3 pos1, float3 pos2){
    float dist = (pos1 - pos2).norm2();
    float h = kernelSize / 2;
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
void densityVolume::buildVolume(const std::vector<particle>& particleArr, const hashGrid& grid){ 
    std::vector<std::thread> tArr; 
    unsigned gap = ceil(resZ / threadNum);
    for(unsigned t = 0; t < threadNum; t++){
        unsigned zs = t * gap;
        unsigned ze = std::min(resZ, zs + gap);
        if(zs >= resZ){
            break;
        }
        tArr.push_back(std::thread(&densityVolume::buildVolumeUnit, 
                    this, zs, ze, std::ref(particleArr), std::ref(grid) ) );
    }
    for(unsigned t = 0; t < threadNum; t++){
        tArr[t].join();
    }
}

void densityVolume::buildVolumeUnit(unsigned zs, unsigned ze, const std::vector<particle>& particleArr, const hashGrid& grid){  

    if(zs < 0 or zs > resZ or ze < 0 or ze > resZ or zs >= ze){
        std::cout<<"Wrong: the z id of densityVolume is beyond range."<<std::endl;
        exit(1);
    }
    for(unsigned zId = zs; zId < ze; zId++){
        for(unsigned yId = 0; yId < resY; yId++){
            for(unsigned xId = 0; xId < resX; xId++){
                int vId = zId * resX * resY + yId * resX + xId;

                float3 pos = (xId + 0.5) * size * xAxis 
                    + (yId + 0.5) * size * yAxis 
                    + (zId + 0.5) * size * zAxis + box[0];

                std::unordered_set<int> neighborArr = grid.query(pos, particleArr);
                float density = 0;
                for(const auto& nId : neighborArr){
                    float3 pos2 = particleArr[nId].pos; 
                    float mass = particleArr[nId].mass;
                    density += kernel(pos, pos2) * mass;
                }
                volume[vId] = density / densityRest - 0.2;
            }
        }
    }
}
