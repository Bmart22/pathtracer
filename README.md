# Pathtracing Graphics Engine
 A path tracing graphics engine, created for my own edification. The program is capable of rendering sphere under spherical lighting.
 The integrator uses sampling
 The material class uses.

# How to run
The "layout.txt" file can be edited to contain scene information. For instance:

// mat   method   dist    type    emissive   rough  diffuseColor      fresnel
material smith beckmann conductor vec3(0,0,0) 0.4 vec3(0.0,0.9,0.0) vec3(0.9,0.0,0.9)
material lambert beckmann beckmann vec3(200,200,200), 0.0, vec3(0,0,0), vec3(0.0,0,0)
sphere vec3(3,0,3) 3 mat0
light vec3(-8,0,0) 3 mat1
camera vec3(0,5,0) vec3(0,-1,0) 1

The first argument of each line determines the type of object to add, while successive arguments specify the parameters of that object. The "material" object requires many parameters.

"Sphere" requires a positional vector, a radius, and a material. "Light," which specifies a spherical light, requires a vector for position, a radius, and a material. "Camera" requires a position, a view direction, and a focal length.

