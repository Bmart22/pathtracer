//
//  geometry.hpp
//  
//
//  Created by Brendan Martin on 6/29/20.
//

#ifndef geometry_hpp
#define geometry_hpp

#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "variables.hpp"

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

struct Ray {
    vec3 origin;
    vec3 path;
};

struct Camera {
    vec3 position;
    vec3 direction;
    float focalLength;
};

//struct Light {
//    vec3 position;
//    vec3 intensity;
//};

class Material {
    vec3 diffuse;
    vec3 specular;
    float phongExp;
    vec3 reflectance;
    vec3 emissive;
    
public:
    Material();
    Material(vec3 diff, vec3 spec, float p, vec3 ref, vec3 em);
    void set(vec3 diff, vec3 spec, float p, vec3 ref, vec3 em);
    
    bool isLight();
    vec3 getEmissive();
    
    // For a pure, Lambertian (diffuse) surface
    vec3 BRDF(vec3 normal, vec3 incoming, vec3 outgoing);
    
    // Chooses a random incoming direction based on a probability distribution
    void randDir(vec3 normal, vec3 &direction, float &probability);
};

class Sphere {
    vec3 position;
    float radius;
    Material* material;
public:
    Sphere();
    Sphere(vec3 pos, float rad, Material* mat);
    void set(vec3 pos, float rad, Material* mat);
    
    vec3 getPosition();
    float getRadius();
    Material* getMaterial();
    
    bool intersects(Ray ray, float minTime, float maxTime );
    bool intersects(Ray ray, float &time, float minTime, float maxTime);
    bool intersects(Ray ray, vec3 &location, vec3 &normal, float &time, float minTime, float maxTime);
    
    void sampleLight(vec3 location, vec3 &direction, float &probability);

};

class Mesh {
    // Vertices are stored counterclockwise
    float vertices[9];
    Material* material;

public:
    Mesh();
    Mesh(float verts[], Material* mat);
    void set(float verts[], Material* mat);
    
    vec3 getVertex( int ind );
    vec3 getNormal();
    Material* getMaterial();
    
    bool intersects(Ray ray, float minTime, float maxTime );
    bool intersects(Ray ray, float &time, float minTime, float maxTime);
    bool intersects(Ray ray, vec3 &location, vec3 &normal, float &time, float minTime, float maxTime);
};



#endif /* geometry_hpp */
