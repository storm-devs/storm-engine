technique ShipFlag
{
    pass p0
    {
        Lighting = true;
        CullMode = none;
        ColorVertex = false;
        AlphaTestEnable = true;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = modulate;
        ColorArg1[0] = texture;
        ColorArg2[0] = tfactor;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique GhostShip
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        ZWriteEnable = true;
        SrcBlend = srcalpha;
        DestBlend = one;
        TextureFactor = 0x2080ff80;
        ColorArg1[0] = texture;
        ColorArg2[0] = tfactor;
        ColorOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = tfactor;
        AlphaOp[0] = modulate2x;
    }
}