#include "sea_shaders.h"

technique Sea2_Foam
{
    pass p0
    {
        FogTableMode = none;
        CullMode = none;
        FogVertexMode = none;
        ColorVertex = true;
        SpecularEnable = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = one;
        Lighting = false;
        NormalizeNormals = false;
        LocalViewer = true;

        VertexShader = SEA2FOAM_VERTEX_SHADER;
        PixelShader = SEA2FOAM_PIXEL_SHADER;
    }
}

technique Sea2_SunRoad
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        SrcBlend = one;
        DestBlend = one;

        AlphaBlendEnable = true;
        AlphaTestEnable = false;

        VertexShader = SEA2SUNROAD_VERTEX_SHADER;
        PixelShader = SEA2SUNROAD_PIXEL_SHADER;
    }
}

technique Sea2
{
    pass p0
    {
        FogTableMode = none;
        FogVertexMode = none;
        CullMode = none;
        ColorVertex = true;
        SpecularEnable = false;
        SrcBlend = one;
        DestBlend = srcalpha;
        Lighting = false;
        LocalViewer = false;
        NormalizeNormals = false;

        AlphaBlendEnable = true;
        AlphaTestEnable = false;

        VertexShader = SEA2_VERTEX_SHADER;
        PixelShader = SEA2_PIXEL_SHADER;
    }
}

technique Sea3
{
    pass p0
    {
        FogTableMode = none;
        FogVertexMode = none;
        CullMode = none;
        ColorVertex = true;
        SpecularEnable = false;
        SrcBlend = one;
        DestBlend = srcalpha;
        Lighting = false;
        LocalViewer = false;
        NormalizeNormals = false;

        AlphaBlendEnable = true;
        AlphaTestEnable = false;

        AddressU[1] = clamp;
        AddressV[1] = clamp;

        AddressU[3] = clamp;
        AddressV[3] = clamp;

        VertexShader = SEA3_VERTEX_SHADER;
        PixelShader = SEA3_PIXEL_SHADER;
    }
}