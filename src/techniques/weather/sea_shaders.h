#define SEA2FOAM_VERTEX_SHADER                                                                                         \
    asm { \
    vs_1_1 \
    dcl_position v0 \
    dcl_normal v1 \
    dcl_texcoord v2 \
    def c45, 5.0f, 0.5f, 0.04f, 1.0f \
    m4x4 r0, v0, c[24] \
    mov oPos, r0 \
    mul r11.z, r0.z, c[1].w \
    expp r11.z, r11.z \
    rcp r11.z, r11.z \
    mov oFog, r11.z \
    mov r4, v1 \
    dp3 r4.w, r4, r4 \
    rsq r4.w, r4.w \
    mul r4, r4, r4.w \
    sub r1, v0, c[23] \
    dp3 r1.w, r1, r1 \
    rsq r1.w, r1.w \
    mul r1, r1, r1.w \
    dp3 r3.x, r1, r4 \
    add r3.x, r3.x, r3.x \
    mad r2.xyz, r3.xxx, r4, -r1 \
    mul r10, r4.yzxw, c[28 + 5].zxyw \
    mad r10, - c[28 + 5].yzxw, r4.zxyw, r10 \
    mul r11, r4.yzxw, r10.zxyw \
    mad r11, - r10.yzxw, r4.zxyw, r11 \
    dp3 r10.w, r10, r10 \
    rsq r10.w, r10.w \
    mul r10, r10, r10.w \
    dp3 r11.w, r11, r11 \
    rsq r11.w, r11.w \
    mul r11, r11, r11.w \
    mul r9, r10.zxyw, r11.yzxw \
    mad r9, r10.yzxw, r11.zxyw, -r9 \
    dp3 oT0.x, r10, c[28 + 8] \
    dp3 oT0.y, r11, c[28 + 8] \
    dp3 oT0.z, r9, c[28 + 8] \
    dp3 oT1.x, r10, c[28 + 9] \
    dp3 oT1.y, r11, c[28 + 9] \
    dp3 oT1.z, r9, c[28 + 9] \
    dp3 oT2.x, r10, c[28 + 10] \
    dp3 oT2.y, r11, c[28 + 10] \
    dp3 oT2.z, r9, c[28 + 10] \
    mul oT3.xy, v2, c[3].z \
    sub r0.z, v0.y, c[3].x \
    mul r0.z, r0.z, c[3].y \
    max oT3.z, r0.z, c[0].x \
    mov oT4.z, c[2].x \
    mov oT4.xy, v2}

#define SEA2FOAM_PIXEL_SHADER                                                                                          \
    asm { \
    ps_1_4 \
    texcrd r0.rgb, t0 \
    texcrd r1.rgb, t1 \
    texcrd r2.rgb, t2 \
    texcrd r3.rgb, t3 \
    texld r4, t4 \
    dp3    r5.r, r0, r4_bx2 \
    dp3    r5.g, r1, r4_bx2 \
    dp3    r5.b, r2, r4_bx2 \
    mul r5.rgb, r5, c0.rrr \
    mov r1.rgb, r3 \
    phase \
    texld r0, r1 \
    mov r0.a, r3.b}

#define SEA2_VERTEX_SHADER                                                                                             \
    asm { \
    vs_1_1 \
    dcl_position v0 \
    dcl_normal v1 \
    dcl_texcoord v2 \
    m4x4 r0, v0, c[24] \
    mov oPos, r0 \
    mul r11.z, r0.z, c[1].w \
    expp r11.z, r11.z \
    rcp r11.z, r11.z \
    mov oFog, r11.z \
    mov r4, v1 \
    dp3 r4.w, r4, r4 \
    rsq r4.w, r4.w \
    mul r4, r4, r4.w \
    sub r1, v0, c[23] \
    dp3 r1.w, r1, r1 \
    rsq r1.w, r1.w \
    mul r1, r1, r1.w \
    dp3 r7 .x, r1, r4 \
    mov r5.x, -r7 .x \
    mov r5.yz, c[28 + 6].xxy \
    mul r10.xy, r5.xy, r5.xy \
    mad r10.z, r10.y, r10.x, -r10.y \
    add r10.z, r10.z, c[0].y \
    rsq r9.x, r10.z \
    mul r9.x, r10.z, r9.x \
    mad r10.y, r5.y, r9.x, r5.x \
    mad r10.x, - r5.y, r9.x, r5.x \
    rcp r9.y, r10.y \
    mad r10.w, r5.y, r5.x, r9.x \
    mad r10.z, r5.y, r5.x, -r9.x \
    rcp r9.z, r10.w \
    mul r9.yz, r9.xyz, r10.xxz \
    mul r9.xw, r9.yyyz, r9.yyyz \
    add r7 .y, r9.x, r9.w \
    mul r7 .y, r7 .y, c[0].z \
    min r7 .y, r7 .y, r5.z \
    rcp r2.x, r1.y \
    mul r2.x, r2.x, c[28].x \
    min r2.x, r2.x, -r2.x \
    expp r7 .z, r2.x \
    mul r2.y, c[28].y, r7 .y \
    dp3 r3.x, c[28 + 30], r4 \
    mul r3.y, r3.x, r2.y \
    mul oD0.xyz, r3.yyy, c[28 + 2] \
    mad r2.x, -r7 .y, r7 .z, r7 .z \
    mul oD0.w, r2.x, c[28].z \
    sub r2.xy, c[0].yy, r7 .yz \
    mul r2.z, r2.x, r2.y \
    mul oD1.xyz, r2.zzz, c[28 + 1] \
    mov oD1.w, c[0].y \
    dp3 r3.x, r1, r4 \
    add r3.x, r3.x, r3.x \
    mad r2.xyz, r3.xxx, r4, -r1 \
    mul r10, r4.yzxw, c[28 + 5].zxyw \
    mad r10, - c[28 + 5].yzxw, r4.zxyw, r10 \
    mul r11, r4.yzxw, r10.zxyw \
    mad r11, - r10.yzxw, r4.zxyw, r11 \
    dp3 r10.w, r10, r10 \
    rsq r10.w, r10.w \
    mul r10, r10, r10.w \
    dp3 r11.w, r11, r11 \
    rsq r11.w, r11.w \
    mul r11, r11, r11.w \
    mul r9, r10.zxyw, r11.yzxw \
    mad r9, r10.yzxw, r11.zxyw, -r9 \
    dp3 oT1.x, r10, c[28 + 8] \
    dp3 oT1.y, r11, c[28 + 8] \
    dp3 oT1.z, r9, c[28 + 8] \
    dp3 oT2.x, r10, c[28 + 9] \
    dp3 oT2.y, r11, c[28 + 9] \
    dp3 oT2.z, r9, c[28 + 9] \
    dp3 oT3.x, r10, c[28 + 10] \
    dp3 oT3.y, r11, c[28 + 10] \
    dp3 oT3.z, r9, c[28 + 10] \
    mov oT1.w, -r1.x \
    mov oT2.w, -r1.y \
    mov oT3.w, -r1.z \
    mov oT0.z, c[2].x \
    mov oT0.xy, v2}

#define SEA2_PIXEL_SHADER                                                                                              \
    asm { \
    ps_1_1 \
    tex t0 \
    texm3x3pad t1, t0_bx2 \
    texm3x3pad t2, t0_bx2 \
    texm3x3vspec t3, t0_bx2 \
    mad r0.xyz, v0, t3, v1 \
    mov r0.w, v0.w}

#define SEA2SUNROAD_VERTEX_SHADER                                                                                      \
    asm { \
    vs_1_1 \
    dcl_position v0 \
    dcl_normal v1 \
    dcl_texcoord v2 \
    m4x4 r0, v0, c[24] \
    mov oPos, r0 \
    mul r11.z, r0.z, c[1].w \
    expp r11.z, r11.z \
    rcp r11.z, r11.z \
    mov oFog, r11.z \
    mov r4, v1 \
    dp3 r4.w, r4, r4 \
    rsq r4.w, r4.w \
    mul r4, r4, r4.w \
    sub r1, v0, c[23] \
    dp3 r1.w, r1, r1 \
    rsq r1.w, r1.w \
    mul r1, r1, r1.w \
    dp3 r3.x, r1, r4 \
    add r3.x, r3.x, r3.x \
    mad r2.xyz, r3.xxx, r4, -r1 \
    mul r10, r4.yzxw, c[28 + 5].zxyw \
    mad r10, - c[28 + 5].yzxw, r4.zxyw, r10 \
    mul r11, r4.yzxw, r10.zxyw \
    mad r11, - r10.yzxw, r4.zxyw, r11 \
    dp3 r10.w, r10, r10 \
    rsq r10.w, r10.w \
    mul r10, r10, r10.w \
    dp3 r11.w, r11, r11 \
    rsq r11.w, r11.w \
    mul r11, r11, r11.w \
    mul r9, r10.zxyw, r11.yzxw \
    mad r9, r10.yzxw, r11.zxyw, -r9 \
    dp3 oT1.x, r10, c[28 + 8] \
    dp3 oT1.y, r11, c[28 + 8] \
    dp3 oT1.z, r9, c[28 + 8] \
    dp3 oT2.x, r10, c[28 + 9] \
    dp3 oT2.y, r11, c[28 + 9] \
    dp3 oT2.z, r9, c[28 + 9] \
    dp3 oT3.x, r10, c[28 + 10] \
    dp3 oT3.y, r11, c[28 + 10] \
    dp3 oT3.z, r9, c[28 + 10] \
    mov oT1.w, -r1.x \
    mov oT2.w, -r1.y \
    mov oT3.w, -r1.z \
    mov oT0.z, c[2].x \
    mov oT0.xy, v2}

#define SEA2SUNROAD_PIXEL_SHADER                                                                                       \
    asm { \
    ps_1_1 \
    tex t0 \
    texm3x3pad t1, t0_bx2 \
    texm3x3pad t2, t0_bx2 \
    texm3x3vspec t3, t0_bx2 \
    mov r0, t3}

#define SEA3_VERTEX_SHADER                                                                                             \
    asm { \
    vs_1_1 \
    dcl_position v0 \
    dcl_normal v1 \
    dcl_texcoord v2 \
    m4x4 r0, v0, c[24] \
    mov oPos, r0 \
    mul r11.z, r0.z, c[1].w \
    expp r11.z, r11.z \
    rcp r11.z, r11.z \
    mov oFog, r11.z \
    mov r4, v1 \
    dp3 r4.w, r4, r4 \
    rsq r4.w, r4.w \
    mul r4, r4, r4.w \
    sub r1, v0, c[23] \
    dp3 r1.w, r1, r1 \
    rsq r1.w, r1.w \
    mul r1, r1, r1.w \
    dp3 r7 .x, r1, r4 \
    mov r5.x, -r7 .x \
    mov r5.yz, c[28 + 6].xxy \
    mul r10.xy, r5.xy, r5.xy \
    mad r10.z, r10.y, r10.x, -r10.y \
    add r10.z, r10.z, c[0].y \
    rsq r9.x, r10.z \
    mul r9.x, r10.z, r9.x \
    mad r10.y, r5.y, r9.x, r5.x \
    mad r10.x, - r5.y, r9.x, r5.x \
    rcp r9.y, r10.y \
    mad r10.w, r5.y, r5.x, r9.x \
    mad r10.z, r5.y, r5.x, -r9.x \
    rcp r9.z, r10.w \
    mul r9.yz, r9.xyz, r10.xxz \
    mul r9.xw, r9.yyyz, r9.yyyz \
    add r7 .y, r9.x, r9.w \
    mul r7 .y, r7 .y, c[0].z \
    min r7 .y, r7 .y, r5.z \
    rcp r2.x, r1.y \
    mul r2.x, r2.x, c[28].x \
    min r2.x, r2.x, -r2.x \
    expp r7 .z, r2.x \
    mul r2.y, c[28].y, r7 .y \
    dp3 r3.x, c[28 + 30], r4 \
    mul r3.y, r3.x, r2.y \
    mul oD0.xyz, r3.yyy, c[28 + 2] \
    mad r2.x, -r7 .y, r7 .z, r7 .z \
    mul oD0.w, r2.x, c[28].z \
    sub r2.xy, c[0].yy, r7 .yz \
    mul r2.z, r2.x, r2.y \
    mul oD1.xyz, r2.zzz, c[28 + 1] \
    mov oD1.w, c[0].y \
    mov r1, v0 \
    m4x4 r0, r1, c[24] \
    rcp r1.w, r0.w \
    mul r0.xyz, r0.xyz, r1.w \
    mul r0.xyz, r0.xyz, c[0].zzz \
    mul r0.y, r0.y, c[1].y \
    add r0.xy, r0.xy, c[0].zz \
    add r0.xy, r0.xy, c[0].ww \
    mov oT3, r0 \
    add r0.xy, r0.xy, v1.xz \
    mov oT1, r0 \
    mov oT0.z, c[2].x \
    mov oT0.xy, v2 \
    mov oT2.z, c[2].x \
    mov oT2.xy, v2}

#define SEA3_PIXEL_SHADER                                                                                              \
    asm { \
    ps_1_1 \
    tex t0 \
    texbem t1, t0 \
    tex t2 \
    texbem t3, t2 \
    mad r0.xyz, v0, t1, v1 \
    add r0.xyz, r0, t3 \
    mov r0.w, v0.w}