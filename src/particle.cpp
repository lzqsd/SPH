#include "particle.h"

int particle::computeGridId(float size, float length, float width, float height, 
        float minX, float minY, float minZ )
{
    if(pos.x > length || pos.y > width || pos.z > height ){
        std::cout<<"Error: particle goes out of the boundary!"<<std::endl;
        exit(1);
    }

    int lNum = int(ceil( length / size) );
    int wNum = int(ceil( width / size) );

    int lId = int( (pos.x - minX) / size );
    int wId = int( (pos.y - minY) / size );
    int hId = int( (pos.z - minZ) / size );

    gridId = (hId * wNum + wId) * lNum + lId;
    
    return gridId;
}
