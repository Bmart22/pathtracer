//
//  material.hpp
//  
//
//  Created by Brendan Martin on 8/7/20.
//

#ifndef material_hpp
#define material_hpp

#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

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

#endif /* material_hpp */
