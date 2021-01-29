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
#include <string>
#include <iostream>

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef std::string string;

class Material {
    string method;
    string distribution;
    string shadowmask;
    vec3 emissive;
    float roughness;
    vec3 diffuseColor;
    vec3 F0;
    
    //    vec3 diffuse;
    //    vec3 specular;
    //    float phongExp;
    //    vec3 reflectance;
    
public:
    Material();
    Material(string m, string d, string s,  vec3 e, float r, vec3 dc, vec3 f0);
    void set(string m, string d, string s,  vec3 e, float r, vec3 dc, vec3 f0);
    
    bool isLight();
    vec3 getEmissive();
    
    vec3 BRDF(vec3 normal, vec3 incoming, vec3 outgoing);
    
    // For a pure, Lambertian (diffuse) surface
    vec3 Lambert(vec3 normal, vec3 incoming, vec3 outgoing);
    vec3 CookTorrance(vec3 normal, vec3 incoming, vec3 outgoing);
    
    float BeckmannD(vec3 normal, vec3 half_angle);
    
    float MaskingShadowing(vec3 normal, vec3 incoming, vec3 outgoing);
    float BeckmannG(vec3 normal, vec3 incoming, vec3 outgoing);
    float BeckmannGsub(vec3 normal, vec3 vec);
    float CookTorranceG(vec3 normal, vec3 incoming, vec3 outgoing);
    
    vec3 SchlickFresnel(vec3 outgoing, vec3 half_angle);
    
    // Chooses a random incoming direction based on a probability distribution
    void randDir(vec3 normal, vec3 &direction, float &probability);
    
    void LambertRandDir(vec3 normal, vec3 &direction, float &probability);
};

#endif /* material_hpp */

