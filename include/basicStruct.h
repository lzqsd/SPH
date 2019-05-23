#ifndef BASICSTRUCT_HEADER
#define BASICSTRUCT_HEADER

const float PI = 3.1415926;

class float3{
public:
    float x, y, z;
    float3(float xx=0, float yy=0, float zz=0 );
    float3(const float3& f);

    void make_float3(float xx, float yy, float zz);
    void make_float3(float xx);
    void make_float3(const float3& f);

    float norm2();
    float norm1();
    
    // Add
    friend float3 operator+(const float3& f1, const float3& f2);
    friend float3 operator+(const float3& f1, float f2);
    friend float3 operator+(float f1, const float3& f2);

    // Minus 
    float3 operator-();
    
    // Subtraction
    friend float3 operator-(const float3& f1, const float3& f2);
    friend float3 operator-(const float3& f1, float f2);
    friend float3 operator-(float f1, const float3& f2);
    
    // Product 
    friend float3 operator*(const float3& f1, const float3& f2);
    friend float3 operator*(const float3& f1, float f2);
    friend float3 operator*(float f1, const float3& f2);
    
    // Quotient 
    friend float3 operator/(const float3& f1, const float3& f2);
    friend float3 operator/(const float3& f1, float f2);
    friend float3 operator/(float f1, const float3& f2);
    
    // Dot product
    float dot(const float3& f);

    // Equal 
    float3 operator=(const float3& f);
};


#endif 
