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

}

Material::Material(string m, string d, string s, vec3 e, float r, vec3 dc, vec3 f0) {
    
    method = m;
    distribution = d;
    shadowmask = s;
    emissive = e;
    roughness = r;
    diffuseColor = dc;
    F0 = f0;
    
}

void Material::set(string m, string d, string s, vec3 e, float r, vec3 dc, vec3 f0) {
    
    method = m;
    distribution = d;
    shadowmask = s;
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

vec3 Material::BRDF(vec3 normal, vec3 incoming, vec3 outgoing) {
    if (method == "lambert") {
        return Lambert(normal, incoming, outgoing);
    }
    if (method == "cooktorrance") {
        return CookTorrance(normal, incoming, outgoing);
    }
    if (method == "smith") {
        return Smith(normal, incoming, outgoing);
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
    
    float denominator = glm::pi<float>() * a * a * glm::pow(ndotm,4);
    
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


// Calculate Distribution of Normals
float Distribution(vec3 normal, vec3 half_angle) {
    if (distribution == "beckmann") {
        return BeckmannD(normal, half_angle);
    }
}


// Simulate random walk
vec3 Material::Smith(vec3 normal, vec3 incoming, vec3 outgoing) {
    float inf = std::numeric_limits<float>::infinity();
    float height = inf;
    float energy = 1;
    vec3 direction = -incoming;
    float weight;
    int r = 0;
    
    float sum = 0;
    
    while (true) {
        height = SampleHeight(normal, direction, height);
        if (height == inf) {
            break;
        }
        direction, weight = SampleDir();
        energy = energy * weight;
        
        sum = sum + energy * Phase(normal, direction, outgoing) * SmithG(normal, outgoing, height);
        
        r++;
    }
    return sum;
}

vec3, float Material::SampleDir(vec3 normal, vec3 direction, float height) {
    if ("conductor") {
        
    }
}

vec3 Material::SampleNorm() {
    if ("conductor") {
        
    }
}

float Material::Phase(vec3 normal, vec3 incoming, vec3 outgoing) {
    vec3 half_angle = glm::normalize(incoming + outgoing);
    vec3 microNormal = SampleNorm();
    
    // Calculate modified distribution of normals
    float D_w = glm::dot(incoming, microNormal) * Distribution(microNormal, half_angle,       ) /
    ( glm::dot(normal, glm::normalize(incoming)) * (1+LambdaG(normal, incoming)) );
    
    
    // Proceed
    float reflectComp = SchlickFresnel(incoming, half_angle) * D_w / ( 4 * glm::abs(glm::dot(incoming, half_angle)) );
    
    if ("conductor") {
        return reflectComp;
    }
}

float Material::SampleHeight(vec3 normal, vec3 direction, float height) {
    float u = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    
    if (u >= 1 - SmithG(normal, direction, height) ) {
        return std::numeric_limits<float>::infinity();
    } else {
        return
    }
}

float Material::SmithG(vec3 normal, vec3 incoming, float height) {
    return glm::pow( CumulativeDist(height), LambdaG(normal, incoming) );
}

float CumulativeDist() {
    if (distribution == "beckmann") {
        
    }
    else if (distribution == "trowbridge") {
        
    }
    
    return
}

float Material::LambdaG(vec3 normal, vec3 w) {
    float cos = glm::pow(glm::dot(normal, w), 2);
    float tan2 = (1-cos2)/cos2;
    
    if (distribution == "beckmann") {
        float a = 1 / (roughness * glm::sqrt(tan2));
        return 0.5 * ( erf(a) - 1 + glm::exp(-glm::pow(a,2)) / (a*glm::sqrt(glm::pi<float>())) );
    }
    else if (distribution == "trowbridge") {
        return (-1 + glm::sqrt(1 + glm::pow(roughness,2) * tan2)) / 2;
    }
    
    return
}




vec3 Material::SampleBeckmann(vec3 incoming, float& slope_x, float& slope_y) {
    // special case (normal incidence)
    if (theta_i < 0.0001) {
        float r = sqrt(-log(U1));
        float phi = 6.28318530718 * U2;
        slope_x = r * cos(phi);
        slope_y = r * sin(phi);
        return;
    }
    
    // precomputations
    float sin_theta_i = sin(theta_i);
    float cos_theta_i = cos(theta_i);
    float tan_theta_i = sin_theta_i/cos_theta_i;
    float a = 1 / tan_theta_i;
    float erf_a = erf(a);
    float exp_a2 = exp(-a*a);
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
            slope_x = -sqrt(-log(U1*exp_a2));
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

vec3 Material::SampleGGX(vec3 incoming, float& slope_x, float& slope_y) {
    

    // special case (normal incidence)
    if(theta_i < 0.0001) {
        float r = sqrt(U1/(1-U1));
        float phi = 6.28318530718 * U2;
        slope_x = r * cos(phi);
        slope_y = r * sin(phi);
        return;
    }
    

    // precomputations
    float tan_theta_i = tan(theta_i);
    float a = 1 / (tan_theta_i);
    float G1 = 2 / (1 + sqrt(1.0+1.0/(a*a)));
    
    // sample slope_x
    float A = 2.0*U1/G1 - 1.0;
    float tmp = 1.0 / (A*A-1.0);
    float B = tan_theta_i;
    float D = sqrt(B*B*tmp*tmp - (A*A-B*B)*tmp);
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
    slope_y = S * z * sqrt(1.0+slope_x*slope_x);
}
