#ifndef PLANET_CONFIG_GLSL
#define PLANET_CONFIG_GLSL

//////////////////////////////////////////////////////////////////////////
// Implementation settings
//////////////////////////////////////////////////////////////////////////

// Comment to disable atmosphere rendering.
#define ENABLE_ATMOSPHERE

//////////////////////////////////////////////////////////////////////////
// Visual presets
//////////////////////////////////////////////////////////////////////////

// Camera settings //

#define CAMERA_FOV 100.0
#define LIGHT_COLOR vec3(1,1,1)
#define LIGHT_DIRECTION vec3(1,0,0)
#define AMBIENT_STRENGTH 0.001

// A more artistic preset //
#define EARTH_RADIUS 1.0
#define EARTH_POSITION vec3(0, 0, 1)
#define ATMOSPHERE_RADIUS 1.2
#define ATMOSPHERE_DENSITY 4.3
#define IN_SCATTERING_SAMPLES 16
#define OPTICAL_DEPTH_SAMPLES 16
#define RAYLEIGH_SCATTERING_STRENGTH 20.0
#define RAYLEIGH_SCATTERING_WAVELENGTHS vec3(700, 532, 440)
#define RAYLEIGH_SCATTERING_BIAS 400.0
#define MIE_SCATTERING_STRENGTH 0.0
#define MIE_HENYEY_GREENSTEIN_G 0.76

#endif // PLANET_CONFIG_GLSL