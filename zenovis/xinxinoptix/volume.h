#pragma once

#include <optix.h>
#include <cuda/random.h>
#include <cuda/helpers.h>
#include <sutil/vec_math.h>
#include "zxxglslvec.h"
#include <math_constants.h>

#ifdef __CUDACC_RTC__ 
    #define _CPU_GPU_ __device__ __host__
#else
    #define _CPU_GPU_ /* Nothing */
#endif

#define _FLT_EPL_ 1.19209290e-7F

#define _DELTA_TRACKING_ true

namespace pbrt {

__device__
inline void CoordinateSystem(const float3& a, float3& b, float3& c) {
    
//    if (abs(a.x) > abs(a.y))
//        b = float3{-a.z, 0, a.x} /
//              sqrt(max(_FLT_EPL_, a.x * a.x + a.z * a.z));
//    else
//        b = float3{0, a.z, -a.y} /
//              sqrt(max(_FLT_EPL_, a.y * a.y + a.z * a.z));
    
    if (abs(a.x) > abs(a.y))
        b = float3{-a.z, 0, a.x};
    else
        b = float3{0, a.z, -a.y};
    
    b = normalize(b);
    c = cross(a, b);
}

inline float3 SphericalDirection(float sinTheta, float cosTheta, float phi) {
    return float3{sinTheta * cosf(phi), sinTheta * sinf(phi), cosTheta};
}

inline float3 SphericalDirection(float sinTheta, float cosTheta, float phi,
                                 const float3 &x, const float3 &y, const float3 &z) {
    return sinTheta * cos(phi) * x + sinTheta * sin(phi) * y + cosTheta * z;
}

struct HenyeyGreenstein {
    float g;
    __device__ HenyeyGreenstein(float g) : g(g) {}
    
    float p(const float3 &wo, const float3 &wi) const;
    float Sample_p(const float3 &wo, float3 &wi, const float2 &uu) const;
};

// Media Inline Functions
inline float PhaseHG(float cosTheta, float g) {
    float gg = g * g;
    float denom = 1 + gg + 2 * g * cosTheta;
    return (0.25 / M_PIf) * (1 - gg) / (denom * sqrt(denom));
}

// HenyeyGreenstein Method Definitions
inline float HenyeyGreenstein::p(const float3 &wo, const float3 &wi) const {
    return PhaseHG(dot(wo, wi), g);
}

inline float HenyeyGreenstein::Sample_p(const float3 &wo, float3 &wi, const float2 &uu) const {
    // Compute $\cos \theta$ for Henyey--Greenstein sample
    float cosTheta;
    if (abs(g) < 1e-3)
        cosTheta = 1 - 2 * uu.x;
    else {
        float gg = g * g;
        float sqrTerm = (1 - gg) / (1 + g - 2 * g * uu.x);
        cosTheta = -(1 + gg - sqrTerm * sqrTerm) / (2 * g);
    }

    // Compute direction _wi_ for Henyey--Greenstein sample
    float sinTheta = sqrt(fmax(0.0f, 1.0f - cosTheta * cosTheta));
    float phi = 2 * M_PIf * uu.y;
    
    float3 v1, v2;
    CoordinateSystem(wo, v1, v2);
    wi = SphericalDirection(sinTheta, cosTheta, phi, v1, v2, wo);

    return PhaseHG(cosTheta, g);
}

// Spectrum Function Declarations
__device__ inline float Blackbody(float lambda, float T) {
    if (T <= 0)
        return 0;
    const float c = 299792458.f;
    const float h = 6.62606957e-34f;
    const float kb = 1.3806488e-23f;
    // Return emitted radiance for blackbody at wavelength _lambda_ 
    float l = lambda * 1e-9f; 
    float Le = (2 * h * c * c) / (powf(l, 5) * (expf((h * c) / (l * kb * T)) - 1));
    //CHECK(!IsNaN(Le));
    return Le;
}

class BlackbodySpectrum {
  public:
    // BlackbodySpectrum Public Methods
    __device__ 
    BlackbodySpectrum(float T) : T(T) {
        // Compute blackbody normalization constant for given temperature
        float lambdaMax = 2.8977721e-3f / T;
        normalizationFactor = 1 / Blackbody(lambdaMax * 1e9f, T);
    }

    float operator()(float lambda) const {
        return Blackbody(lambda, T) * normalizationFactor;
    }

    const static int NSpectrumSamples = 3;

    float3 Sample(const float3 &lambda) const {
        float3 s;

        s.x = Blackbody(lambda.x, T) * normalizationFactor;
        s.y = Blackbody(lambda.y, T) * normalizationFactor;
        s.z = Blackbody(lambda.z, T) * normalizationFactor;

        return s;
    }

    float MaxValue() const { return 1.f; }
    //std::string ToString() const;

  private:
    // BlackbodySpectrum Private Members
    float T;
    float normalizationFactor;
};

} // namespace pbrt

__device__ float3 kelvinRGB(float kelvin) //https://tannerhelland.com/2012/09/18/convert-temperature-rgb-algorithm-code.html
{
    float3 result;

    kelvin = clamp(kelvin, 1000.f, 40000.f) / 100.f;

    if (kelvin <= 66) {
        result.x = 1.0;
        result.y = 0.39008157876901960784 * log(kelvin) - 0.63184144378862745098;
    } else {
        float t = kelvin - 60.0;
		result.x = 1.29293618606274509804 * powf(t, -0.1332047592);
		result.y = 1.12989086089529411765 * powf(t, -0.0755148492);
    }

    if (kelvin >= 66)
        result.z = 1.0;
    else if (kelvin <= 19)
        result.z = 0.0;
    else
        result.z = 0.54320678911019607843 * log(kelvin - 10.0) - 1.19625408914;

    return clamp(result, 0.0, 1.0);;
}

__device__ vec3 fakeBlackBody(float temp) //https://www.shadertoy.com/view/MlcGD7
{
    const static float white_temp = 6500;

    const static float min_temp = 1000;
    const static float max_temp = 15000;

    temp = clamp(temp, min_temp, max_temp);

    if (temp < white_temp) {
        float ratio = temp / white_temp;
        return vec3(1,1./4.,1./16.) * exp(4.* ratio - 1.);
    } else {
        float ratio = (temp - white_temp) / (max_temp - white_temp);
        return pow(vec3(.1, .7, .8), vec3(4 * ratio));
    }
}
