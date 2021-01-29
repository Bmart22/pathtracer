//
//  main.cpp
//  
//
//  Created by Brendan Martin on 6/29/20.
//

#include <stdio.h>
#include <iostream>
#include <FreeImage.h>
#include <glm/glm.hpp>
#include "geometry.hpp"
#include "material.hpp"
#include "variables.hpp"

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

// screenHeight and screenWidth are expressed in pixels
float screenHeight = 500;
float screenWidth = 500;

struct Sphere lights[10];
int lightsUsed;

struct Camera cam = { vec3(0,5,0), vec3(0,-1,0), 1 };

Sphere objects[10];
//Mesh objects[10];
int numObjects;
Material materials[10];

Ray genCameraRay( int xCoor, int yCoor ) {
    float worldHeight = screenHeight / 100;
    float worldWidth = screenWidth / 100;
    
    vec3 right = glm::normalize( glm::cross( cam.direction, vec3(0.0,0.0,1.0) ) );
    vec3 up = glm::normalize( glm::cross( right, cam.direction ) );
    
    float u = (-worldWidth/2) + worldWidth*(xCoor+0.5)/screenWidth;
    float v = (-worldHeight/2) + worldHeight*(yCoor+0.5)/screenHeight;
    
    Ray camRay;
    camRay.origin = cam.position;
    // FreeImage begins at the lower left corner
    camRay.path = (cam.focalLength * glm::normalize(cam.direction)) + (u*right) + (v*up);
    
    return camRay;
}

//vec3 genDirect(float cosThetaMax, vec3 location, Sphere light) {
//
//}

int findClosestObject(Ray ray, vec3 &location, vec3 &normal, float &time, float minTime, float maxTime) {
    
    int closestObj = -1;
    for (int obj = 0; obj < numObjects; obj++) {
        // If the current object is intersected and closer than the previous object
        if (objects[obj].intersects(ray, location, normal, time, minTime, time)) {
            closestObj = obj;
        }
    }
    return closestObj;
}

int findClosestLight(Ray ray, float &time, float minTime, float maxTime) {

    int closestLight = -1;
    for (int l = 0; l < lightsUsed; l++) {
        // If the current object is intersected and closer than the previous object
        if (lights[l].intersects(ray, time, 0.01, maxTime)) {
            closestLight = l;
        }
    }
    return closestLight;
}

// Function is called once per view ray
vec3 tracepath( Ray ray, int depth = 0 ) {
    
    vec3 color = vec3(0,0,0);
    
    // Find the closest object
    vec3 location = vec3(0,0,0);
    vec3 normal = vec3(0,0,0);
    float time = std::numeric_limits<float>::infinity();
    int closestObj = findClosestObject(ray, location, normal, time, 0.01, time);
    
    // Find closest light
    float lightTime = std::numeric_limits<float>::infinity();
    int closestLight = findClosestLight(ray, lightTime, 0.01, time);
    
    // Check if a light has been hit and the light is closer than the nearest object
    if (closestLight != -1 && lightTime < time) {
        // Return the emission of the light
        return lights[closestLight].getMaterial()->getEmissive();
    }
    
    // Otherwise, if an object has been hit
    if (closestObj != -1) {
        
        // Sample direct illumination
        if (depth == 0) {
            for (int l = 0; l < lightsUsed; l++) {
                vec3 incoming;
                float prob;
                lights[l].sampleLight(location, incoming, prob);
                
                Ray directRay = {location, incoming};
                float shadowBound;
                lights[l].intersects(directRay, shadowBound, 0.01, std::numeric_limits<float>::infinity());
                
                bool inShadow = false;
                int obj = 0;
                while (!inShadow && obj < numObjects) {
                    // Check if the light is in shadow
                    inShadow = objects[obj].intersects(directRay, 0.01, shadowBound);
                    obj++;
                }
                
                if (!inShadow) {
                    color += lights[l].getMaterial()->getEmissive() / prob;
                }
            }
            
        }
        
        // Sample Indirect Lighting
        
        // Exit Condition: Russian Roulette
        float rouletteCutoff = 0.2;
        float roulette = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        
        if (roulette > rouletteCutoff) {
        
            // Generate new random direction and the probability of choosing that direction
            vec3 incoming;
            float prob;
            objects[closestObj].getMaterial()->randDir(normal, incoming, prob);
            
            // Calculate the amount of incoming light reflected in the outgoing direction
            vec3 brdf = objects[closestObj].getMaterial()->BRDF(normal, incoming, -glm::normalize(ray.path));
            
            // Calculate the cos of angle between normal vector and incoming light
            float cos_theta = glm::dot(incoming, normal);
            
            // Record new ray to trace
            ray.origin = location;
            ray.path = incoming;
            
            // Compute the transport equation, continue to recurse
            color += brdf * tracepath(ray, depth+1) * cos_theta / (prob * (1-rouletteCutoff));
        }
    }
    
    return color;
}

int main(int argc, char* argv[]) {
//    lights[0].position = vec3(5,5,0);
//    lights[0].intensity = vec3(1,1,1);
//    lights[1].position = vec3(0,5,0);
//    lights[1].intensity = vec3(0.5,0.5,0.5);
//    lightsUsed = 2;
    
    // Materials for objects
//    materials[0].set(vec3(100,100,100), vec3(100,100,100), 100, vec3(0.6f), vec3(0.0f));
//    materials[1].set(vec3(200,0,0), vec3(100,100,100), 100, vec3(0.0f), vec3(0.0f));
    
    // Material for light
    materials[0].set("lambert", "beckmann", "cooktorrance", vec3(0), 0.1, vec3(0.0,0.0,0.9), vec3(0.5,0.3,0.6));
    materials[2].set("lambert", "beckmann", "beckmann", vec3(200), 0.0, vec3(0.0f), vec3(0.0f));
    
    
    // Objects
    objects[0].set(vec3(3,0,0), 3, &materials[0]);
    objects[1].set(vec3(-3,0,0), 2, &materials[0]);
    numObjects = 2;
    
    // Lights
    lights[0].set(vec3(5,5,0), 1, &materials[2]);
    lights[1].set(vec3(0,5,0), 1, &materials[2]);
    lightsUsed = 2;
    
//    float verts[9] = {0,0,4, 4,0,-4, -4,2,-4};
//    objects[0].set(verts, &materials[0]);
//    float verts2[9] = {2,3,4, 2,3,-4, 1,0,0};
//    objects[1].set(verts2, &materials[1]);
//    numObjects = 2;

    FreeImage_Initialise();

    int bitsPerPixel = 24;
    FIBITMAP* bitmap = FreeImage_Allocate(screenWidth, screenHeight, bitsPerPixel);
    
    int numSamples = 32;
    vec3 colVec;
    RGBQUAD color;
    for (int i = 0; i < screenWidth; i++) {
        for (int j = 0; j < screenHeight; j++) {
            colVec = vec3(0.0f);
            for (int n = 0; n < numSamples; n++) {
                colVec += tracepath( genCameraRay(i,j) );
            }
            colVec = colVec / (float)numSamples;
            
            // Clamp color values
            colVec = glm::min(colVec, vec3(255));
            color.rgbRed = colVec.z;
            color.rgbGreen = colVec.y;
            color.rgbBlue = colVec.x;
            FreeImage_SetPixelColor(bitmap,i,j,&color);
        }
    }
    
    FreeImage_Save(FIF_PNG, bitmap, "image.png", 0);
    FreeImage_DeInitialise();
}
