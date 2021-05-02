technique InterfaceFont
{
    pass p0
    {
        CullMode = none;
        FogEnable = false;
        AlphaTestEnable = false;
        ZEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = Modulate;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;

        AlphaOp[0] = Modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;

        ColorOp[1] = disable;
    }
}

technique iStartTechnique
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = modulate;
        ColorArg1[0] = current;
        ColorArg2[0] = texture;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = current;
        AlphaArg2[0] = texture;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
        ColorOp[2] = disable;
        AlphaOp[2] = disable;
    }
}

technique iMouseCurShow
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = selectarg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = selectarg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
    }
}

technique iBounder
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = modulate2x;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = diffuse;
        AlphaArg2[0] = texture;

        ColorOp[1] = disable;
    }
}

technique iButton
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = modulate2x;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = diffuse;
        AlphaArg2[0] = texture;

        ColorOp[1] = disable;
    }
}

technique iDisabledNode
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = modulate2x;
        ColorArg1[0] = texture;
        ColorArg2[0] = tfactor;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = tfactor;

        ColorOp[1] = disable;
    }
}

technique iFourImages_border
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
    }
}

technique iFourImages_main
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = modulate2x;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique iImageCollection
{
    pass p0
    {
        CullMode = none;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorVertex = false;

        ColorOp[0] = modulate2x;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;

        ColorOp[1] = disable;
    }
}

technique iLineCollection
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = diffuse;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = diffuse;

        ColorOp[1] = disable;
    }
}

technique iRectangle
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = diffuse;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = diffuse;

        ColorOp[1] = disable;
    }
}

technique iTFRectangle
{
    pass p0
    {
        CullMode = none;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorVertex = false;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = tfactor;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = tfactor;

        ColorOp[1] = disable;
    }
}

technique iTextButton
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = modulate2x;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = diffuse;
        AlphaArg2[0] = texture;

        ColorOp[1] = disable;
    }
}

technique iTitle
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = modulate2x;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = diffuse;
        AlphaArg2[0] = texture;

        ColorOp[1] = disable;
    }
}

technique iVideo
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = modulate2x;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = diffuse;
        AlphaArg2[0] = texture;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
        ColorOp[2] = disable;
        AlphaOp[2] = disable;
    }
}

technique iScrollImages_border
{
    pass p0
    {
        CullMode = none;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorVertex = false;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
    }
}

technique iScrollImages_main
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = modulate2x;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;

        ColorOp[1] = disable;
    }
}

technique iDinamicPictures
{
    pass p0
    {
        CullMode = none;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorVertex = false;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;
        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
    }
}

technique iStatusLine
{
    pass p0
    {
        CullMode = none;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorVertex = false;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;
        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
    }
}

technique iChanger
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
    }
}

technique iGlow
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;

        DestBlend = one;

        ColorOp[0] = Modulate2x;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;

        AlphaOp[0] = Modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;

        ColorOp[1] = disable;
    }
}

technique iGlowCursor
{
    pass p0
    {
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = one;
        CullMode = none;
        ColorVertex = true;

        ColorOp[0] = Modulate2x;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;

        AlphaOp[0] = Modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;

        ColorOp[1] = disable;
    }
}

technique iIcon
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = Modulate2x;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;

        AlphaOp[0] = Modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;

        ColorOp[1] = disable;
    }
}

technique iShadow
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = diffuse;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = diffuse;
        AlphaArg2[0] = texture;

        ColorOp[1] = disable;
    }
}

technique iSpec1
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = tfactor;

        ColorOp[1] = disable;
    }
}

technique iExitTechnique
{
    pass p0
    {
        ZWriteEnable = true;
        CullMode = ccw;
        ZEnable = true;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = true;

        ColorOp[0] = modulate2x;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;
        ColorOp[1] = disable;
        ColorArg1[1] = current;
        ColorArg2[1] = texture;

        ColorArg1[2] = current;
        ColorArg2[2] = diffuse;

        AlphaOp[0] = Modulate;
        AlphaArg1[0] = diffuse;
        AlphaArg2[0] = texture;

        ColorVertex = true;
    }
}

technique iBlindPictures
{
    pass p0
    {
        CullMode = none;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorVertex = false;

        ColorOp[0] = modulate2x;
        ColorArg1[0] = texture;
        ColorArg2[0] = tfactor;
        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = tfactor;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique iHelpChooser
{
    pass p0
    {
        CullMode = none;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorVertex = false;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;
        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
    }
}

technique iRotate
{
    pass p0
    {
        CullMode = none;
        ColorVertex = true;
        FogEnable = false;
        ZEnable = false;
        Lighting = false;
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = modulate2x;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = diffuse;
        AlphaArg2[0] = texture;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
        ColorOp[2] = disable;
        AlphaOp[2] = disable;

        addressu[0] = clamp;
        addressv[0] = clamp;
    }
}

technique iInfoShower
{
    pass p0
    {
        CullMode = none;
        Lighting = false;
        FogEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        ZEnable = false;
        ColorVertex = true;

        ColorOp[0] = selectarg1;
        ColorArg1[0] = diffuse;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = diffuse;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique iInfoShowerPic
{
    pass p0
    {
        CullMode = none;
        Lighting = false;
        FogEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        ZEnable = false;
        ColorVertex = true;

        ColorOp[0] = selectarg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique iInfoShowerPicWithTips
{
    pass p0
    {
        Lighting = false;
        ZEnable = false;
        FogEnable = false;
        CullMode = none;
        texcoordindex[1] = 0;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        ColorOp[1] = blendtexturealpha;
        ColorOp[2] = disable;
        AlphaOp[0] = disable;
    }
}