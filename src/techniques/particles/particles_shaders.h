#define VERTEX_SHADER                                                                                                  \
    asm { \
    vs_1_1 \
    dcl_position v0 \
    dcl_color v1 \
    dcl_texcoord v2 \
    dcl_texcoord1 v3 \
    dcl_tangent v4 \
    dcl_blendweight v5 \
    dcl_texcoord2 v6 \
    dcl_texcoord3 v7 \
    mov oD0, v1 \
    mov oT0, v2 \
    mov oT1, v3 \
    mov oT3, v2 \
    mov oT2.xyzw, c[13].xxxx \
    mov oT2.x, v7.x \
    mov oD1.xyzw, v5.xxxx \
    mad r7.xy, v4.x, c[1].x, c[1].yzzw \
    frc r2.xy, r7.xy \
    mul r9.xy, r2, c[1].w \
    add r4.xy, r9, c[2].x \
    mul r11.xy, r4, r4 \
    mad r1.xy, r11.xy, c[2].w, c[2].y \
    mad r3.xy, r11.xy, r1, c[2].z \
    mad r5.xy, r11.xy, r3, c[0].x \
    mad r7.xy, r11.xy, r5, c[0].w \
    mad r0.xy, r11.xy, r7, c[0].y \
    mad oD1.x, -r0.y, c[13].z, c[13].z \
    mad oD1.y, -r0.x, c[13].z, c[13].z \
    mov oD1.z, c[13].z \
    mov r1, v0 \
    mul r3.x, r1.y, r0.y \
    mul r3.y, r1.x, r0.x \
    add r2.x, r3.y, r3.x \
    mul r3.x, r1.x, r0.y \
    mul r3.y, r1.y, r0.x \
    add r2.y, r3.y, -r3.x \
    mov r2.z, c[13].x \
    mov r2.w, c[13].x \
    m4x4 r3, v6, c[3] \
    add r2, r2, r3 \
    m4x4 oPos, r2, c[7]}

#define PIXEL_SHADER                                                                                                   \
    asm { \
    ps_1_1 \
    def c0, 0.6f,0.6f,0.6f,0.6f \
    tex t0 \
    tex t1 \
    texcoord t2 \
    tex t3 \
    lrp r0, v1.aaaa, t0, t1 \
    mul r1, r0, v0 \
    mul r0.rgb, r1, r1.aaaa \
    dp3 r1.rgba, t2, r1.aaaa \
    mov r0.a, r1.a \
    dp3 r1, t3_bx2, v1_bx2 \
    add r1, r1, c0 \
    mul r0.rgb, r0, r1}