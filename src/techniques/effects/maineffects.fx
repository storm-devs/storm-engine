technique Fader
{
    pass p0
    {
        Lighting = false;
        ZEnable = false;
        FogEnable = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaArg1[0] = diffuse;
        AlphaOp[0] = SelectArg1;
        AlphaOp[1] = disable;
    }
}

technique FaderWithTips
{
    pass p0
    {
        Lighting = false;
        ZEnable = false;
        FogEnable = false;
        CullMode = none;
        TexCoordIndex[1] = 0;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;
        ColorOp[0] = modulate;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        ColorOp[1] = blendtexturealpha;
        ColorOp[2] = disable;
        AlphaArg1[0] = diffuse;
        AlphaOp[0] = SelectArg1;
        AlphaArg1[1] = current;
        AlphaOp[1] = SelectArg1;
        AlphaOp[2] = disable;
    }
}

technique Coronas
{
    pass p0
    {
        ZWriteEnable = false;
        ZEnable = false;
        FogEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = one;
        DestBlend = one;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        AlphaOp[1] = disable;
    }
}

technique LocFly
{
    pass p0
    {
        Lighting = false;
        ZWriteEnable = false;
        FogEnable = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        ColorArg1[0] = diffuse;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        AlphaOp[1] = disable;
    }
}

technique WideScreen
{
    pass p0
    {
        Lighting = false;
        ZEnable = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        texturefactor = 0;
        ColorArg1[0] = tfactor;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaOp[0] = disable;
    }
}

technique Blot
{
    pass p0
    {
        SlopeScaleDepthBias = -0.6;
        Lighting = false;
        ZEnable = true;
        ZWriteEnable = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        ColorArg2[0] = texture;
        ColorOp[0] = SelectArg2;
        ColorOp[1] = disable;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        AlphaArg1[1] = current;
        AlphaArg2[1] = tfactor;
        AlphaOp[1] = modulate;
        AlphaOp[2] = disable;
    }
}

technique ProgressTech
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;
        ZEnable = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaArg1[0] = texture;
        AlphaOp[0] = SelectArg1;
        AlphaOp[1] = disable;
    }
}

technique ProgressBackTech
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;
        ZEnable = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaOp[0] = disable;
    }
}

technique ProgressBackTechWithTips
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;
        ZEnable = false;
        CullMode = none;
        TexCoordIndex[1] = 0;
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        ColorOp[1] = blendtexturealpha;
        ColorOp[2] = disable;
        AlphaOp[0] = disable;
    }
}

technique Blood
{
    pass p0
    {
        SlopeScaleDepthBias = -0.6;
        Lighting = false;
        ZEnable = true;
        ZWriteEnable = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = modulate;
        ColorArg1[0] = tfactor;
        ColorArg2[0] = texture;
        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}