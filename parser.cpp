//
//  parser.cpp
//  
//
//  Created by Brendan Martin on 4/10/21.
//

#include "parser.hpp"

Parser::Parser() {

}

vec3 Parser::convertVec(string command) {
    string parameters[3];
    
    int delims[2];
    delims[0] = 4;
    
    for (int i = 0; i < 2; i++) {
        delims[1] = command.find(",", delims[0]+1);
        parameters[i] = command.substr(delims[0]+1, delims[1]-delims[0]-1);
        delims[0] = delims[1];
    }
    
    delims[1] = command.find(")", delims[0]+1);
    parameters[2] = command.substr(delims[0]+1, delims[1]-delims[0]-1);
    
    return vec3( std::stof(parameters[0]), std::stof(parameters[1]), std::stof(parameters[2]) );
}

void Parser::load(string file) {
    std::ifstream buffer;
    buffer.open(file);

    if (buffer.is_open()) {
        string str;
        int material_num = 0;
        numObjects = 0;
        numLights = 0;
        
        string command;
        int num_params;
        int delims[2];

        while(std::getline(buffer, str)) {

            delims[0] = str.find(" ");
            command = str.substr(0, delims[0]);
            
//            std::cout << command;
            
            // Determine number of parameters for the object
            if (command.compare("material") == 0) { num_params = 7; }
            else if (command.compare("sphere") == 0) { num_params = 3; }
            else if (command.compare("light") == 0) { num_params = 3; }
            else if (command.compare("camera") == 0) { num_params = 3; }
            else { num_params = 0; }
            
            // Parse the parameters
            string parameters[num_params];
            
            for (int i = 0; i < num_params; i++) {
                delims[1] = str.find(" ", delims[0]+1);
                parameters[i] = str.substr(delims[0]+1, delims[1]-delims[0]-1);
                delims[0] = delims[1];
            }
            
            // Initialize the object
            if (command.compare("material") == 0) {

                // Vectorize necessary parameters
                vec3 matvecs[3];
                matvecs[0] = convertVec(parameters[3]);
                matvecs[1] = convertVec(parameters[5]);
                matvecs[2] = convertVec(parameters[6]);

                // Add material to list
                materials[material_num].set(parameters[0],parameters[1],parameters[2],matvecs[0],std::stof(parameters[4]),matvecs[1],matvecs[2]);
                material_num++;

            } else if (command.compare("sphere") == 0) {

                // Vectorize necessary parameters
                vec3 matvec = convertVec(parameters[0]);

                // Add sphere to list
                // sphere(position, radius, material)
                objects[numObjects].set(matvec, std::stof(parameters[1]), &materials[std::stoi(parameters[2].substr(3))]);
                numObjects++;

            } else if (command.compare("light") == 0) {

                // Vectorize necessary parameters
                vec3 matvec = convertVec(parameters[0]);

                lights[numLights].set(matvec, std::stof(parameters[1]), &materials[std::stoi(parameters[2].substr(3))]);
                numLights++;
                
            } else if (command.compare("camera") == 0) {
                
                cam.position = convertVec(parameters[0]);
                cam.direction = convertVec(parameters[1]);
                cam.focalLength = std::stof(parameters[2]);
//                { convertVec(parameters[0]), convertVec(parameters[1]), std::stof(parameters[2]) };

            } else if (command.compare("//") == 0) {
                // Just a comment, do nothing

            } else {
                std::cout << "Unknown command";
            }
        }

    }
}




//vec3 convertVec(string command) {
//    return vec3(command[5], command[7], command[9]);
//}
//
//void parse(string file) {
//    std::ifstream buffer;
//    buffer.open(file);
//
//    if (buffer.is_open()) {
//        string str;
//        int material_num = 0;
//        numObjects = 0;
//        numLights = 0;
//
//        while(std::getline(buffer, str)) {
//
//            delim = str.find(" ");
//            command = str.substr(0, delim);
//
//            if (command.equals("material")) {
//                num_params = 7;
//
//                // Parse the parameters
//                int delims[2];
//                delims[0] = delim;
//                string parameters[num_params];
//                for (int i = 0, i < num_params, i++) {
//                    delims[1] = str.find(" ", delims[0]);
//                    parameters[i] = str.substr(delims[0], delims[1]);
//                    delims[0] = delims[1];
//                }
//
//                // Vectorize necessary parameters
//                vec3 matvecs[3];
//                matvecs[0] = convertVec(parameters[3]);
//                matvecs[1] = convertVec(parameters[5]);
//                matvecs[2] = convertVec(parameters[6]);
//
////                matvecs[0] = vec3(parameters[3][5], parameters[3][7], parameters[3][9]);
////                matvecs[1] = vec3(parameters[5][5], parameters[5][7], parameters[5][9]);
////                matvecs[2] = vec3(parameters[6][5], parameters[6][7], parameters[6][9]);
//
//                // Add material to list
//                materials[material_num].set(parameters[0],parameters[1],parameters[2],matvecs[0],std::stoi(parameters[4]),matvecs[1],matvecs[2]);
//                material_num++;
//
//            } else if (command.equals("sphere")) {
//                num_params = 3;
//
//                // Parse the parameters
//                int delims[2];
//                delims[0] = delim;
//                string parameters[num_params];
//                for (int i = 0, i < num_params, i++) {
//                    delims[1] = str.find(" ", delims[0]);
//                    parameters[i] = str.substr(delims[0], delims[1]);
//                    delims[0] = delims[1];
//                }
//
//                // Vectorize necessary parameters
//                vec3 matvec = convertVec(parameters[0]);
//
//                // Add sphere to list
//                // sphere(position, radius, material)
//                objects[numObjects].set(matvec, std::stof(parameters[1]), materials[std::stoi(parameters[2][3:])]);
//                numObjects++;
//
//            } else if (command.equals("light")) {
//                num_params = 3;
//
//                // Parse the parameters
//                int delims[2];
//                delims[0] = delim;
//                string parameters[num_params];
//                for (int i = 0, i < num_params, i++) {
//                    delims[1] = str.find(" ", delims[0]);
//                    parameters[i] = str.substr(delims[0], delims[1]);
//                    delims[0] = delims[1];
//                }
//
//                // Vectorize necessary parameters
//                vec3 matvec = convertVec(parameters[0]);
//
//                lights[numLights].set(matvec, std::stof(parameters[1]), materials[std::stoi(parameters[2][3:])]);
//                numLights++;
//
//            } else if (command.equals("//")) {
//                // Just a comment, do nothing
//
//            } else {
//                std::cout << "Unknown command";
//            }
//        }
//
//    }
//}
