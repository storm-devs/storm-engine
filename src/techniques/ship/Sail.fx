technique ShipSail
{
    pass p0
    {
        Lighting = true;
        CullMode = none;
        DiffuseMaterialSource = material;
        AmbientMaterialSource = color1;
        AlphaTestEnable = true;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorOp[0] = modulate;
        ColorArg1[0] = current;
        ColorArg2[0] = texture;
        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        addressu[1] = border;
        addressv[1] = border;
        ColorOp[1] = blendtexturealpha;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        AlphaOp[1] = SelectArg1;
        AlphaArg1[1] = current;

        ColorOp[2] = SelectArg1;
        ColorArg1[2] = current;
        AlphaOp[2] = modulate;
        AlphaArg1[2] = current;
        AlphaArg2[2] = texture;

        ColorOp[3] = modulate;
        ColorArg1[3] = tfactor;
        ColorArg2[3] = current;
        AlphaOp[3] = SelectArg1;
        AlphaArg1[3] = current;
    }
}

technique ShipSail_alpha
{
    pass p0
    {
        Lighting = true;
        CullMode = none;
        DiffuseMaterialSource = material;
        AmbientMaterialSource = color1;
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        ZWriteEnable = false;

        ColorOp[0] = modulate;
        ColorArg1[0] = current;
        ColorArg2[0] = texture;
        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = texture;

        addressu[1] = border;
        addressv[1] = border;
        ColorOp[1] = blendtexturealpha;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        AlphaOp[1] = SelectArg1;
        AlphaArg1[1] = current;

        ColorOp[2] = SelectArg1;
        ColorArg1[2] = current;
        AlphaOp[2] = modulate;
        AlphaArg1[2] = current;
        AlphaArg2[2] = texture;

        ColorOp[3] = modulate;
        ColorArg1[3] = tfactor;
        ColorArg2[3] = current;
        AlphaOp[3] = modulate;
        AlphaArg1[3] = tfactor;
        AlphaArg2[3] = current;
    }
}