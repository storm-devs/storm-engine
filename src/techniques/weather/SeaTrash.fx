technique seatrash
{
    pass p0
    {
        ZEnable = true;
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
        AlphaArg2[0] = diffuse;

        ColorOp[1] = disable;
    }
}