#pragma once

// Unique identifiers for the particle editor
enum EditorUniqueTypes
{
    ET_POINT_EMITTER = 0,

    ET_BILLBOARD_PARTICLE = 50,
    ET_MODEL_PARTICLE = 51,

    ET_FIELD_BOOL = 250,
    ET_FIELD_FLOAT = 251,
    ET_FIELD_GRAPH = 252,
    ET_FIELD_POSITION = 253,
    ET_FIELD_STRING = 254,
    ET_FIELD_UV = 255,
    ET_FIELD_COLOR = 256,

    ET_UNKNOWN = 999999,

    ET_FORCE_DWORD = 0x7fffffff
};
