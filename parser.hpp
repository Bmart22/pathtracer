//
//  parser.hpp
//  
//
//  Created by Brendan Martin on 4/10/21.
//

#ifndef parser_hpp
#define parser_hpp

#include <stdio.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include "geometry.hpp"
#include "material.hpp"
//#include "variables.hpp"

typedef glm::vec3 vec3;
typedef std::string string;

// screenHeight and screenWidth are expressed in pixels
extern float screenHeight;
extern float screenWidth;

extern Sphere lights[10];
extern int numLights;

extern Camera cam;

extern Sphere objects[10];
//Mesh objects[10];
extern int numObjects;
extern Material materials[10];

class Parser {

public:
    Parser();
    vec3 convertVec(string command);
    void load(string file);

};

//vec3 convertVec(string command);
//void parse(string file);

#endif /* parser_hpp */
