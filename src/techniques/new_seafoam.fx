technique new_seafoam
{
    pass p0
    {
        CullMode = CW;
        ColorVertex = true;
        AlphaTestEnable = true;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        AlphaRef = 0x0;
        ZWriteEnable = false;

        ColorOp[0] = selectarg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = current;

        ColorOp[1] = disable;
    }
}
