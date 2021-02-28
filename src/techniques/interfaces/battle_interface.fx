technique battle_arrows
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
        ColorVertex = false;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique battle_icons
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
        ColorVertex = false;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique battle_alphacutcolor
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

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = diffuse;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique battle_rectangle
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
        ColorVertex = false;
        addressu[0] = clamp;
        addressv[0] = clamp;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique battle_msg
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

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique battle_tf_rectangle
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
        ColorVertex = false;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = tfactor;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique battle_texure_blend_tf
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
        ColorVertex = false;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = Modulate;
        AlphaArg1[0] = tfactor;
        AlphaArg2[0] = texture;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique battle_island
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
        ColorVertex = false;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique battle_only_tfactor
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
        ColorVertex = false;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = tfactor;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = tfactor;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique battle_only_color
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

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = diffuse;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = diffuse;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique battle_island_gettexture
{
    pass p0
    {
        CullMode = none;
        Lighting = false;
        FogEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        ZEnable = true;
        ColorVertex = true;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique battle_location_gettexture
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        ZEnable = true;
    }
}

technique battle_textureXtfactor
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

        ColorOp[0] = modulate;

        ColorArg1[0] = texture;
        ColorArg2[0] = tfactor;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique battle_colorRectangle
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

technique battle_tex_col_Rectangle
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

        ColorOp[0] = modulate2x;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;

        ColorOp[1] = disable;
    }
}

technique battle_tex2_col_Rectangle
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

        ColorOp[0] = modulate2x;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;

        ColorOp[1] = selectarg1;
        ColorArg1[1] = current;

        AlphaOp[1] = modulate;
        AlphaArg1[1] = texture;
        AlphaArg2[1] = current;

        ColorOp[2] = disable;
    }
}

technique battle_minimap
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

        ColorOp[0] = modulate2x;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;

        addressu[0] = clamp;
        addressv[0] = clamp;

        ColorOp[1] = selectarg1;
        ColorArg1[1] = current;

        AlphaOp[1] = modulate;
        AlphaArg1[1] = texture;
        AlphaArg2[1] = current;

        addressu[1] = clamp;
        addressv[1] = clamp;

        ColorOp[2] = disable;
    }
}

technique battle_shadow_Rectangle
{
    pass p0
    {
        CullMode = none;
        Lighting = false;
        FogEnable = false;
        SrcBlend = zero;
        DestBlend = srccolor;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        ZEnable = false;
        ColorVertex = true;

        ColorOp[0] = modulate2x;
        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;

        ColorOp[1] = disable;
    }
}

technique spyglass_interface
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
        ColorVertex = false;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = tfactor;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}

technique battle_shippointer
{
    pass p0
    {
        CullMode = none;
        Lighting = false;
        FogEnable = false;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        AlphaTestEnable = true;
        AlphaBlendEnable = true;
        ZEnable = true;
        ColorVertex = false;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;

        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        ColorOp[1] = disable;
        AlphaOp[1] = disable;
    }
}
