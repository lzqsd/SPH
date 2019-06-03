#include "initializer.h"

initializer::initializer(int n, float d, float mRate){
    numOfParticles = n;
    if(numOfParticles <= 0){
        std::cout<<"Wrong: the number of particles has to be larger than 0."<<std::endl;
        exit(1);
    }
    densityRest = d; 
    marginRate = mRate;
}

void initializer::initialize(
        std::vector<particle>& particleArr, 
        const std::vector<float3>& points,
        const std::vector<float3>& waterBox,
        float& length, float& width, float& height, 
        float& size, 
        float& minX, float& minY, float& minZ)
{
    computeGridParameters(
            points, waterBox, 
            length, width, height, size, 
            minX, minY, minZ );

    initializeParticles(
            size, 
            particleArr, waterBox );
}

void initializer::computeGridParameters(
            const std::vector<float3>& points, 
            const std::vector<float3>& waterBox, 
            float& length, float& width, float& height, 
            float& size, 
            float& minX, float& minY, float& minZ
            )
{
    if(waterBox.size() != 4){
        std::cout<<"Error: the initial water volume must be a box."<<std::endl;
        exit(1);
    }
    
    // Compute the boundary
    minX = 1e10f; minY = 1e10f; minZ = 1e10f;
    float maxX=-1e10f, maxY = -1e10f, maxZ=-1e10f; 
    for(unsigned i = 0; i < points.size(); i++){
        float3 f = points[i];

        maxX = std::max(maxX, f.x);
        maxY = std::max(maxY, f.y);
        maxZ = std::max(maxZ, f.z);

        minX = std::min(minX, f.x);
        minY = std::min(minY, f.y);
        minZ = std::min(minZ, f.z);
    }
    
    for(unsigned i = 0; i < waterBox.size(); i++){
        float3 f = waterBox[i];

        maxX = std::max(maxX, f.x);
        maxY = std::max(maxY, f.y);
        maxZ = std::max(maxZ, f.z);

        minX = std::min(minX, f.x);
        minY = std::min(minY, f.y);
        minZ = std::min(minZ, f.z); 
    }
    
    // Compute the simulation region 
    float L = maxX - minX;
    float W = maxY - minY;
    float H = maxZ - minZ;

    length = L * (1 + 2 * marginRate );
    width = W * (1 + 2 * marginRate );
    height = H * (1 + 2 * marginRate );

    minX -= L * marginRate;
    minY -= W * marginRate;
    minZ -= H * marginRate;

    float volumeUnit = (L * W * H) / numOfParticles;
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

                if( (dx + k) * h > length || (dy + j) * h > width 
                        || (dz + i) * h > height) {
                    continue;
                }

                float3 pos = sp + dx*h*xAxis + dy*h*yAxis + dz*h*zAxis;
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
