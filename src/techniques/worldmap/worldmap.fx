#include "worldmap_shader.h"

technique WdmModelDrawStd
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaOp[0] = disable;

        AlphaTestEnable = false;
        AlphaBlendEnable = false;
    }
}

technique WdmModelDrawStdAlphaTest
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaArg1[0] = texture;
        AlphaOp[0] = SelectArg1;
        AlphaOp[1] = disable;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
    }
}

technique WdmModelDrawStdA
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaArg1[0] = tfactor;
        AlphaOp[0] = SelectArg1;
        AlphaOp[1] = disable;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
    }
}

technique WdmCounterDrawSky
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        TexCoordIndex[0] = 0;
        TexCoordIndex[1] = 0;
        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        ColorOp[1] = blendfactoralpha;
        ColorOp[2] = disable;
        AlphaOp[0] = selectarg1;
        AlphaArg1[0] = tfactor;
        AlphaOp[1] = disable;

        AlphaTestEnable = false;
        AlphaBlendEnable = false;
    }
}

technique WdmCounterDrawNumber
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        ColorOp[1] = addsigned;
        ColorOp[2] = disable;
        AlphaOp[0] = disable;

        AlphaTestEnable = false;
        AlphaBlendEnable = false;
    }
}

technique WdmClouds
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZWriteEnable = false;

        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;
        ColorOp[0] = modulate;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        ColorOp[1] = modulate;
        ColorArg1[2] = specular;
        ColorArg2[2] = current;
        ColorOp[2] = add;
        ColorOp[3] = disable;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        AlphaOp[1] = disable;

        AddressU[1] = clamp;
        AddressV[1] = clamp;

        VertexShader = SHADER;
    }
}

technique WdmLightning
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZWriteEnable = false;

        SrcBlend = srcalpha;
        DestBlend = one;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaArg1[0] = diffuse;
        AlphaOp[0] = SelectArg1;
        AlphaOp[1] = disable;
    }
}

technique WdmRain
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZWriteEnable = false;

        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        AlphaOp[1] = disable;
    }
}

technique WdmWarringShipClouds
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZWriteEnable = false;

        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        AlphaOp[1] = disable;
    }
}

technique WdmShipWaterTrack
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZWriteEnable = false;

        SrcBlend = one;
        DestBlend = one;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaOp[0] = disable;
    }
}

technique WdmDebugDraw
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZEnable = false;
        ZWriteEnable = false;

        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;
        ColorOp[0] = SelectArg2;
        ColorOp[1] = disable;
        AlphaOp[0] = disable;
    }
}

technique WdmDrawLabelIcon
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZEnable = false;
        ZWriteEnable = false;

        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;
        ColorOp[0] = selectarg2;
        AlphaArg1[0] = diffuse;
        AlphaArg2[0] = texture;
        AlphaOp[0] = selectarg2;

        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        ColorOp[1] = blendfactoralpha;
        AlphaArg1[1] = texture;
        AlphaArg2[1] = current;
        AlphaOp[1] = blendfactoralpha;

        ColorOp[2] = disable;
        AlphaArg1[2] = diffuse;
        AlphaArg2[2] = current;
        AlphaOp[2] = modulate;

        AlphaOp[3] = disable;
    }
}

technique WdmDrawMapBlend
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZEnable = false;
        ZWriteEnable = false;

        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        AlphaOp[1] = disable;
    }
}

technique WdmSeaDraw1
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        TextureTransformFlags[0] = count2;
        AddressU[0] = clamp;
        AddressV[0] = clamp;

        ZEnable = false;
        ZWriteEnable = false;

        AlphaTestEnable = false;
        AlphaBlendEnable = false;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaOp[0] = disable;
    }
}

technique WdmSeaDraw2
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        TextureTransformFlags[0] = count2;
        AddressU[0] = clamp;
        AddressV[0] = clamp;

        ZEnable = false;
        ZWriteEnable = false;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorArg1[0] = texture;
        ColorArg2[0] = tfactor;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaArg2[0] = tfactor;
        AlphaOp[0] = SelectArg2;
        AlphaOp[1] = disable;
    }
}

technique WdmSeaDraw3
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        TextureTransformFlags[0] = count2;
        TextureTransformFlags[1] = count2;
        TexCoordIndex[0] = 0;
        TexCoordIndex[1] = 0;

        ZEnable = false;
        ZWriteEnable = false;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = one;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorArg0[1] = tfactor;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        ColorOp[1] = lerp;
        ColorOp[2] = disable;
        AlphaArg2[0] = tfactor;
        AlphaOp[0] = SelectArg2;
        AlphaOp[1] = disable;
    }
}

technique WdmSeaDraw4
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        TextureTransformFlags[0] = count2;
        TextureTransformFlags[1] = count2;
        TexCoordIndex[0] = 0;
        TexCoordIndex[1] = 0;

        ZEnable = false;
        ZWriteEnable = false;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = one;
        DestBlend = one;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        ColorOp[1] = blendfactoralpha;
        ColorArg1[2] = current;
        ColorArg2[2] = tfactor;
        ColorOp[2] = subtract;
        ColorArg1[3] = current;
        ColorArg2[3] = current;
        ColorOp[3] = modulate4x;
        ColorOp[4] = disable;

        AlphaArg2[0] = tfactor;
        AlphaOp[0] = SelectArg2;
        AlphaArg2[1] = tfactor;
        AlphaOp[1] = SelectArg2;
        AlphaArg2[2] = tfactor;
        AlphaOp[2] = SelectArg2;
        AlphaOp[3] = disable;
    }
}

technique WdmSeaDrawFlare
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZEnable = true;
        ZWriteEnable = false;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = one;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;

        AlphaArg1[0] = texture;
        AlphaArg2[0] = tfactor;
        AlphaOp[0] = modulate;
        AlphaOp[1] = disable;
    }
}

technique WdmInterfaceDrawBlend
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;

        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        AlphaOp[1] = disable;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
    }
}

technique WdmInterfaceDrawSky
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZEnable = false;
        ZWriteEnable = false;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorArg1[1] = current;
        ColorOp[1] = SelectArg1;
        ColorOp[2] = disable;

        AlphaArg1[0] = texture;
        AlphaOp[0] = SelectArg1;
        AlphaArg1[1] = texture;
        AlphaArg2[1] = current;
        AlphaOp[1] = modulate;
        AlphaOp[2] = disable;
    }
}

technique WdmIslandWaves
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;
        ZWriteEnable = false;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        TextureTransformFlags[0] = count2;
        AddressV[0] = clamp;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;
        ColorOp[1] = disable;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = tfactor;
        AlphaArg2[0] = texture;
        AlphaOp[1] = disable;
    }
}