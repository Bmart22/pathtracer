//
//  geometry.cpp
//  
//
//  Created by Brendan Martin on 6/29/20.
//

#include <iostream>
#include "geometry.hpp"
#include <stdlib.h>


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


// Sphere Class

// Default constructor
Sphere::Sphere() {
    position = vec3(0.0f);
    radius = 0;
    material = NULL;
}

// Constructor for the Sphere class
Sphere::Sphere(vec3 pos, float rad, Material* mat) {
    position = pos;
    radius = rad;
    material = mat;
}

void Sphere::set(vec3 pos, float rad, Material* mat) {
    position = pos;
    radius = rad;
    material = mat;
}

vec3 Sphere::getPosition() {
    return position;
}

float Sphere::getRadius() {
    return radius;
}

Material* Sphere::getMaterial() {
    return material;
}

bool Sphere::intersects(Ray ray, float minTime, float maxTime) {
    float t = 0.0;
    
    // Origin minus position (OMP)
    vec3 OMP = ray.origin - position;
    float path_2 = glm::dot(ray.path, ray.path);
    
    // Calculate the discriminant
    float discriminant = glm::dot(ray.path, OMP)*glm::dot(ray.path, OMP) - path_2 * (glm::dot(OMP,OMP) - (radius*radius));
    
    // If the dicriminant is less than 0, the ray does not intersect the sphere
    if (discriminant < 0.0) {
        return false;
    }
    // Otherwise, the ray intersects the sphere at least once.
    else{
        // Find the time value when discriminant = 0 (tangential intersection)
        t = glm::dot(-ray.path, OMP) / path_2;
        
        // If the discriminant is non-zero, there are two intersections
        // Find the smaller of the two time values
        if (discriminant > 0.0) {
            t = t - (glm::sqrt(discriminant)/path_2);
        }
        
        if (t > minTime && t < maxTime) {
            return true;
        }
    }
    return false;
}

bool Sphere::intersects(Ray ray, float &time, float minTime, float maxTime) {
    float t = 0.0;
    
    // Origin minus position (OMP)
    vec3 OMP = ray.origin - position;
    float path_2 = glm::dot(ray.path, ray.path);
    
    // Calculate the discriminant
    float discriminant = glm::dot(ray.path, OMP)*glm::dot(ray.path, OMP) - path_2 * (glm::dot(OMP,OMP) - (radius*radius));
    
    // If the dicriminant is less than 0, the ray does not intersect the sphere
    if (discriminant < 0.0) {
        return false;
    }
    // Otherwise, the ray intersects the sphere at least once.
    else{
        // Find the time value when discriminant = 0 (tangential intersection)
        t = glm::dot(-ray.path, OMP) / path_2;
        
        // If the discriminant is non-zero, there are two intersections
        // Find the smaller of the two time values
        if (discriminant > 0.0) {
            t = t - (glm::sqrt(discriminant)/path_2);
        }
        
        if (t > minTime && t < maxTime) {
            time = t;
            return true;
        }
    }
    return false;
}
    
// Color and normal passed by reference
// Determine if ray hits sphere, update color and normal
// Return boolean value indicating if the sphere is hit
bool Sphere::intersects(Ray ray, vec3 &location, vec3 &normal, float &time, float minTime, float maxTime) {
    bool success = intersects(ray, time, minTime, maxTime);
    
    if (success) {
        location = ray.origin + (time * ray.path);
        normal = glm::normalize( location - position );
    }
    
    return success;
}

void Sphere::sampleLight(vec3 location, vec3 &direction, float &probability) {
    float d = glm::distance(position, location);
    float r = radius;
    float cosThetaMax = glm::sqrt(d*d - r*r)/d;
    
    float PI = glm::pi<float>();
    
    // Generate two random floats in range (0,1)
    float cosTheta = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    cosTheta = glm::mix(cosThetaMax, 1.0f, cosTheta);
    float phi = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    phi = phi * PI * 2;

    float sinTheta = glm::sqrt(1 - (cosTheta*cosTheta));
    vec3 randVec = vec3( sinTheta * glm::cos(phi), sinTheta * glm::sin(phi), cosTheta);
    
    vec3 zaxis = glm::normalize(position - location);
    vec3 yaxis;
    if (zaxis != vec3(0.0,0.0,1.0)) {
        yaxis = glm::normalize( glm::cross( zaxis, vec3(0.0,0.0,1.0) ) );
    } else {
        yaxis = glm::normalize( glm::cross( zaxis, vec3(0.0,1.0,0.0) ) );
    }
    vec3 xaxis = glm::normalize( glm::cross( yaxis, zaxis ) );
    
    direction = glm::mat3( xaxis, yaxis, zaxis ) * randVec;
    probability = 1/( 2*PI*(1-cosThetaMax) );
}


// Mesh Class
//
// Default constructor
Mesh::Mesh() {
    for (int i = 0; i < 9; i++) {
        vertices[i] = 0;
    }
    material = NULL;
}

// Vertices are stored counterclockwise
Mesh::Mesh(float verts[], Material* mat) {
    for (int i = 0; i < 9; i++) {
        vertices[i] = verts[i];
    }
    material = mat;
}

void Mesh::set(float verts[], Material* mat) {
    for (int i = 0; i < 9; i++) {
        vertices[i] = verts[i];
    }
    material = mat;
}

vec3 Mesh::getVertex( int ind ) {
    if (ind >= 0 && ind < 3) {
        return vec3( vertices[(ind*3)+0], vertices[(ind*3)+1], vertices[(ind*3)+2] );
    }
    return vec3(0,0,0);
}

vec3 Mesh::getNormal() {
    vec3 edge1 = getVertex(1) - getVertex(0);
    vec3 edge2 = getVertex(2) - getVertex(0);
    return glm::normalize( glm::cross(edge1, edge2) );
}

Material* Mesh::getMaterial() {
    return material;
}

bool Mesh::intersects(Ray ray, float minTime, float maxTime) {
    float dummy;
    return intersects(ray, dummy, minTime, maxTime);
}

bool Mesh::intersects(Ray ray, float &time, float minTime, float maxTime) {
    vec3 edge_ba = getVertex(0) - getVertex(1);
    vec3 edge_ca = getVertex(0) - getVertex(2);
    vec3 aMinusOrigin = getVertex(0) - ray.origin;

    float ei_hf = edge_ca[1] * ray.path[2] - ray.path[1] * edge_ca[2];
    float gf_di = -(edge_ca[0] * ray.path[2] - ray.path[0] * edge_ca[2]);
    float dh_eg = edge_ca[0] * ray.path[1] - ray.path[0] * edge_ca[1];

    float M = edge_ba[0] * ei_hf + edge_ba[1] * gf_di + edge_ba[2] * dh_eg;


    float beta = (aMinusOrigin[0] * ei_hf + aMinusOrigin[1] * gf_di + aMinusOrigin[2] * dh_eg) / M;

    // Condition for early termination
    if (beta < 0 || beta > 1) {
        return false;
    }

    float ak_jb = edge_ba[0] * aMinusOrigin[1] - aMinusOrigin[0] * edge_ba[1];
    float jc_al = -(edge_ba[0] * aMinusOrigin[2] - aMinusOrigin[0] * edge_ba[2]);
    float bl_kc = edge_ba[1] * aMinusOrigin[2] - aMinusOrigin[1] * edge_ba[2];

    float gamma = (ray.path[2] * ak_jb + ray.path[1] * jc_al + ray.path[0] * bl_kc) / M;

    // Condition for early termination
    if (gamma < 0 || gamma > 1-beta) {
        return false;
    }

    // If we have gotten this far, the ray has hit the triangle
    // Calculate necessary values
    float t = -(edge_ca[2] * ak_jb + edge_ca[1] * jc_al + edge_ca[0] * bl_kc) / M;
    if (t > minTime && t < maxTime) {
        time = t;
        return true;
    }

    return false;
}

bool Mesh::intersects(Ray ray, vec3 &location, vec3 &normal, float &time, float minTime, float maxTime) {
    bool success = intersects(ray, time, minTime, maxTime);

    if (success) {
        location = ray.origin + (time * ray.path);
        normal = getNormal();
    }

    return success;
}



