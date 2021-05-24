technique InterfaceBackScene_Menu
{
    pass p0
    {
        CullMode = none;
        FogEnable = false;
        AlphaTestEnable = true;
        AlphaBlendEnable = true;
        AlphaRef = 1;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
    }
}

technique InterfaceBackScene_Logo
{
    pass p0
    {
        CullMode = ccw;
        FogEnable = false;
        AlphaTestEnable = true;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
    }
}
