#include "postprocess_shader.h"

technique PostProcess
{
    pass p0
    {
        ZEnable = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        FogEnable = false;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = Texture;

        ColorOp[1] = disable;
    }
}

technique PostProcessGlow
{
    pass p0
    {
        ZEnable = false;
        AlphaTestEnable = false;
        FogEnable = false;

        ColorOp[0] = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = TFactor;

        ColorOp[1] = disable;

        AlphaBlendEnable = true;
        SrcBlend = one;
        DestBlend = one;
    }
}

technique PostProcessBlur
{
    pass p0
    {
        addressu[0] = clamp;
        addressv[0] = clamp;
        addressu[1] = clamp;
        addressv[1] = clamp;
        addressu[2] = clamp;
        addressv[2] = clamp;
        addressu[3] = clamp;
        addressv[3] = clamp;

        ZEnable = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        FogEnable = false;

        PixelShader = SHADER;
    }
}