# SPH

Thie repository contains code of smoothed particle hydrodynamics (SPH). 
The implementation follows the EGSR 2014 tutorial "SPH Fluids in Computer Graphics".
Two solvers are included: the basic solver and the non-iterative EOS solver. 
Code to generate 3D mesh using marching cube is also provided. 

## Compile the code
To compile the code 
```
cmake . 
make 
```

## Fluid simulation 
To run basic solver
```
./build/SPH -water data/water.txt -obj data/box.obj -volume data/volume.txt
```
To run the divergence free solver
```
./build/SPH -water data/water.txt -obj data/box.obj -volume data/volume.txt -noDivergence
```

## Generate 3D mesh 
To generate the 3D meshes after simulation
```
python visualizeVolume.py
```
