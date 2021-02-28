technique Sprite
{
    pass p0
    {
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = modulate;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;

        ColorOp[1] = disable;
    }
}