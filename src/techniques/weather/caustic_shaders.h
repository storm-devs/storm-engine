#define VERTEX_SHADER                                                                                                  \
    asm { \
    vs_1_1 \
    dcl_position v0 \
    dcl_texcoord0 v1 \
    dcl_color v2 \
    dcl_texcoord1 v3 \
    m4x4 r3, v0, c[0] \
    mov oPos, r3 \
    m4x4 r0, v0, c[4] \
    mov r1, r0 \
    m3x3 r2, v1, c[4] \
    dp3 r2.x, c[14], r2 \
    mul r2.y, r3.z, c15.x \
    min r2.y, r2.y, c15.y \
    sub r2.y, c15.y, r2.y \
    mul r2.x, r2.x, r2.y \
    mov oT0.xy, v3 \
    mul oT1.xy, r0.xz, c[10].xx \
    mul oT2.xy, r0.xz, c[10].xx \
    mov oT3.xyzw, c[10].yyyy \
    mov oT3.w, r2.x \
    mov oD0, c[11] \
    max r3.z, c[13].x, r3.z \
    mul r3.z, r3.z, c[12].x \
    expp r3.z, r3.z \
    rcp oFog, r3.z}

#define PIXEL_SHADER                                                                                                   \
    asm { \
        ps_1_1 \
        def c0, 0.5, 0.5, 0.5, 0.0 \
        def c1, 1.0, 1.0, 1.0, 0.0 \
        def c2, 0.0, 0.0, 0.0, 0.0 \
        tex t0 \
        tex t1 \
        tex t2 \
        texcoord  t3 \
        mov r0, t3 \
        lrp r0, r0, t2, t1 \
        mul r0, r0, v0 \
        sub r0.rgb, r0, c0 \
        mul r0.rgb, r0, c1 \
        mul r0.a, t0.a, t3.a}