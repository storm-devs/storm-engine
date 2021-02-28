#include "caustic_shaders.h"

technique caustic
{
    pass p0
    {
        FogEnable = false;
        FogTableMode = none;
        FogVertexMode = none;
        CullMode = none;
        ColorVertex = true;
        SpecularEnable = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = one;
        Lighting = false;
        LocalViewer = true;
        NormalizeNormals = false;
        ZWriteEnable = false;
        SlopeScaleDepthBias = -0.6;

        VertexShader = VERTEX_SHADER;
        PixelShader = PIXEL_SHADER;
    }
}
