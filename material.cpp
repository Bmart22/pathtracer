//
//  material.cpp
//  
//
//  Created by Brendan Martin on 8/7/20.
//

#include "material.hpp"

// Material Class

Material::Material() {
    method = "cooktorrance";
    distribution = "beckmann";
    shadowmask = "beckmann";
    emissive = vec3(0.0f);
    roughness = 0.5;
    diffuseColor = vec3(0.5f);
    F0 = vec3(0.5f);  // range [0,1]
    
    
//    diffuse = vec3(0.0f);
//    specular = vec3(0.0f);
//    phongExp = 0;
//    reflectance = vec3(0.0f);
//    emissive = vec3(0.0f);
}

Material::Material(string m, string d, string s, vec3 e, float r, vec3 dc, vec3 f0) {
    
    method = m;
    distribution = d;
    shadowmask = s;
    emissive = e;
    roughness = r;
    diffuseColor = dc;
    F0 = f0;
    
//    diffuse = diff;
//    specular = spec;
//    phongExp = p;
//    reflectance = ref;
//    emissive = em;
    
}

void Material::set(string m, string d, string s, vec3 e, float r, vec3 dc, vec3 f0) {
    
    method = m;
    distribution = d;
    shadowmask = s;
    emissive = e;
    roughness = r;
    diffuseColor = dc;
    F0 = f0;
    
//    diffuse = diff;
//    specular = spec;
//    phongExp = p;
//    reflectance = ref;
//    emissive = em;
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
    if (method == "lambert") {
        return Lambert(normal, incoming, outgoing);
    }
    if (method == "cooktorrance") {
        return CookTorrance(normal, incoming, outgoing);
    }
    
    return Lambert(normal, incoming, outgoing);
}

// For a pure, Lambertian (diffuse) surface
vec3 Material::Lambert(vec3 normal, vec3 incoming, vec3 outgoing) {
    return diffuseColor/glm::pi<float>();
}

vec3 Material::CookTorrance(vec3 normal, vec3 incoming, vec3 outgoing) {
    // Because Torrance model assumes facets reflect specularly,
    // Calculate the half-angle vector for the distribution function
    vec3 half_angle = glm::normalize(incoming + outgoing);
    
    vec3 numerator = vec3(0.0f);
    if (distribution == "beckmann") {
        float dist = BeckmannD(normal, half_angle);
        
        float atten = MaskingShadowing(normal,incoming,outgoing);
        
        vec3 fres = SchlickFresnel(outgoing, half_angle);
        
        numerator = dist * atten * fres;
    } else {
        numerator = vec3(0.5f);
    }
    
    float denominator = 4 * glm::dot(normal,incoming) * glm::dot(normal,outgoing);
    return numerator / denominator;
}

// Beckmann Distribution Function, based on gaussian
float Material::BeckmannD(vec3 normal, vec3 half_angle) {
    float a = roughness;

    float ndotm = glm::dot(normal,half_angle);
    
    float numerator = glm::exp( (ndotm*ndotm - 1) / (a * a * ndotm * ndotm) );
    
    float denominator = glm::pi<float>()* a * a *glm::pow(ndotm,4);
    
    return numerator / denominator;
}

float Material::MaskingShadowing(vec3 normal, vec3 incoming, vec3 outgoing) {
    if (shadowmask == "beckmann") {
        return BeckmannG(normal, incoming, outgoing);
    }
    if (shadowmask == "cooktorrance") {
        return CookTorranceG(normal, incoming, outgoing);
    }
    return BeckmannG(normal, incoming, outgoing);
}

float Material::BeckmannG(vec3 normal, vec3 incoming, vec3 outgoing) {
    return BeckmannGsub(normal, incoming) * BeckmannGsub(normal, outgoing);
}

// Beckmann Shadowing-Masking Function
float Material::BeckmannGsub(vec3 normal, vec3 vec) {
    float a = roughness;
    float ndotv = glm::dot(normal, vec);
    float c = ndotv / (a * glm::sqrt(1 - ndotv*ndotv ));
    
    if (c >= 1.6) {
        return 1.0f;
    } else {
        return (3.535*c + 2.181*c*c)/(1 + 2.276*c + 2.577*c*c);
    }
    
}

float Material::CookTorranceG(vec3 normal, vec3 incoming, vec3 outgoing) {
    vec3 half_angle = glm::normalize(incoming + outgoing);
    float ndoth = glm::dot(normal,half_angle);
    float ndoti = glm::dot(normal,incoming);
    float ndoto = glm::dot(normal,outgoing);
    
    float m = glm::min(1.0f, 2*ndoth*ndoto/ndoto);
    return glm::min(m, 2*ndoth*ndoti/ndoto);
    
}

vec3 Material::SchlickFresnel(vec3 outgoing, vec3 half_angle) {
    float a = 1 - glm::dot(outgoing,half_angle);
    return F0 + (vec3(1.0f)-F0) * a*a*a*a*a;
}



void Material::randDir(vec3 normal, vec3 &direction, float &probability) {
    if (method == "lambert") {
        return LambertRandDir(normal, direction, probability);
    }
    return LambertRandDir(normal, direction, probability);
}

// Chooses a random incoming direction based on a uniform probability distribution
void Material::LambertRandDir(vec3 normal, vec3 &direction, float &probability) {
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
    
    direction = glm::normalize( glm::mat3( xaxis, yaxis, normal ) * randVec );
    probability = 1/( 2*PI );
    
}
