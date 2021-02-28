technique CoastFoam
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
        DestBlend = invsrcalpha;

        ColorOp[0] = selectarg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = modulate2x;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;

        ColorOp[1] = disable;
    }
}