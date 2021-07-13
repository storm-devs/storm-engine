
#pragma once

#include "Matrix.h"
#include "vmodule_api.h"

struct Light
{
    enum Type
    {
        t_none,
        t_amb,
        t_sun,
        t_sky,
        t_point,
        t_group,
    };

    CVECTOR color; // Source color
    CVECTOR p;     // Point source position or direction of directional source
    float att0;    // Attenuation D^0
    float att1;    // Attenuation D^1
    float att2;    // Attenuation D^2
    float range;   // Radius of action
    char *group;   // The group the source belongs to
    // Lighting options
    float cosine; // Coefficient of cosine
    float shadow; // shadow Coefficient
    float bright; // Shadow brightness
    float contr;  // Shadow contrast
    float gamma;  // shadow Gamma
    float curgm;  // Current source gamma
    float curct;  // current Contrast for the source
    //
    Type type;   // Source type
    bool isOn;   // Does the source work
    bool isMark; // To build a list
};

namespace lighter
{
struct Shadow
{
    double v;    // Calculated value
    double nrm;  // Normalization factor
    double sm;   // Smoothed value
    float cs;    // Cosine of the angle of the normal and direction to the source
    float att;   // Attenuation coefficient
    float dst;   // Distance to source
    float csatt; // Attenuation coefficient
    float shw;   // Shadow attenuation coefficient
    CVECTOR c;   // Current source color
};
} // namespace lighter

struct Vertex
{
    enum Flag
    {
        f_zero = 0, // Initialization
        f_inv = 1,  // Is the normal inverted
        f_set = 2,  // Has the normal been amended
        f_bug = 4,  // There was a mistake
    };

    CVECTOR p;               // Position
    CVECTOR n;               // Normal
    CVECTOR c;               // Colour
    CVECTOR bc;              // Anti-aliasing color
    CVECTOR mc;              // Color mask
    uint32_t alpha;          // vertex Alpha
    lighter::Shadow *shadow; // Shading values from each source
    long flags;              // Flags
    long vbid;               // Vertex buffer
    long addr;               // The relative address of the color in the buffer
    long obj;                // The object it belongs to
    long cindex;             // Index in final buffer of object colors
};

struct Triangle
{
    CVECTOR n; // Normal
    float sq;  // Area
    long i[3]; // Vertex indices
};

struct VertexBuffer
{
    long vbID;  // vertex Buffer identifier
    long start; // Buffer index in the global array
};

struct OctFndVerts
{
    Vertex *v; // Vertex
    float r2;  // Squared distance
};
