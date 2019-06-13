#ifndef WRITER_HEADER 
#define WRITER_HEADER 

#include "basicStruct.h"
#include "particle.h"
#include "densityVolume.h"

#include <fstream> 
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string>

bool writeRawParticleData(std::string outputDir, const std::vector<particle>& particleArr, int frameCount );

bool writeVolumeData(std::string outputDir, const densityVolume& volume, int frameCount);


#endif
