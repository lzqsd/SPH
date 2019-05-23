#include "basicStruct.h"
#include <cmath>

float3::float3(float xx, float yy, float zz){
    x = xx; y = yy; z = zz;
}

float3::float3(const float3& f){
    x = f.x; y = f.y; z = f.z;
}

void float3::make_float3(float xx, float yy, float zz){
    x = xx; y = yy; z = zz;
}

void float3::make_float3(float xx){
    x = xx; y = xx; z = xx;
}

void float3::make_float3(const float3& f){
    x = f.x; y = f.y; z = f.z;
}

float float3::norm2(){
    float sum = x * x + y * y + z * z;
    return sqrt(sum); 
}

float float3::norm1(){
    return fabs(x) + fabs(y) + fabs(z);
}

// Plus 
float3 operator+(const float3& f1, const float3& f2){
    float3 f;
    f.x = f1.x + f2.x;
    f.y = f1.y + f2.y;
    f.z = f1.z + f2.z;
    return f;
}

float3 operator+(const float3& f1, float f2){
    float3 f;
    f.x = f1.x + f2;
    f.y = f1.y + f2;
    f.z = f1.z + f2;
    return f; 
}

float3 operator+(float f1, const float3& f2){
    float3 f;
    f.x = f1 + f2.x;
    f.y = f1 + f2.y;
    f.z = f1 + f2.z;
    return f;
}

// Minus 
float3 float3::operator-(){
    float3 f;
    f.x = -x;
    f.y = -y;
    f.z = -z;
    return f;
}

// Subtraction 
float3 operator-(const float3& f1, const float3& f2){
    float3 f;
    f.x = f1.x - f2.x;
    f.y = f1.y - f2.y;
    f.z = f1.z - f2.z;
    return f;
}

float3 operator-(const float3& f1, float f2){
    float3 f;
    f.x = f1.x - f2;
    f.y = f1.y - f2;
    f.z = f1.z - f2;
    return f; 
}

float3 operator-(float f1, const float3& f2){
    float3 f;
    f.x = f1 - f2.x;
    f.y = f1 - f2.y;
    f.z = f1 - f2.z;
    return f;
}

// Product
float3 operator*(const float3& f1, const float3& f2){
    float3 f;
    f.x = f1.x * f2.x;
    f.y = f1.y * f2.y;
    f.z = f1.z * f2.z;
    return f;
}

float3 operator*(const float3& f1, float f2){
    float3 f;
    f.x = f1.x * f2;
    f.y = f1.y * f2;
    f.z = f1.z * f2;
    return f; 
}

float3 operator*(float f1, const float3& f2){
    float3 f;
    f.x = f1 * f2.x;
    f.y = f1 * f2.y;
    f.z = f1 * f2.z;
    return f;
}

// Quotient 
float3 operator/(const float3& f1, const float3& f2){
    float3 f;
    f.x = f1.x / f2.x;
    f.y = f1.y / f2.y;
    f.z = f1.z / f2.z;
    return f;
}

float3 operator/(const float3& f1, float f2){
    float3 f;
    f.x = f1.x / f2;
    f.y = f1.y / f2;
    f.z = f1.z / f2;
    return f; 
}

float3 operator/(float f1, const float3& f2){
    float3 f;
    f.x = f1 / f2.x;
    f.y = f1 / f2.y;
    f.z = f1 / f2.z;
    return f;
}

// Dot product 
float float3::dot(const float3& f){
    return x * f.x + y * f.y + z * f.z;
}

// Equal 
float3 float3::operator=(const float3& f){
    x = f.x;
    y = f.y;
    z = f.z;
    return *this;
}
