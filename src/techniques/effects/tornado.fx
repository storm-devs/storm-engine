technique TornadoPillar
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;
        ZWriteEnable = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        ColorArg1[0] = diffuse;
        ColorOp[0] = SelectArg1;
        AlphaArg1[0] = diffuse;
        AlphaOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique TornadoPillarParticles
{
    pass p0
    {
        Lighting = false;
        CullMode = none;
        FogEnable = false;
        ZWriteEnable = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;
        ColorOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique TornadoGroundParticles
{
    pass p0
    {
        Lighting = false;
        CullMode = none;
        FogEnable = false;
        ZWriteEnable = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;
        ColorOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique TornadoClouds
{
    pass p0
    {
        Lighting = false;
        CullMode = none;
        FogEnable = false;
        ZWriteEnable = false;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;
        ColorOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique TornadoDebris
{
    pass p0
    {
        Lighting = true;
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        ColorArg1[0] = tfactor;
        ColorArg2[0] = texture;
        ColorOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = tfactor;
        AlphaOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}