technique Animation
{
    pass p0
    {
        Lighting = true;

        ColorOp[0] = modulate2x;
        ColorOp[1] = disable;

        AlphaOp[0] = disable;

        AlphaTestEnable = false;
        AlphaBlendEnable = false;
    }
}

technique AnimationBlend
{
    pass p0
    {
        Lighting = true;

        ColorOp[0] = modulate2x;
        ColorOp[1] = disable;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = tfactor;
        AlphaOp[1] = disable;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
    }
}

technique AnimationSelected
{
    pass p0
    {
        Lighting = true;

        ColorOp[0] = modulate2x;

        ColorOp[1] = add;
        ColorArg1[1] = tfactor;
        ColorArg2[1] = current;

        ColorOp[2] = multiplyadd;
        ColorArg0[2] = current;
        ColorArg1[2] = tfactor;
        ColorArg2[2] = current;

        ColorOp[3] = multiplyadd;
        ColorArg0[3] = current;
        ColorArg1[3] = tfactor;
        ColorArg2[3] = current;

        ColorOp[4] = disable;

        AlphaOp[0] = disable;

        AlphaTestEnable = false;
        AlphaBlendEnable = false;
    }
}

technique AnimationSelectedBlend
{
    pass p0
    {
        Lighting = true;

        ColorOp[0] = modulate2x;

        ColorOp[1] = add;
        ColorArg1[1] = tfactor;
        ColorArg2[1] = current;

        ColorOp[2] = multiplyadd;
        ColorArg0[2] = current;
        ColorArg1[2] = tfactor;
        ColorArg2[2] = current;

        ColorOp[3] = multiplyadd;
        ColorArg0[3] = current;
        ColorArg1[3] = tfactor;
        ColorArg2[3] = current;

        ColorOp[4] = disable;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = tfactor;
        AlphaOp[1] = disable;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
    }
}