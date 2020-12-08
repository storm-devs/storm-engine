#define SHADER                                                                                                         \
    asm { \
	vs_1_1 \
	dcl_position v0 \
	dcl_color v1 \
	dcl_color1 v2 \
	dcl_texcoord v3 \
	mul r0.xy, v3, c36 \
	add r0.w, r0.y, r0.x \
	max r0.w, -r0.w, c40.y \
	mul r2.xw, v1.zyzw, c40.x \
	min r0.w, r0.w, c40.z \
	mov a0.x, r2.x \
	mov r0.y, c[a0.x + 0].z \
	mul r1.w, r0.y, c40.w \
	mad r0.w, r0.w, c39.x, r1.w \
	mul r1.w, r0.w, v2.y \
	mul r0.xy, v3, v3 \
	add r0.w, r0.y, r0.x \
	mad r0.xy, v1, c41, c41.zwzw \
	mul r1.w, r1.w, c39.y \
	add r0.w, -r0.w, r0.y \
	mul r0.xy, r0.x, c[a0.x + 0] \
	rsq r0.w, r0.w \
	rcp r0.w, r0.w \
	add r3.w, v2.x, c40.y \
	mov r3.xy, v2 \
	mul r0.w, r0.w, r3.y \
	mad r0.w, v0.y, c38.w, r0.w \
	mul r1.xy, r0, -r3.w \
	mad r1.xy, v0.xzzw, c38.w, r1.xy \
	mul r0, r0.w, c33 \
	mad r2.xy, v3, r3.y, r1 \
	mov r1.xyz, c38 \
	mad oD0.xyz, r1, r1.w, c37 \
	mad r0, c32, r2.x, r0 \
	mov r1.zw, c39 \
	mov a0.x, r2.w \
	mad oT0.xy, r3, r1.zwzw, c[a0.x + 16] \
	mad r0, c34, r2.y, r0 \
	add oPos, r0, c35 \
	mov oD0.w, v3.z}