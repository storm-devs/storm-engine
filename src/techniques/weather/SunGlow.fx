technique sunglow_noz
{
    pass p0
    {
        ZEnable = false;
        FogEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = one;
        DestBlend = one;

        ColorOp[0] = modulate;
        ColorArg1[0] = texture;
        ColorArg2[0] = current;

        AlphaOp[0] = disable;

        ColorOp[1] = disable;
    }
}

technique sunglow_z
{
    pass p0
    {
        ZEnable = true;
        FogEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = one;
        DestBlend = one;

        ColorOp[0] = modulate;
        ColorArg1[0] = texture;
        ColorArg2[0] = current;

        AlphaOp[0] = disable;

        ColorOp[1] = disable;
    }
}

technique moon_z
{
    pass p0
    {
        ZEnable = true;
        FogEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = one;
        DestBlend = one;

        ColorOp[0] = modulate;
        ColorArg1[0] = texture;
        ColorArg2[0] = current;

        AlphaOp[0] = selectarg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
    }
}

technique SunRoad_Glow
{
    pass p0
    {
        ZEnable = false;
        ZWriteEnable = false;
        FogEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = one;
        DestBlend = one;

        ColorOp[0] = selectarg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = disable;

        ColorOp[1] = disable;
    }
}

technique sunoverflow
{
    pass p0
    {
        ZEnable = false;
        FogEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = one;
        DestBlend = one;

        ColorOp[0] = modulate;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;

        AlphaOp[0] = disable;

        ColorOp[1] = disable;
    }
}

technique sunreflection
{
    pass p0
    {
        ZEnable = false;
        FogEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = one;

        ColorOp[0] = modulate;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;

        AlphaOp[0] = selectarg1;
        AlphaArg1[0] = diffuse;

        ColorOp[1] = disable;
    }
}