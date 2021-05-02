#include "particles_shaders.h"

technique particles
{
    pass p0
    {
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        ZWriteEnable = false;
        FogEnable = true;
        Lighting = false;
        TexCoordIndex[0] = 0;
        ColorArg1[0] = texture;
        ColorArg2[0] = current;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;
        Srcblend = srcalpha;
        Destblend = invsrcalpha;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[1] = disable;
        Alphaop[0] = modulate;
    }
}

technique particles_inv
{
    pass p0
    {
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        ZWriteEnable = false;
        FogEnable = true;
        Lighting = false;
        TexCoordIndex[0] = 0;
        ColorArg1[0] = texture | complement;
        ColorArg2[0] = current;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;
        Srcblend = srcalpha;
        Destblend = invsrcalpha;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[1] = disable;
        Alphaop[0] = modulate;
    }
}

technique particlesfire
{
    pass p0
    {
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        ZWriteEnable = false;
        FogEnable = true;
        Lighting = false;
        TexCoordIndex[0] = 0;
        ColorArg1[0] = texture;
        ColorArg2[0] = current;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;
        Srcblend = srcalpha;
        Destblend = one;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[1] = disable;
        Alphaop[0] = modulate;
    }
}

technique AdvancedParticles
{
    pass p0
    {
        FogEnable = false;
        ZWriteEnable = false;

        SrcBlend = one;
        DestBlend = invsrcalpha;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        PixelShader = PIXEL_SHADER;
        VertexShader = VERTEX_SHADER;
    }
}