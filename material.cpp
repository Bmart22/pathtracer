//
//  material.cpp
//  
//
//  Created by Brendan Martin on 8/7/20.
//

#include "material.hpp"

// Material Class

Material::Material() {
    diffuse = vec3(0.0f);
    specular = vec3(0.0f);
    phongExp = 0;
    reflectance = vec3(0.0f);
    emissive = vec3(0.0f);
}

Material::Material(vec3 diff, vec3 spec, float p, vec3 ref, vec3 em) {
    diffuse = diff;
    specular = spec;
    phongExp = p;
    reflectance = ref;
    emissive = em;
    
}

void Material::set(vec3 diff, vec3 spec, float p, vec3 ref, vec3 em) {
    diffuse = diff;
    specular = spec;
    phongExp = p;
    reflectance = ref;
    emissive = em;
}

bool Material::isLight() {
    if (emissive != vec3(0.0f)) { return true; }
    return false;
}

vec3 Material::getEmissive() {
    return emissive;
}

// For a pure, Lambertian (diffuse) surface
vec3 Material::BRDF(vec3 normal, vec3 incoming, vec3 outgoing) {
    return vec3( glm::dot(-incoming, normal) );
}

// Chooses a random incoming direction based on a uniform probability distribution
void Material::randDir(vec3 normal, vec3 &direction, float &probability) {
    float PI = glm::pi<float>();
    
    // Generate two random floats in range (0,1)
    float cosTheta = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float phi = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    phi = phi * PI * 2;

    float sinTheta = glm::sqrt(1 - (cosTheta*cosTheta));
    vec3 randVec = vec3( sinTheta * glm::cos(phi), sinTheta * glm::sin(phi), cosTheta);
    
    vec3 yaxis;
    if (normal != vec3(0.0,0.0,1.0)) {
        yaxis = glm::normalize( glm::cross( normal, vec3(0.0,0.0,1.0) ) );
    } else {
        yaxis = glm::normalize( glm::cross( normal, vec3(0.0,1.0,0.0) ) );
    }
    vec3 xaxis = glm::normalize( glm::cross( yaxis, normal ) );
    
    direction = glm::mat3( xaxis, yaxis, normal ) * randVec;
    probability = 1/( 2*PI );
    
}
