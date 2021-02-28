technique EditorBack
{
    pass p0
    {
        CullMode = none;
        Lighting = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        ZEnable = false;
        ColorArg1[0] = diffuse;
        ColorOp[0] = selectarg1;
        ColorOp[1] = disable;
        AlphaArg1[0] = diffuse;
        AlphaOp[0] = selectarg1;
    }
}

technique EditorFront
{
    pass p0
    {
        CullMode = none;
        Lighting = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        ZEnable = true;
        ColorArg1[0] = diffuse;
        ColorOp[0] = selectarg1;
        AlphaArg1[0] = diffuse;
        AlphaOp[0] = selectarg1;
        ColorOp[1] = disable;
    }
}