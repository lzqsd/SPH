#include "solver.h"

solver::solver( float S, const std::vector<objLoader::shape_t>& shs,  
        const std::vector<float3>& box, 
        int gridNum, int fps,
        float3 G, float d, float stiff, float v ): 
    grid(hashGrid(gridNum, S) ), 
    integrator(eulerIntegrator( 1.0f / float(fps) ) )
{
    densityRest = d; 
    stiffness = stiff;
    viscosity = v;
    gravityAcce = G;
    size = S;
    volumeBox = box;

    framePerSec = fps;
    timeStep = 1.0f / framePerSec;
    timeElapse = 0.0f;
    frameCount = 0.0f; 

    shapes = shs;
}
    
// Init grid 
void solver::init(std::vector<particle>& particleArr )
{
    for(unsigned pId = 0; pId < particleArr.size(); pId++){
        int gridId = grid.computeGridId(particleArr[pId].pos );
        grid.updateParticle(pId, gridId );
        particleArr[pId].gridId = gridId;
    }
    std::cout<<"Finish building grid!"<<std::endl;
}

// Set time step according to CFL rule 
bool solver::CFLRule(std::vector<particle > particleArr )
{   
    float vMax = 0;
    for(unsigned i = 0; i < particleArr.size(); i++){
        if(particleArr[i].isValid == false)
            continue;
        float3 v = particleArr[i].vel;
        if(v.norm2() > vMax ){
            vMax = v.norm2();
        }
    }
    float h = size / 2.0;
    float t = 0.4 * h / std::max(vMax, 1e-10f);
    if(t + timeElapse > float(frameCount +1.0f) / float(framePerSec) ){
        timeStep = float(frameCount + 1.0f) / float(framePerSec) - timeElapse;
        timeElapse = float(frameCount + 1.0f) / float(framePerSec );
        frameCount += + 1;
        return true;
    }
    else{
        timeElapse += t;
        timeStep = t;
        return false;
    }
    std::cout<<"Time step: "<<timeStep<<std::endl;
    return true;
}


// Steps to compute velocity
void solver::computeDensity(int pId, std::unordered_set<int>& neighborArr, 
        std::vector<particle>& particleArr )
{
    float3 pos1 = particleArr[pId].pos; 
    float mass1 = particleArr[pId].mass;
    float density = kernel(pos1, pos1 ) * mass1;

    for(const auto& nId : neighborArr){
        if(nId == pId )
            continue;
        float3 pos2 = particleArr[nId].pos;
        float mass2 = particleArr[nId].mass;
        density += kernel(pos1, pos2) * mass2;
    }
    particleArr[pId].density = density;
}

void solver::computePressure(int pId, std::vector<particle>& particleArr )
{
    float density = particleArr[pId].density;
    float pressure = stiffness * ( pow(density / densityRest, 7.0f) - 1);
    particleArr[pId ].pressure = pressure;
}

float3 solver::computePressureForce(int pId, std::unordered_set<int>& neighborArr, 
            std::vector<particle>& particleArr )
{
    float3 force(0, 0, 0); 
    float m1 = particleArr[pId].mass;
    float p1 = particleArr[pId].pressure;
    float d1 = particleArr[pId].density;
    float3 pos1 = particleArr[pId].pos;

    for(const auto& nId : neighborArr){
        float m2 = particleArr[nId].mass;
        float p2 = particleArr[nId].pressure;
        float d2 = particleArr[nId].density;
        float3 pos2 = particleArr[nId].pos;

        float3 grad = kernelGrad(pos1, pos2);
        float coef = m2 * ( p1 / d1 / d1 + p2 / d2 / d2);
        force = force + coef * grad;
    }
    force = - m1 * force;

    return force;
}

float3 solver::computeViscosityForce(int pId, std::unordered_set<int>& neighborArr, 
            std::vector<particle>& particleArr )
{
    float3 force(0, 0, 0);
    float h = size / 2.0f;
    float m1 = particleArr[pId].mass;
    float3 pos1 = particleArr[pId].pos;
    float3 vel1 = particleArr[pId].vel;

    for(const auto& nId : neighborArr){
        float m2 = particleArr[nId].mass;
        float d2 = particleArr[nId].density;
        float3 pos2 = particleArr[nId].pos;
        float3 vel2 = particleArr[nId].vel;

        float3 x12 = pos1 - pos2;
        float3 v12 = vel1 - vel2;
        float3 grad = kernelGrad(pos1, pos2);

        float coef = 2 * m2 / d2 * x12.dot(grad ) / (x12.dot(x12) + 0.01*h*h);
        force = force + coef * (v12 );
    }
    force = m1 * viscosity * force; 
    return force;
}

float3 solver::computeGravityForce(int pId, std::vector<particle>& particleArr )
{
    float3 gForce = particleArr[pId].mass * gravityAcce;
    return gForce;
}

// check if a point is in the box 
bool solver::checkValid(const float3& pos ){
    float3 pos_r = pos - volumeBox[0];
    float3 xAxis = volumeBox[1] - volumeBox[0];
    float3 yAxis = volumeBox[2] - volumeBox[0];
    float3 zAxis = volumeBox[3] - volumeBox[0];

    float L = xAxis.norm2();
    float W = yAxis.norm2(); 
    float H = zAxis.norm2();
    
    xAxis = xAxis / L; 
    yAxis = yAxis / W; 
    zAxis = zAxis / H;

    float dotX = pos_r.dot(xAxis);
    float dotY = pos_r.dot(yAxis);
    float dotZ = pos_r.dot(zAxis);

    if(dotX > L || dotX < 0 || 
            dotY > W || dotY < 0 ||
            dotZ > H || dotZ < 0)
    {
        return false;
    }
    else{
        return true;
    }
}


// Reflect the velocity and location if hit a boundary 
bool solver::reflectAtBoundary(const float3& pos1, float3& pos2, float3& velocity){
    bool isHit = false;
    for(unsigned n = 0; n < shapes.size(); n++){
        unsigned faceNum = shapes[n].mesh.indicesP.size() / 3;
        for(unsigned i = 0; i < faceNum; i++){
            int v1id = shapes[n].mesh.indicesP[3*i];
            int v2id = shapes[n].mesh.indicesP[3*i+1];
            int v3id = shapes[n].mesh.indicesP[3*i+2];

            float3 v1(shapes[n].mesh.positions[3*v1id], 
                    shapes[n].mesh.positions[3*v1id + 1], 
                    shapes[n].mesh.positions[3*v1id+2] );
            float3 v2(shapes[n].mesh.positions[3*v2id], 
                    shapes[n].mesh.positions[3*v2id + 1], 
                    shapes[n].mesh.positions[3*v2id+2] );
            float3 v3(shapes[n].mesh.positions[3*v3id], 
                    shapes[n].mesh.positions[3*v3id + 1], 
                    shapes[n].mesh.positions[3*v3id+2] ); 

            float3 N = (v2 - v1).cross(v3 - v1);
            float normN = N.norm2();
            N = N / std::max(normN, 1e-10f); 
            
            float de = (pos1 - v1).dot(N) - (pos2 - v1).dot(N);
            float nu = (pos1 - v1).dot(N);
            
            if(de == 0 or de * nu < -1e-2 or fabs(nu) > fabs(de) * 1.01 ){
                continue;
            }
            float t = nu / de;
            float3 hitpoint = pos1 + t * (pos2 - pos1);

            // check if the hitpoint is inside triangle 
            float ind1 = (v2 - v1).cross(hitpoint - v1).dot( 
                    (hitpoint - v1).cross(v3-v1) );
            if(ind1 < 0){
                continue;
            }

            float ind2 = (v1 - v2).cross(hitpoint - v2).dot(
                    (hitpoint - v2).cross(v3 - v2) );
            if(ind2 < 0){
                continue;
            }

            float ind3 = (v1 - v3).cross(hitpoint - v3).dot(
                    (hitpoint - v3).cross(v2 - v3) );
            if(ind3 < 0){
                continue;
            }
            
            // Reflect position and velocity 
            isHit = true;
            float3 newDirect = (hitpoint - pos1) 
                - 2 * (hitpoint - pos1).dot(N) * N;
            newDirect = newDirect / std::max(newDirect.norm2(), 1e-10f);
            
            float posNorm = (pos2 - pos1).norm2() * fabs(1.0f - nu / de);
            float velNorm = velocity.norm2();
            
            pos2 = 0.9 * posNorm * newDirect + hitpoint;
            velocity = 0.9 * velNorm * newDirect;
            if(isHit == true ){
                break;
            }
        }
        if(isHit == true){
            break;
        }
    }
    return isHit;
}

// Kernel function 
float solver::kernel(float3 pos1, float3 pos2){
    float dist = (pos1 - pos2).norm2();
    float h = size / 2;
    float q = dist / h;
    
    float f_q = 0;
    if( q >= 0 && q < 1){
        f_q = 2.0f / 3.0f - q * q + 0.5f * q * q * q;
    }
    else if(q >= 1 && q < 2){ 
        f_q = 1.0 / 6.0f * pow(2.0f - q, 3.0f );
    }

    f_q *= 1 / pow(h, 3.0f) * 3.0f / 2.0f / PI; 
    
    return f_q;
}

float3 solver::kernelGrad(float3 pos1, float3 pos2){
    float dist = (pos1 - pos2).norm2();
    float h = size / 2;
    float q = dist / h;

    float3 grad = (pos1 - pos2) / dist; 
    
    float f_q_grad = 0;
    if( q >= 0 && q < 1){
        f_q_grad = -2 * q + 3.0f / 2.0f * pow(q, 2.0f);
    }
    else if(q >= 1 && q < 2){ 
        f_q_grad = -0.5 * pow(2.0f - q, 2.0f);
    }
    f_q_grad = 1 / pow(h, 4.0f) * 3.0f / 2.0f / PI * f_q_grad;
    grad = f_q_grad * grad;
    
    return grad; 
}
