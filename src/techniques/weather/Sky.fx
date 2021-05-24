technique sky
{
    pass p0
    {
        ZEnable = false;
        FogEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = false;

        AddressU[0] = clamp;
        AddressV[0] = clamp;

        ColorOp[0] = modulate;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;

        AlphaOp[0] = disable;

        ColorOp[1] = disable;
    }
}

technique skyblend
{
    pass p0
    {
        ZEnable = false;
        FogEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = false;

        AddressU[0] = clamp;
        AddressV[0] = clamp;
        AddressU[1] = clamp;
        AddressV[1] = clamp;

        ColorOp[0] = modulate;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;

        AlphaOp[0] = disable;

        ColorOp[1] = blendfactoralpha;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;

        AlphaOp[1] = disable;

        ColorOp[2] = disable;
        AlphaOp[2] = disable;
    }
}

technique skyblend_alpha
{
    pass p0
    {
        ZEnable = false;
        FogEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        DestBlend = srcalpha;
        SrcBlend = invsrcalpha;

        AddressU[0] = clamp;
        AddressV[0] = clamp;
        AddressU[1] = clamp;
        AddressV[1] = clamp;

        ColorOp[0] = modulate;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;

        AlphaOp[0] = selectarg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = blendfactoralpha;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;

        AlphaOp[1] = blendfactoralpha;
        AlphaArg1[1] = texture;
        AlphaArg2[1] = current;

        ColorOp[2] = disable;
        AlphaOp[2] = disable;
    }
}

technique skyfog
{
    pass p0
    {
        ZEnable = false;
        FogEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaRef = 0x0;
        AlphaTestEnable = true;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = selectarg1;
        ColorArg1[0] = diffuse;

        AlphaOp[0] = selectarg1;
        AlphaArg1[0] = diffuse;

        ColorOp[1] = disable;
    }
}

technique skyland
{
    pass p0
    {
        ZEnable = true;
        FogEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = false;

        AddressU[0] = clamp;
        AddressV[0] = clamp;

        ColorOp[0] = modulate;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;

        AlphaOp[0] = disable;

        ColorOp[1] = disable;
    }
}

technique skyblendland
{
    pass p0
    {
        ZEnable = true;
        FogEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = false;

        AddressU[0] = clamp;
        AddressV[0] = clamp;
        AddressU[1] = clamp;
        AddressV[1] = clamp;

        ColorOp[0] = modulate;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;

        AlphaOp[0] = disable;

        ColorOp[1] = blendfactoralpha;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;

        AlphaOp[1] = disable;

        ColorOp[2] = disable;
        AlphaOp[2] = disable;
    }
}

technique skyblend_alphaland
{
    pass p0
    {
        ZEnable = true;
        FogEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        DestBlend = srcalpha;
        SrcBlend = invsrcalpha;

        AddressU[0] = clamp;
        AddressV[0] = clamp;
        AddressU[1] = clamp;
        AddressV[1] = clamp;

        ColorOp[0] = modulate;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;

        AlphaOp[0] = selectarg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = blendfactoralpha;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;

        AlphaOp[1] = blendfactoralpha;
        AlphaArg1[1] = texture;
        AlphaArg2[1] = current;

        ColorOp[2] = disable;
        AlphaOp[2] = disable;
    }
}

technique skyfogland
{
    pass p0
    {
        ZEnable = true;
        FogEnable = false;
        Lighting = false;
        CullMode = none;
        AlphaRef = 0x0;
        AlphaTestEnable = true;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = selectarg1;
        ColorArg1[0] = diffuse;

        AlphaOp[0] = selectarg1;
        AlphaArg1[0] = diffuse;

        ColorOp[1] = disable;
    }
}