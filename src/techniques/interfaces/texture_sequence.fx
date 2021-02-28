technique TextureSequence
{
    pass p0
    {
        CullMode = none;
        FogEnable = false;
        AlphaTestEnable = false;
        ZEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = zero;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;
        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = blendfactoralpha;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        AlphaOp[1] = SelectArg1;
        AlphaArg1[1] = texture;

        ColorOp[2] = disable;
        AlphaOp[2] = disable;
    }
}