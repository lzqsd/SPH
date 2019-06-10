#include <iostream>
#include <vector> 
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "basicStruct.h"
#include "initializer.h"
#include "loader.h"
#include "particle.h"
#include "solver.h"
#include "eosSolver.h"
#include "writer.h"

int main(int argc, char** argv )
{   
    // Water box 
    std::string waterFile; 
    // Container, will keep still forever 
    std::vector<std::string > containerFiles; 
    // Objects, can be moved by water 
    std::vector<std::string > objFiles;
    // Output directory 
    std::string outputDir;
    
    // Length of the video
    float timeLength = 3.0f;

    // Simulatuion parameters 
    int framePerSec = 30;
    float3 gravity = float3(0.0f, -9.8f, 0.0f); 
    float densityRest = 1e3f;
    float stiffness = 1.119e3f; 
    float viscosity = 1e-6;
    int numOfParticles = 10000;
    bool noDivergence = false;

    // hash grid parameter 
    int gridNum = 10000;

    for(int i = 0; i < argc; i++){
        if(i == 0){
            continue;
        }
        else if(std::string(argv[i]) == std::string("-water") ){
            // load the water box 
            if(i == argc - 1){
                std::cout<<"Missing input variable"<<std::endl;
                exit(1);
            }
            waterFile = std::string(argv[++i] ); 
        }
        else if(std::string(argv[i] ) == std::string("-objs") ){
            if(i == argc - 1){
                std::cout<<"Missing input variable"<<std::endl;
                exit(1);
            }
            while(i + 1 <= argc-1 && argv[i+1][0] != '-'){
                objFiles.push_back(std::string(argv[++i] ) );
            }
        }
        else if(std::string(argv[i]) == std::string("-output") ){
            if(i == argc - 1){
                std::cout<<"Missing input variable"<<std::endl;
                exit(1);
            }
            outputDir = std::string(argv[++i] );
        }
        else if(std::string(argv[i] ) == std::string("-time") ){
            if(i == argc - 1){
                std::cout<<"Missing input variable"<<std::endl;
                exit(1);
            }
            timeLength = atof(argv[++i] );
        }
        else if(std::string(argv[i]) == std::string("-fps") ){
            if(i == argc - 1){
                std::cout<<"Missing input variable"<<std::endl;
                exit(1);
            }
            framePerSec = atoi(argv[++i] );
        }
        else if(std::string(argv[i]) == std::string("-gravity") ){
            if(i == argc - 1){
                std::cout<<"Missing input variable"<<std::endl;
                exit(1);
            }
            gravity = float3(0.0f, -atof(argv[++i]), 0.0f);
        }
        else if(std::string(argv[i]) == std::string("-densityRest") ){
            if(i == argc - 1){
                std::cout<<"Missing input variable"<<std::endl;
                exit(1);
            }
            densityRest = atof(argv[++i] );
        }
        else if(std::string(argv[i]) == std::string("-stiffness") ){
            if(i == argc - 1){
                std::cout<<"Missing input variable"<<std::endl;
                exit(1);
            }
            stiffness = atof(argv[++i] );
        }
        else if(std::string(argv[i]) == std::string("-viscosity") ){
            if(i == argc - 1){
                std::cout<<"Missing input variable"<<std::endl;
                exit(1);
            }
            viscosity = atof(argv[++i] );
        }
        else if(std::string(argv[i]) == std::string("-numOfParticles") ){
            if(i == argc - 1){
                std::cout<<"Missing input variable"<<std::endl;
                exit(1); 
            }
            numOfParticles = atoi(argv[++i] );
        }
        else if(std::string(argv[i]) == std::string("-gridNum") ){
            if(i == argc - 1){
                std::cout<<"Missing input variable"<<std::endl;
                exit(1); 
            }
            gridNum = atoi(argv[++i] );
        }
        else if(std::string(argv[i]) == std::string("--noDivergence") ){
            noDivergence = true;
        }
        else{
            std::cout<<"Unrecognizable input command"<<std::endl;
            exit(1);
        }
    }

    std::cout<<"Fluid simulation."<<std::endl;

    // load water box  
    std::vector<float3> waterBox;
    loadWaterBox(waterFile, waterBox );

    // load obj 
    std::vector<objLoader::shape_t> shapes(objFiles.size() );
    for(unsigned n = 0; n < shapes.size(); n++){
        std::cout<<"Load Objects: "<<objFiles[n]<<std::endl;
        objLoader::LoadObj(shapes[n], objFiles[n] );
    }

    // Initialize the initializer 
    initializer init(numOfParticles, densityRest );

    // Init the particles
    float size;
    std::vector<particle> particleArr; 
    init.initialize(particleArr, waterBox, size );

    // Init solver 
    solver* fluidSolver = NULL;
    if(noDivergence == true){
        fluidSolver = new eosSolver(size, shapes, 
                gridNum, framePerSec, 
                gravity, densityRest, stiffness, viscosity );
    }
    else{
        fluidSolver = new solver(size, shapes, 
                gridNum, framePerSec, 
                gravity, densityRest, stiffness, viscosity );
    }
    fluidSolver -> init(particleArr );


    // Start simulating  
    while(fluidSolver -> getTimeElapse() < timeLength){
        bool isNextFrame = fluidSolver -> update(particleArr );
        if(isNextFrame == true){
            printf("Frame %d, Time %.4f/%.4f \n", 
                    fluidSolver -> getFrameCount(), fluidSolver -> getTimeElapse(), timeLength);
            writeRawParticleData(outputDir, particleArr, fluidSolver -> getFrameCount() );
        }
    }
    delete fluidSolver;
    
    return 0;
}
