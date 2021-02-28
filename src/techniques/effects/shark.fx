technique Shark
{
    pass p0
    {
        CullMode = none;
        Lighting = true;
        FogEnable = true;
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        ColorArg1[0] = current;
        ColorArg2[0] = texture;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaOp[0] = disable;
    }
}

technique SharkTrack
{
    pass p0
    {
        DepthBias = 4;
        CullMode = none;
        Lighting = false;
        FogEnable = true;
        ZEnable = true;
        ZWriteEnable = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = one;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaArg1[0] = diffuse;
        AlphaOp[0] = SelectArg1;
        AlphaOp[1] = disable;
    }
}