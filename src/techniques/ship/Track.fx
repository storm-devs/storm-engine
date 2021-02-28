technique ShipTrack
{
    pass p0
    {
        ZWriteEnable = false;
        CullMode = none;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;

        SrcBlend = srcalpha;
        DestBlend = one;

        ColorOp[0] = selectarg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;

        ColorOp[1] = disable;
    }
}
