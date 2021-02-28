technique rain
{
    pass p0
    {
        ZWriteEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = one;

        ColorOp[0] = modulate;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = tfactor;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = diffuse;
        AlphaArg2[0] = tfactor;

        ColorOp[1] = disable;
    }
}

technique rain_drops
{
    pass p0
    {
        ZWriteEnable = false;
        FogEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = one;

        ColorOp[0] = modulate;
        ColorArg1[0] = texture;
        ColorArg2[0] = current;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = current;

        ColorOp[1] = disable;
    }
}

technique sea_rain_drops
{
    pass p0
    {
        ZWriteEnable = false;
        FogEnable = false;
        Lighting = false;
        CullMode = none;
        SlopeScaleDepthBias = -0.6;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = one;

        ColorOp[0] = modulate;
        ColorArg1[0] = texture;
        ColorArg2[0] = current;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = current;

        ColorOp[1] = disable;
    }
}