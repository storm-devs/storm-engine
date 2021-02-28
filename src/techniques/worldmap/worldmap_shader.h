#define SHADER                                                                                                         \
    asm { \
    vs_1_1 \
    dcl_position v0 \
    dcl_color v1 \
    dcl_texcoord v2 \
    mov  r0, v0 \
    mul  r1, r0.xxxx, c[0] \
    mad  r1, r0.yyyy, c[1], r1 \
    mad  r1, r0.zzzz, c[2], r1 \
    add  oPos, r1, c[3] \
    mov  oD0, v1.xxxw \
    mul  oD1, v1.yyyw, c[6] \
    mov  oT0, v2 \
    mov  r2, c[5] \
    mad  r2.w, v1.z, r2.z, r2.x \
    dp4  r1, r0, c[4] \
    rcp  r2.w, r2.w \
    mad  r1.w, r1.w, r2.w, c[5].w \
    mov oT1, r1.wwww}