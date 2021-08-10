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
    material = "conductor";
    emissive = vec3(0.0f);
    roughness = 0.5;
    diffuseColor = vec3(0.5f);
    F0 = vec3(0.5f);  // range [0,1]
}

Material::Material(string m, string d, string t, vec3 e, float r, vec3 dc, vec3 f0) {
    method = m;
    distribution = d;
    material = t;
    emissive = e;
    roughness = r;
    diffuseColor = dc;
    F0 = f0;
}

void Material::set(string m, string d, string t, vec3 e, float r, vec3 dc, vec3 f0) {
    method = m;
    distribution = d;
    material = t;
    emissive = e;
    roughness = r;
    diffuseColor = dc;
    F0 = f0;
}

bool Material::isLight() {
    if (emissive != vec3(0.0f)) { return true; }
    return false;
}

vec3 Material::getEmissive() {
    return emissive;
}

mat3 Material::genCoorFrame(vec3 z_axis) {
//    vec3 basis[3];
//    basis[2] = glm::normalize(z_axis);
//
//    // Develop an orthonormal basis from the provided vector
//
//    // Create nonparallel vector t
//    float a = std::numeric_limits<float>::infinity();
//    int ind = 0;
//    for (int i = 0; i<3; i++) {
//        if ( abs( basis[2][i] ) < a ) {
//            a = basis[2][i];
//            ind = i;
//        }
//    }
//    vec3 t = basis[2];
//    t[ind] = 1;
//
//    // Cross product t with basis[2] to get perpendicular axis x
//    basis[0] = glm::normalize( glm::cross(t, basis[2]) );
//
//    // Create the final axis y
//    basis[1] = glm::cross(basis[2], basis[0]);
//
//    return mat3(basis[0], basis[1], basis[2]);
    
    

    
    vec3 poss[3];
    poss[0] = vec3(-z_axis[1], z_axis[0], 0.0f);
    poss[1] = vec3(z_axis[2], 0.0f, -z_axis[0]);
    poss[2] = vec3(0.0f, -z_axis[2], z_axis[1]);
    
    int ind = 0;
    while (poss[ind] == vec3(0.0f,0.0f,0.0f)) {
        ind++;
    }
    
    z_axis = glm::normalize(z_axis);
    vec3 x_axis = glm::normalize(poss[ind]);
    vec3 y_axis = glm::cross(z_axis, x_axis);
    
    return mat3(x_axis, y_axis, z_axis);
}


// Incoming: points toward previous object bounce
// Outgoing: points toward next object bounce
vec3 Material::BRDF(vec3 normal, vec3 incoming, vec3 outgoing) {
    mat3 basis = glm::transpose(genCoorFrame(normal));
    incoming = basis * incoming;
    outgoing = basis * outgoing;
    
        
    if (method == "lambert") {
        return Lambert(vec3(0.0,0.0,1.0), incoming, outgoing);
    }
    if (method == "cooktorrance") {
        return CookTorrance(vec3(0.0,0.0,1.0), incoming, outgoing);
    }
    if (method == "smith") {
        return Smith(incoming, outgoing);
    }
    
    return Lambert(normal, incoming, outgoing);
}

// For a pure, Lambertian (diffuse) surface
vec3 Material::Lambert(vec3 normal, vec3 incoming, vec3 outgoing) {
//    return vec3(outgoing[2]);
//    return vec3(glm::dot(normal,outgoing));
    return diffuseColor/glm::pi<float>();
}

vec3 Material::CookTorrance(vec3 normal, vec3 incoming, vec3 outgoing) {
    // Because Torrance model assumes facets reflect specularly,
    // Calculate the half-angle vector for the distribution function
    vec3 half_angle = glm::normalize(incoming + outgoing);
    
    vec3 numerator = vec3(0.0f);
    if (distribution == "beckmann" || distribution == "ggx") {
        float dist = Distribution(half_angle);
        
        float atten = MaskingShadowing(incoming,outgoing);
        
        vec3 fres = SchlickFresnel(outgoing, half_angle);
        
        numerator = dist * atten * fres;
    } else {
        numerator = vec3(0.5f);
    }
    
    float denominator = 4 * incoming[2] * outgoing[2];
    return numerator / denominator;
}

float Material::MaskingShadowing(vec3 incoming, vec3 outgoing) {
    return G1(incoming) * G1(outgoing);
}

// Beckmann Shadowing-Masking Function
float Material::G1(vec3 vec) {
    return 1 / (1 + LambdaG(vec));
}

vec3 Material::SchlickFresnel(vec3 outgoing, vec3 half_angle) {
    float a = 1 - glm::dot(outgoing,half_angle);
    return F0 + (vec3(1.0f)-F0) * a*a*a*a*a;
}



void Material::sampleDir(vec3 normal, vec3 incoming, vec3 &direction, vec3 &probability) {
    if (method == "lambert") {
        LambertSampleDir(normal, direction, probability);
    }
    if (method == "smith") {
        mat3 basis = glm::transpose(genCoorFrame(normal));
        incoming = basis * incoming;
        
        SmithSampleDir(normal, incoming, direction, probability);
        
        direction = glm::transpose(basis) * direction;
    }
    LambertSampleDir(normal, direction, probability);
}

// Chooses a random incoming direction based on a uniform probability distribution
void Material::LambertSampleDir(vec3 normal, vec3 &direction, vec3 &probability) {
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
    probability = vec3( 1/( 2*PI ) );
    
}

void Material::SmithSampleDir(vec3 normal, vec3 incoming, vec3 &direction, vec3 &probability) {
    float inf = std::numeric_limits<float>::infinity();
        
    float height = InvCumulativeDist(0.9f);
    
    vec3 energy = vec3(1.0f);
    vec3 dir = -incoming;
    vec3 weight;
    int r = 0;
    
    float theta;
    float ax = roughness;
    float ay = roughness;
    vec3 microNormal;
    
//    vec3 sum = vec3(0.0f);
    
    while (true) {
        
        
        height = SampleHeight(dir, height);

        if (height == inf) {
            break;
        }
        
        
        microNormal = SampleNorm(-dir, ax, ay);
        SamplePhase(microNormal, -dir, dir, weight);
        energy = energy * weight;
        
        
        
        // Direction points opposite incoming, but Phase expects incoming
        
//        sum += energy * Phase(-direction, outgoing) * SmithG(outgoing, height);
        
        r++;
    }
    
    direction = direction;
    probability = energy;
}

// Calculate Distribution of Normals
float Material::Distribution(vec3 half_angle) {
    if (distribution == "beckmann") {
        return BeckmannD(half_angle);
    }
    if (distribution == "ggx") {
        return GGXD(half_angle);
    }
    
    return 0;
}

// Beckmann Distribution Function, based on gaussian
float Material::BeckmannD(vec3 half_angle) {
    float a = roughness;

//    float ndotm = glm::dot(normal,half_angle);
    float ndotm = half_angle[2];
    
    float numerator = glm::exp( (ndotm*ndotm - 1) / (a * a * ndotm * ndotm) );
    
    float denominator = glm::pi<float>() * a * a * glm::pow(ndotm,4);
    
    return numerator / denominator;
}

float Material::GGXD(vec3 half_angle) {
    float a = roughness;

//    float ndotm = glm::dot(normal,half_angle);
    float ndotm = half_angle[2];
    
    float part1 = glm::pi<float>() * a * a * glm::pow(ndotm,4);
    
    float part2 = 1 + (1 - ndotm*ndotm) / (a * a * ndotm * ndotm);
    
    return 1 / (part1 * part2);
}





// Simulate random walk
vec3 Material::Smith(vec3 incoming, vec3 outgoing) {
    float inf = std::numeric_limits<float>::infinity();
    
    float height = InvCumulativeDist(0.9f);
    
    vec3 energy = vec3(1.0f);
    vec3 direction = -incoming;
    vec3 weight;
    int r = 0;
    
    float theta;
    float ax = roughness;
    float ay = roughness;
    vec3 microNormal;
    
    vec3 sum = vec3(0.0f);
    
    while (true) {
        
        
        height = SampleHeight(direction, height);

        if (height == inf) {
            break;
        }
        
        
        microNormal = SampleNorm(-direction, ax, ay);
        SamplePhase(microNormal, -direction, direction, weight);
        energy = energy * weight;
        
        
        // Direction points opposite incoming, but Phase expects incoming
        
        sum += energy * Phase(-direction, outgoing) * SmithG(outgoing, height);
        
        r++;
    }
    
    // Divide by cosine of outgoing direction
    return sum / outgoing[2];
}

void Material::SamplePhase(vec3 microNormal, vec3 incoming, vec3& outgoing, vec3& weight) {
    
    vec3 sf = SchlickFresnel(incoming, microNormal);
    
    if (material == "conductor") {
        // reflect
        outgoing = -incoming + 2 * glm::dot(microNormal,incoming) * microNormal;
        weight = sf;
    }
    else if (material == "dielectric") {
        float u = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        
        for (int i = 0; i < 3; i++) {
            if (u < sf[i]) {
                // reflect
                outgoing = -incoming + 2 * glm::dot(microNormal,incoming) * microNormal;
            } else {
                // transmit
                
            }
        }
        weight = vec3(1.0f);
    }
    
    return;
}


vec3 Material::Phase(vec3 incoming, vec3 outgoing) {
    vec3 half_angle = glm::normalize(incoming + outgoing);
    
//    float cos_theta = glm::dot(normal, glm::normalize(incoming));
//    float theta = glm::acos(cos_theta);
    float cos_theta = incoming[2];
    float theta = acos(cos_theta);
    
    // Calculate modified distribution of normals
    float D_w = glm::dot(incoming, half_angle) * Distribution(half_angle) / ( cos_theta * (1+LambdaG(incoming)) );
    
    
    // Proceed
    vec3 reflectComp = SchlickFresnel(incoming, half_angle) * D_w / ( 4 * glm::abs(glm::dot(incoming, half_angle)) );
    
//    std::cout << material << " ";
    
    if (material == "conductor") {

        return reflectComp;
    } else if (material == "dielectric") {
        
    }
    
    return vec3(0.0f);
}

float Material::SampleHeight(vec3 direction, float height) {
    float u = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    
    float sg = SmithG(direction, height);
    
    
    if ( u > 1 - sg ) {
        return std::numeric_limits<float>::infinity();
    } else {
        return InvCumulativeDist( CumulativeDist(height) / glm::pow(1-u, 1/LambdaG(direction)) );
    }
}

float Material::SmithG(vec3 incoming, float height) {
    float cd = CumulativeDist(height);
    float lg = LambdaG(incoming);
    
    return powf( cd, lg );
}

// Cumulative Distribution of Heights (gaussian)
float Material::CumulativeDist(float height) {
    const float value = std::min(1.0f, std::max(0.0f, 0.5f*(height+1.0f)));
    return value;
    
//    float mean = 0;
//    float dev = 1;
//    return 0.5 * (1 + erf( (height - mean) / (dev*glm::sqrt(2)) ));
}

// Inverse Cumulative Distribution of Heights (gaussian)
// Also known as the Quantile function
float Material::InvCumulativeDist(float num) {
    const float h = std::max(-1.0f, std::min(1.0f, 2.0f*num-1.0f));
    return h;
    
//    float mean = 0;
//    float dev = 1;
//    return mean + dev * glm::sqrt(2) * erfinv(2*height-1);
}

float Material::LambdaG(vec3 w) {
    float cos2 = w[2] * w[2];
//    glm::pow(glm::dot(normal, w), 2);
    float tan2 = (1-cos2)/cos2;
    float a = 1 / (roughness * glm::sqrt(tan2));
    float pi = glm::pi<float>();
    
    if (distribution == "beckmann") {
        // Walter et al 2007 approximation
        if (a < 1.6) {
            return (1 - 1.259*a + 0.396*a*a) / (3.535*a + 2.181*a*a);
        } else {
            return 0;
        }
        
        // Exact implementation
        // return 0.5 * ( erf(a) - 1 + glm::exp(-a*a)/(a*glm::sqrt(pi)) );
    }
    else if (distribution == "ggx") {
        return (-1 + glm::sqrt( 1 + 1/(a*a) )) / 2;
    }
    
    return 0;
}


vec3 Material::SampleNorm(vec3 direction, float ax, float ay) {
    float slope_x;
    float slope_y;
    
    float theta = acos(direction[2]);
    float phi = atan2(direction[1], direction[0]);
    
    
    if (distribution == "beckmann") {
        SampleBeckmann(theta, slope_x, slope_y);
    }
    else if (distribution == "ggx") {
        SampleGGX(theta, slope_x, slope_y);
    }
    
    
    // rotate
    float tmp = cos(phi)*slope_x - sin(phi)*slope_y;
    slope_y = sin(phi)*slope_x + cos(phi)*slope_y;
    slope_x = tmp;
    
    slope_x = ax * slope_x;
    slope_y = ay * slope_y;
    
    float norm = glm::sqrt(slope_x*slope_x + slope_y*slope_y + 1);
    vec3 normal = vec3(-slope_x, -slope_y, 1) / norm;
    
    return normal;
}


void Material::SampleBeckmann(float theta_i, float& slope_x, float& slope_y) {
    // Random numbers
    float U1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float U2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    
    // special case (normal incidence)
    if (theta_i < 0.0001) {
        float r = glm::sqrt(-logf(U1));
        float phi = 6.28318530718 * U2;
        slope_x = r * glm::cos(phi);
        slope_y = r * glm::sin(phi);
        return;
    }
    
    // precomputations
    float sin_theta_i = glm::sin(theta_i);
    float cos_theta_i = glm::cos(theta_i);
    float tan_theta_i = sin_theta_i/cos_theta_i;
    float a = 1 / tan_theta_i;
    float erf_a = erf(a);
    float exp_a2 = glm::exp(-a*a);
    float SQRT_PI_INV = 0.56418958354;
    float Lambda = 0.5*(erf_a-1) + 0.5*SQRT_PI_INV*exp_a2/a;
    float G1 = 1 / (1 + Lambda);
    float C = 1 - G1 * erf_a;
    
    if (U1 < C) {
        
        // rescale U1
        U1 = U1 / C;
        
        float w_1 = 0.5 * SQRT_PI_INV * sin_theta_i * exp_a2;
        float w_2 = cos_theta_i * (0.5 - 0.5*erf_a);
        float p = w_1 / (w_1+w_2);
        
        if (U1 < p) {
            U1 = U1 / p;
            slope_x = -sqrt(-logf(U1*exp_a2));
        } else {
            U1 = (U1-p) / (1-p);
            slope_x = erfinv(U1 - 1.0 - U1*erf_a); // find the inverse erf function
        }
    
    } else {
        // rescale U1
        U1 = (U1-C) / (1-C);
        
        slope_x = erfinv( (-1.0+2.0*U1) * erf_a );
        float p = (-slope_x*sin_theta_i + cos_theta_i) / (2.0*cos_theta_i);
        
        if (U2 > p) {
            slope_x = -slope_x;
            U2 = (U2-p) / (1-p);
        } else {
            U2 = U2 / p;
        }
    }
    
    // sample slope Y
    slope_y = erfinv(2.0*U2-1.0);
    
}

void Material::SampleGGX(float theta_i, float& slope_x, float& slope_y) {
    // Random numbers
    float U1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float U2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

    // special case (normal incidence)
    if(theta_i < 0.0001) {
        float r = glm::sqrt(U1/(1-U1));
        float phi = 6.28318530718 * U2;
        slope_x = r * glm::cos(phi);
        slope_y = r * glm::sin(phi);
        return;
    }
    

    // precomputations
    float tan_theta_i = glm::tan(theta_i);
    float a = 1 / (tan_theta_i);
    float G1 = 2 / (1 + glm::sqrt(1.0+1.0/(a*a)));

    
    // sample slope_x
    float A = 2.0*U1/G1 - 1.0;
    float tmp = 1.0 / (A*A-1.0);
    float B = tan_theta_i;
    float D = glm::sqrt(B*B*tmp*tmp - (A*A-B*B)*tmp);
    float slope_x_1 = B*tmp - D;
    float slope_x_2 = B*tmp + D;
    slope_x = (A < 0 || slope_x_2 > 1.0/tan_theta_i) ? slope_x_1 : slope_x_2;
    
    // sample slope_y
    double S;
    if (U2 > 0.5) {
        S = 1.0;
        U2 = 2.0*(U2-0.5);
    } else {
        S = -1.0;
        U2 = 2.0*(0.5-U2);
    }
    
    float z = (U2*(U2*(U2*0.27385-0.73369)+0.46341)) / (U2*(U2*(U2*0.093073+0.309420)-1.000000)+0.597999);
    slope_y = S * z * glm::sqrt(1.0+slope_x*slope_x);
}


// Implementation modeled on Giles 2012
float Material::erfinv(float x) {
    float w;
    float p;

    w = -logf((1-x)*(1+x));

    if (w < 5) {
        w = w - 2.5;
        p = 2.81022636e-08f;
        p = 3.43273939e-07f + p*w;
        p = -3.5233877e-06f + p*w;
        p = -4.39150654e-06f + p*w;
        p = 0.00021858087f + p*w;
        p = -0.00125372503f + p*w;
        p = -0.00417768164f + p*w;
        p = 0.246640727f + p*w;
        p = 1.50140941f + p*w;
    } else {
        w = glm::sqrt(w) - 3;
        p = -0.000200214257f;
        p = 0.000100950558f + p*w;
        p = 0.00134934322f + p*w;
        p = -0.00367342844f + p*w;
        p = 0.00573950773f + p*w;
        p = -0.0076224613f + p*w;
        p = 0.00943887047f + p*w;
        p = 1.00167406f + p*w;
        p = 2.83297682f + p*w;
    }

    return p*x;
}

float Material::vec2angle(vec3 normal, vec3 w) {
    
    // Normalize?

    return acos( glm::dot(normal, w) );
}
