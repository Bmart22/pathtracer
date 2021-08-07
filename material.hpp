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
    // method is either lambert or cooktorrance or smith
    // distribution is beckmann or ggx
    // shadowmask is either beckmaan or cooktorrance
    // roughness [0,1] is used for Beckmann distribution and masking-shadowing
    // diffuseColor [0, 255] is the diffuse color for lambert
    // F0 [0, 1] determines the color of light that is reflected
    
    string method;
    string distribution;
    string shadowmask;
    string material;
    vec3 emissive;
    float roughness;
    vec3 diffuseColor;
    vec3 F0;
    
public:
    Material();
    Material(string m, string d, string s, vec3 e, float r, vec3 dc, vec3 f0);
    void set(string m, string d, string s, vec3 e, float r, vec3 dc, vec3 f0);
    
    bool isLight();
    vec3 getEmissive();
    
    mat3 genCoorFrame(vec3 z_axis);
    vec3 BRDF(vec3 normal, vec3 incoming, vec3 outgoing);
    
    // For a pure, Lambertian (diffuse) surface
    vec3 Lambert(vec3 normal, vec3 incoming, vec3 outgoing);
    
    // CookTorrance uses the Beckmann distribution function, but can use either
    // the Beckmann or CookTorrance masking-shadowing function
    vec3 CookTorrance(vec3 normal, vec3 incoming, vec3 outgoing);
    
    
    
    float MaskingShadowing(vec3 normal, vec3 incoming, vec3 outgoing);
    float SmithGUncorrelated(vec3 incoming, vec3 outgoing);
    float G1(vec3 vec);
    float CookTorranceG(vec3 normal, vec3 incoming, vec3 outgoing);
    
    vec3 SchlickFresnel(vec3 outgoing, vec3 half_angle);
    
    // Chooses a random incoming direction based on a probability distribution
    void randDir(vec3 normal, vec3 &direction, float &probability);
    
    void LambertRandDir(vec3 normal, vec3 &direction, float &probability);
    
    
    float Distribution(vec3 half_angle);
    float BeckmannD(vec3 half_angle);
    float GGXD(vec3 half_angle);
    
    vec3 Smith(vec3 incoming, vec3 outgoing);
    
    void SamplePhase(vec3 microNormal, vec3 incoming, vec3& outgoing, vec3& weight);
    vec3 Phase(vec3 incoming, vec3 outgoing);
    float SampleHeight(vec3 direction, float height);
    float SmithG(vec3 incoming, float height);
    
    // Cumulative Distribution of Heights (gaussian)
    float CumulativeDist(float height);
    // Inverse Cumulative Distribution of Heights (gaussian)
    float InvCumulativeDist(float num);
    
    float LambdaG(vec3 w);
    
    
    // Sampling procedure is from Heitz 2016 supplemental material
    vec3 SampleNorm(vec3 direction, float ax, float ay);
    void SampleBeckmann(float theta_i, float& slope_x, float& slope_y);
    void SampleGGX(float theta_i, float& slope_x, float& slope_y);
    
    // Implementation modeled on Giles 2012
    float erfinv(float x);
    
    // Returns angle between w and normal
    float vec2angle(vec3 normal, vec3 w);
};

#endif /* material_hpp */

