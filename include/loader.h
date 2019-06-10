#ifndef LOADER_HEADER
#define LOADER_HEADER 

#include "basicStruct.h"

#include <vector>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cctype>
#include <fstream> 
#include <sstream>
#include <iostream>

void loadWaterBox(std::string fileName, std::vector<float3>& waterBox );

namespace objLoader {

struct mesh_t {
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> texcoords;
    std::vector<int> indicesP;
    std::vector<int> indicesN;
    std::vector<int> indicesT;
    std::vector<int> materialIds; // per-mesh material ID
    std::vector<std::string > materialNames; 
    std::vector<int > materialNameIds;
};


struct shape_t {
    mesh_t mesh; 
    float ks;
    float ds;

    shape_t(float k = 1e3, float d = 1e-3){
        ks = k;
    }
}; 

#define TINYOBJ_SSCANF_BUFFER_SIZE  (4096)

struct vertex_index {
    int v_idx, vt_idx, vn_idx;
    vertex_index(){
        v_idx = -1;
        vt_idx = -1;
        vn_idx = -1;
    }
    vertex_index(int idx) : v_idx(idx), vt_idx(idx), vn_idx(idx){}
    vertex_index(int vidx, int vtidx, int vnidx)
        : v_idx(vidx), vt_idx(vtidx), vn_idx(vnidx){}
};

bool LoadObj(shape_t& shape, const std::string& filename);

}

#endif 
