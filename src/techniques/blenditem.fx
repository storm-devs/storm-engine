technique blenditemlit
{
    pass p0
    {
        CullMode = CCW;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = one;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;
        ColorOp[0] = modulate;
        AlphaArg1[0] = tfactor;
        AlphaOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique blenditem
{
    pass p0
    {
        CullMode = CW;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        AlphaArg1[0] = tfactor;
        AlphaOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}