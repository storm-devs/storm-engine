technique DbgPatchViewZ
{
    pass p0
    {
        DepthBias = 3;
        Lighting = false;
        FogEnable = false;
        ZEnable = true;
        ZWriteEnable = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;
        ColorOp[0] = selectArg1;
        ColorOp[1] = disable;
        AlphaOp[0] = disable;
    }
}

technique DbgPatchViewNoZ
{
    pass p0
    {
        DepthBias = 3;
        Lighting = false;
        ZEnable = false;
        ZWriteEnable = false;
        AlphaTestEnable = true;
        AlphaBlendEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;
        ColorOp[0] = selectArg1;
        AlphaArg1[0] = diffuse;
        AlphaOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique DbgDrawLocators
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
        DestBlend = invsrcalpha;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = tfactor;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaArg1[0] = diffuse;
        AlphaArg2[0] = tfactor;
        AlphaOp[0] = modulate;
        AlphaOp[1] = disable;
    }
}

technique DbgLocationDrawLine
{
    pass p0
    {
        DepthBias = 3;
        Lighting = false;
        FogEnable = false;
        ZWriteEnable = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        ColorArg1[0] = diffuse;
        ColorOp[0] = selectArg1;
        ColorOp[1] = disable;
        AlphaArg1[0] = diffuse;
        AlphaOp[0] = selectArg1;
        AlphaOp[1] = disable;
    }
}

technique DbgDrawLines
{
    pass p0
    {
        DepthBias = 3;
        Lighting = false;
        FogEnable = false;
        ZEnable = true;
        ZWriteEnable = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        ColorArg1[0] = tfactor;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaOp[0] = disable;
    }
}

technique DbgDraw2DTFColor
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;
        CullMode = none;
        ZEnable = false;
        ZWriteEnable = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        ColorArg1[0] = tfactor;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaArg1[0] = tfactor;
        AlphaOp[0] = SelectArg1;
        AlphaOp[1] = disable;
    }
}

technique DbgDraw2DTexture
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;
        CullMode = none;
        ZEnable = false;
        ZWriteEnable = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaArg1[0] = texture;
        AlphaOp[0] = SelectArg1;
        AlphaOp[1] = disable;
    }
}