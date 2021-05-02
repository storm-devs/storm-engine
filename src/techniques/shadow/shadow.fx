technique shadow_model
{
    pass p0
    {
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        FogEnable = false;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = TFactor;

        ColorOp[1] = disable;
    }
}

technique shadow_draw
{
    pass p0
    {
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = zero;
        DestBlend = invsrccolor;
        FogEnable = false;
        Lighting = false;
        ZWriteEnable = false;
        CullMode = ccw;
        SlopeScaleDepthBias = -0.6;

        ColorOp[0] = modulate2x;
        ColorArg1[0] = texture;
        ColorArg2[0] = tfactor;

        ColorOp[1] = disable;
    }
}

technique shadow_smooth
{
    pass p0
    {
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = one;
        DestBlend = one;
        FogEnable = false;
        Lighting = false;
        ZEnable = false;
        CullMode = none;

        ColorOp[0] = modulate;
        ColorArg1[0] = texture;
        ColorArg2[0] = tfactor;

        ColorOp[1] = modulate;
        ColorArg1[1] = current;
        ColorArg2[1] = diffuse;

        ColorOp[2] = disable;
    }
}
