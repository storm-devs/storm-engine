#pragma once

#define MUL_DEGTORAD 0.01745329251994329547f  // Degrees to Radians
#define MUL_RADTODEG 57.29577951308232286465f // Radians to Degrees

#define GUIDSTEP 256

// Types for emitters
enum EmitterType
{
    POINT_EMITTER = 0,

    UNKNOWN_EMITTER = 0x7fffffff
};

// Types for particles
enum ParticleType
{
    BILLBOARD_PARTICLE = 1,
    MODEL_PARTICLE = 2,

    UNKNOWN_PARTICLE = 0x7fffffff
};

// Field types
enum FieldType
{
    FIELD_UNKNOWN = 0,
    FIELD_BOOL,
    FIELD_FLOAT,
    FIELD_GRAPH,
    FIELD_POSITION,
    FIELD_STRING,
    FIELD_UV,
    FIELD_COLOR,

    FIELD_FORCE_DWORD = 0x7fffffff
};
