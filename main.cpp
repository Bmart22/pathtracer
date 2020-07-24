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
#include "variables.hpp"

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

// screenHeight and screenWidth are expressed in pixels
float screenHeight = 500;
float screenWidth = 500;

//struct Light lights[10];
//int lightsUsed;

struct Camera cam = { vec3(0,5,0), vec3(0,-1,0), 1 };

//Sphere objects[10];
Sphere objects[10];
int numObjects;

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
    //std::cout << glm::normalize(cam.direction).y;
    
    return camRay;
}

// Function is called once per view ray
vec3 tracepath( Ray ray, int depth = 0 ) {
    
    // Exit Condition
    if (depth > 5) { return vec3(0.0f); }
    
    vec3 color = vec3(0,0,0);
    vec3 location = vec3(0,0,0);
    vec3 normal = vec3(0,0,0);
    float time = std::numeric_limits<float>::infinity();
    int closestObj = -1;
    
    // Loop over every object
    for (int obj = 0; obj < numObjects; obj++) {
        // If the current object is intersected and closer than the previous object
        if (objects[obj].intersects(ray, location, normal, time, 0.01, time)) {
            closestObj = obj;
        }
    }

    if (closestObj != -1) {
        
        // If the closest objects is a light, return the emission of the light
        if (objects[closestObj].emissive != vec3(0.0f)) {
            return objects[closestObj].emissive;
        }
        
        // Otherwise, calculate the light reflecting off the object
        
        // Generate new random direction and the probability of choosing that direction
        vec3 incoming;
        float prob;
        randDir(incoming, prob);
        
        // Calculate the amount of incoming light reflected in the outgoing direction
        float brdf = BRDF(normal, incoming, ray.path);
        
        // Calculate the cos of angle between normal vector and incoming light
        float cos_theta = glm::dot(-glm::normalize(incoming), normal);
        
        // Record new ray to trace
        ray.origin = location;
        ray.path = incoming;
        
        // Compute the transport equation, continue to recurse
        color += brdf * tracepath(ray, depth+1) * cos_theta / prob;
    }
    
    return color;
}

int main(int argc, char* argv[]) {
//    lights[0].position = vec3(5,5,0);
//    lights[0].intensity = vec3(1,1,1);
//    lights[1].position = vec3(0,5,0);
//    lights[1].intensity = vec3(0.5,0.5,0.5);
//    lightsUsed = 2;
    
    // Objects
    objects[0].set(vec3(3,0,0), 3, vec3(100,100,100), vec3(100,100,100), 100, vec3(0.6f), vec3(0.0f));
    objects[1].set(vec3(-3,0,0), 2, vec3(200,0,0), vec3(100,100,100), 100, vec3(0.0f), vec3(0.0f));
    
    // Lights
    objects[2].set(vec3(5,5,0), 1, vec3(0.0f), vec3(0.0f), 0, vec3(0.0f), vec3(100));
    objects[3].set(vec3(5,5,0), 1, vec3(0.0f), vec3(0.0f), 0, vec3(0.0f), vec3(100));
    
    numObjects = 4;
    
    
//    float verts[9] = {0,0,4, 4,0,-4, -4,2,-4};
//    objects[0].set(verts, vec3(100,100,100), vec3(0,0,0), 100, vec3(0.0f));
//    float verts2[9] = {2,3,4, 2,3,-4, 1,0,0};
//    objects[1].set(verts2, vec3(200,0,0), vec3(100,100,100), 100, vec3(0,0.6,0));
    

    FreeImage_Initialise();

    int bitsPerPixel = 24;
    FIBITMAP* bitmap = FreeImage_Allocate(screenWidth, screenHeight, bitsPerPixel);
    
    RGBQUAD color;
    for (int i = 0; i < screenWidth; i++) {
        for (int j = 0; j < screenHeight; j++) {
            vec3 colVec = tracepath( genCameraRay(i,j) );
            color.rgbRed = colVec.z;
            color.rgbGreen = colVec.y;
            color.rgbBlue = colVec.x;
            FreeImage_SetPixelColor(bitmap,i,j,&color);
        }
    }
    
    FreeImage_Save(FIF_PNG, bitmap, "image.png", 0);
    FreeImage_DeInitialise();
}
