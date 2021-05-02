technique DXVector
{
    pass p0
    {
        ZEnable = false;
        Lighting = false;
        CullMode = none;
        SpecularEnable = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = false;

        ColorOp[0] = selectarg1;
        ColorArg1[0] = diffuse;

        ColorOp[1] = disable;
    }
}

technique DXSphere
{
    pass p0
    {
        ZWriteEnable = false;
        Lighting = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = modulate;
        ColorArg1[0] = tfactor;
        ColorArg2[0] = diffuse;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = tfactor;
        AlphaArg2[0] = diffuse;

        ColorOp[1] = disable;
    }
}