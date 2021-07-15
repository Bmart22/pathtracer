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
#include <math.h>

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef std::string string;

class Material {
    // method is either lambert or cooktorrance
    // distribution is only beckmann
    // shadowmask is either beckmaan or cooktorrance
    // roughness [0,1] is used for Beckmann distribution and masking-shadowing
    // diffuseColor [0, 255] is the diffuse color for lambert
    // F0 [0, 1] determines the color of light that is reflected
    
    string method;
    string distribution;
    string shadowmask;
    vec3 emissive;
    float roughness;
    vec3 diffuseColor;
    vec3 F0;
    
public:
    Material();
    Material(string m, string d, string s,  vec3 e, float r, vec3 dc, vec3 f0);
    void set(string m, string d, string s,  vec3 e, float r, vec3 dc, vec3 f0);
    
    bool isLight();
    vec3 getEmissive();
    
    vec3 BRDF(vec3 normal, vec3 incoming, vec3 outgoing);
    
    // For a pure, Lambertian (diffuse) surface
    vec3 Lambert(vec3 normal, vec3 incoming, vec3 outgoing);
    
    // CookTorrance uses the Beckmann distribution function, but can use either
    // the Beckmann or CookTorrance masking-shadowing function
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

