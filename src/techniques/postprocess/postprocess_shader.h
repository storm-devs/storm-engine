#define SHADER                                                                                                         \
    asm { \
    ps_1_1 \
    def c1, 0.25, 0.25, 0.25, 0.25 \
    def c2, 1.0, 1.0, 1.0, 1.0 \
    tex t0 \
    tex t1 \
    tex t2 \
    tex t3 \
    mul r0, t0, c1 \
    mad r0, t1, c1, r0 \
    mad r0, t2, c1, r0 \
    mad r0, t3, c1, r0}