#ifndef WRITER_HEADER 
#define WRITER_HEADER 

#include "basicStruct.h"
#include "particle.h"

#include <fstream> 
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string>

bool writeRawParticleData(std::string outputDir, std::vector<particle> particleArr, int frameCount );


#endif
