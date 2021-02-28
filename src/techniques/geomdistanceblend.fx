technique geomdistanceblend
{
    pass p0
    {
        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        AlphaOp[1] = modulate;
        AlphaArg1[1] = current;
        AlphaArg2[1] = tfactor;
    }
}