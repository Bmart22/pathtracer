//
//  variables.hpp
//  
//
//  Created by Brendan Martin on 7/8/20.
//

#ifndef variables_hpp
#define variables_hpp

#include <stdio.h>
#include "geometry.hpp"
#include "material.hpp"

//typedef glm::mat3 mat3;
//typedef glm::mat4 mat4;
//typedef glm::vec3 vec3;
//typedef glm::vec4 vec4;



// screenHeight and screenWidth are expressed in pixels
float screenHeight = 500;
float screenWidth = 500;

Sphere lights[10];
int numLights;

Camera cam = { vec3(0,5,0), vec3(0,-1,0), 1 };

Sphere objects[10];
//Mesh objects[10];
int numObjects;
Material materials[10];

//vec3 lightPosition[10];
//vec3 lightIntensity[10];
//int lightsUsed;

#endif /* variables_hpp */
