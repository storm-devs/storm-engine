technique Line
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