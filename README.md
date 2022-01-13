# Pathtracing Graphics Engine
 A path tracing graphics engine, created for my own edification. The program is capable of rendering spheres with spherical lights.
 
 The integrator samples both direct and indirect lighting. Russian roulette determines the number of indirect lighting bounces that are evaluated.
 
 The material class includes several different BRDF's: Lambertian, Cook-Torrance, and Smith Multi-scattering. The available microfacet distributions are Beckmann and GGX. The Smith Multi-scattering BRDF is based on the paper "Multiple-Scattering Microfacet BSDFs with the Smith Model."

# How to run
The "layout.txt" file can be edited to contain scene information. For example: 


// mat   method   dist    type    emissive   rough  diffuseColor      fresnel

material smith beckmann conductor vec3(0,0,0) 0.4 vec3(0.0,0.9,0.0) vec3(0.9,0.0,0.9)

material lambert beckmann beckmann vec3(200,200,200), 0.0, vec3(0,0,0), vec3(0.0,0,0)

sphere vec3(3,0,3) 3 mat0

light vec3(-8,0,0) 3 mat1

camera vec3(0,5,0) vec3(0,-1,0) 1


The first argument of each line determines the type of object to add, while successive arguments specify the parameters of that object. The "material" object requires many parameters, but depending on the BRDF that is chosen, they .

"Sphere" requires a positional vector, a radius, and a material. "Light," which specifies a spherical light, requires a vector for position, a radius, and a material. "Camera" requires a position, a view direction, and a focal length.

Material objects are stored in an array in the order they are created. To assign a material to an object, use "mat#", where "#" is the index into the array of materials.

Once the layout file is complete, running "pathtracer layout.txt" will generate a rendered image.

