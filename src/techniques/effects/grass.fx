struct VS_OUTPUT
{
    float4 pos : POSITION;
    float4 clr : COLOR;
    float2 uv : TEXCOORD0;
};

// View*Projection
uniform extern float4x4 gVP;
// Tables
uniform extern float3 aAngles[16];
uniform extern float2 aUV[16];
// Lighting
uniform extern float2 lDir;
uniform extern float kLitWF;
uniform extern float3 aColor;
uniform extern float3 lColor;
// Position
uniform extern float fDataScale;
uniform extern float2 aSize;

VS_OUTPUT main(float3 pos : POSITION, float4 params : COLOR0, float4 offset : COLOR1, float3 wa : TEXCOORD)
{
    VS_OUTPUT Output;

    // Orientation
    float3 ang = aAngles[params.z * 15.0f];

    // Lighting
    float kClr = mul(wa.xy, lDir);
    kClr = clamp(kClr, -0.5f, 1.0f);
    kClr = (kClr * 0.9f + ang.z * 0.8f) * kLitWF * offset.y;
    Output.clr.xyz = aColor + lColor * kClr;
    Output.clr.w = wa.z;

    // Position
    float2 size = params.xy * float2(0.3f * aSize.x, 0.4f * aSize.y) + float2(0.7f * aSize.x, 0.6f * aSize.y);
    float4 tmpPos;
    pos = pos * fDataScale;
    tmpPos.xz = pos.xz - ang.xy * size.xx * (offset.xx - 0.5f) + wa.xy * offset.yy;
    tmpPos.y = pos.y + sqrt(size.y - (wa.x * wa.x + wa.y * wa.y)) * offset.y;
    tmpPos.w = 1.0f;
    Output.pos = mul(tmpPos, gVP);

    // Tex coords
    Output.uv = aUV[params.w * 15.0f].xy + offset.xy * float2(0.245f, -0.245f);

    return Output;
}

technique Grass
{
    pass p0
    {
        Lighting = false;
        AlphaRef = 80;
        CullMode = none;

        AlphaTestEnable = true;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;
        ColorOp[0] = modulate2x;
        ColorOp[1] = disable;
        AlphaArg1[0] = diffuse;
        AlphaArg2[0] = texture;
        AlphaOp[0] = modulate;
        AlphaOp[1] = disable;

        VertexShader = compile vs_1_0 main();
    }
}

technique GrassDark
{
    pass p0
    {
        Lighting = false;
        AlphaRef = 80;
        CullMode = none;

        AlphaTestEnable = true;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaArg1[0] = diffuse;
        AlphaArg2[0] = texture;
        AlphaOp[0] = modulate;
        AlphaOp[1] = disable;

        VertexShader = compile vs_1_0 main();
    }
}
