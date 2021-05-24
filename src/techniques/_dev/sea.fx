technique SeaLocatorsShow
{
    pass p0
    {
        ZWriteEnable = false;
        TextureFactor = 0xFFFFFFFF;
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