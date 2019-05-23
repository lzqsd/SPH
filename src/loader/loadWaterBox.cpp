#include "loader.h"

void loadWaterBox(std::string fileName, std::vector<float3>& waterBox)
{
    if(waterBox.size() > 0){
        std::cout<<"Warning: the water box should be empty before loading."<<std::endl;
        waterBox.erase(waterBox.begin(), waterBox.end() );
    }
    std::ifstream fileRead(fileName.c_str(), std::ios::in ); 
    
    for(int n = 0; n < 4; n++){
        float3 p; 
        fileRead>>p.x>>p.y>>p.z;
        waterBox.push_back(p );
    }
}
