technique ShipCannonAngles
{
    pass p0
    {
        ZEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        ColorOp[0] = selectarg1;
        ColorArg1[0] = diffuse;

        AlphaOp[0] = selectarg1;
        AlphaArg1[0] = diffuse;

        ColorOp[1] = disable;
    }
}
