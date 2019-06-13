#include "initializer.h"

initializer::initializer(int n, float d ){
    numOfParticles = n;
    if(numOfParticles <= 0){
        std::cout<<"Wrong: the number of particles has to be larger than 0."<<std::endl;
        exit(1);
    }
    densityRest = d; 
}

void initializer::initialize(
        std::vector<particle>& particleArr, 
        const std::vector<float3>& waterBox,
        float& size)
{
    computeGridParameters(
            waterBox, size );

    initializeParticles(
            size, 
            particleArr, waterBox );
}

void initializer::computeGridParameters(
            const std::vector<float3>& waterBox, 
            float& size
            )
{
    if(waterBox.size() != 4){
        std::cout<<"Error: the initial water volume must be a box."<<std::endl;
        exit(1);
    }
    
    // Compute the simulation region 
    float length = (waterBox[1] - waterBox[0]).norm2();
    float width = (waterBox[2] - waterBox[0]).norm2();
    float height = (waterBox[3] - waterBox[0]).norm2();

    float volumeUnit = (length * width * height) / numOfParticles;
    float h = cbrt(volumeUnit );
    size = h * 2;

    return;
}

void initializer::initializeParticles(
        float size, 
        std::vector<particle>& particleArr, 
        const std::vector<float3>& waterBox )
{
    if(!particleArr.empty()){
        std::cout<<"Warning: particle array should be empty before initialization!"<<std::endl;
        particleArr.erase(particleArr.begin(), particleArr.end() ); 
    }


    // Compute the bouding box
    float length = (waterBox[1] - waterBox[0]).norm2();
    float width = (waterBox[2] - waterBox[0]).norm2();
    float height = (waterBox[3] - waterBox[0]).norm2(); 
    
    float3 xAxis = (waterBox[1] - waterBox[0]) / length;
    float3 yAxis = (waterBox[2] - waterBox[0]) / width;
    float3 zAxis = (waterBox[3] - waterBox[0]) / height;

    if(xAxis.dot(yAxis) > 1e-3 || xAxis.dot(zAxis ) > 1e-3 || yAxis.dot(zAxis) > 1e-3){
        std::cout<<"Error: the initial water volume must be a orthogonal box"<<std::endl;
        exit(1);
    }
    
    // Randomly generate the 
    float h = size / 2;
    int lNum = int(ceil(length / h) );
    int wNum = int(ceil(width / h) );
    int hNum = int(ceil(height / h) );
    
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

    for(int i = 0; i < hNum; i++){
        for(int j = 0;  j < wNum; j++){
            for(int k = 0; k < lNum; k++){
                float3 xSp = (k * h) * xAxis;
                float3 ySp = (j * h) * yAxis;
                float3 zSp = (i * h) * zAxis;

                float3 sp = waterBox[0] + xSp + ySp + zSp;

                float dx = distribution(generator );
                float dy = distribution(generator );
                float dz = distribution(generator );

                if( (dx*0.000 + k) * h > length || (dy*0.000 + j) *h > width 
                        || (dz*0.00 + i) *h > height) {
                    continue;
                }

                float3 pos = sp + dx*0.00*h*xAxis + 
                    dy*0.00*h*yAxis + dz*0.00*h*zAxis;
                particle p;
                p.pos = pos;
                p.vel = float3(0.0f, 0.0f, 0.0f);
                p.mass = h * h * h * densityRest;

                particleArr.push_back(p );
            }
        }
    }

    return;
}
