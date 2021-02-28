technique ShipVant
{
    pass p0
    {
        CullMode = none;
        Lighting = false;
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
        ColorOp[2] = disable;
        AlphaOp[2] = disable;
    }
}

technique ShipVant_alpha
{
    pass p0
    {
        CullMode = none;
        Lighting = false;
        ColorVertex = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        ZWriteEnable = false;

        ColorOp[0] = modulate;
        ColorArg1[0] = texture;
        ColorArg2[0] = tfactor;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = tfactor;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
        ColorOp[2] = disable;
        AlphaOp[2] = disable;
    }
}
