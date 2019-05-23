#include "writer.h"

bool writeRawParticleData(std::string outputDir, std::vector<particle> particleArr, int frameCount )
{
    char fileNameBuffer[100];
    if(outputDir.back() == '/'){
        sprintf(fileNameBuffer, "%s%08d.dat", outputDir.c_str(), frameCount );
    }
    else{
        sprintf(fileNameBuffer, "%s/%08d.dat", outputDir.c_str(), frameCount );
    }

    std::ofstream fOut(fileNameBuffer, std::ios::out | std::ios::binary);
    if(fOut ){
        unsigned particleNum = particleArr.size();
        fOut.write((char*)&particleNum, sizeof(unsigned ) );
        for(unsigned i = 0; i < particleNum; i++){
            float3 pos = particleArr[i].pos;  
            fOut.write((char*)&(pos.x), sizeof(float ) );
            fOut.write((char*)&(pos.y), sizeof(float ) );
            fOut.write((char*)&(pos.z), sizeof(float ) );
        }
        fOut.close();
        return true;
    }

    return false;
}
