#pragma once

// INTEL COMMENT:
// This function simply gathers 4 floats and places them on a __m128 variable.
static inline void SSE_GatherFourFloats(float *pf0, float *pf1, float *pf2, float *pf3, __m128 *pm128Result)
{
    __m128 xmm0 = _mm_load_ss(pf0); // 0 0 0 pf0
    __m128 xmm1 = _mm_load_ss(pf1); // 0 0 0 pf1
    __m128 xmm2 = _mm_load_ss(pf2); // 0 0 0 pf2
    __m128 xmm3 = _mm_load_ss(pf3); // 0 0 0 pf3

    xmm0 = _mm_movelh_ps(xmm0, xmm2);                           // 0 pf2 0 pf0
    xmm1 = _mm_shuffle_ps(xmm1, xmm3, _MM_SHUFFLE(0, 1, 0, 1)); // pf3 0 pf1 0

    *pm128Result = _mm_or_ps(xmm0, xmm1); // pf3 pf2 pf1 pf0
}

// INTEL COMMENT:
// This function simply scatters the contents of a __m128 variable on to 4 floats.
static inline void SSE_ScatterFourFloats(float *pf0, float *pf1, float *pf2, float *pf3, __m128 m128Value)
{
    __m128 xmm1 = _mm_shuffle_ps(m128Value, m128Value, _MM_SHUFFLE(3, 2, 1, 1)); // 3 2 1 1
    __m128 xmm2 = _mm_movehl_ps(_mm_setzero_ps(), m128Value);                    // 0 0 3 2
    __m128 xmm3 = _mm_shuffle_ps(m128Value, m128Value, _MM_SHUFFLE(3, 2, 1, 3)); // 3 2 1 3

    _mm_store_ss(pf0, m128Value); // 3 2 1 0
    _mm_store_ss(pf1, xmm1);      // 3 2 1 1
    _mm_store_ss(pf2, xmm2);      // 0 0 3 2
    _mm_store_ss(pf3, xmm3);      // 3 2 1 3
}

// INTEL COMMENT:
// This function performs the maths of a normalise using SSE instructions - 4 normalises in 9 instructions - very fast.
static inline void SSE_Normalise(__m128 m128X, __m128 m128Y, __m128 m128Z, __m128 *pm128X, __m128 *pm128Y,
                                 __m128 *pm128Z)
{
    __m128 xmm0 = _mm_mul_ps(m128X, m128X); // X * X
    __m128 xmm1 = _mm_mul_ps(m128Y, m128Y); // Y * Y
    __m128 xmm2 = _mm_mul_ps(m128Z, m128Z); // Z * Z

    xmm0 = _mm_add_ps(xmm0, xmm1); // X * X + Y * Y
    xmm1 = _mm_add_ps(xmm0, xmm2); // X * X + Y * Y + Z * Z

    xmm0 = _mm_rsqrt_ps(xmm1); // 1 / sqrt( X * X + Y * Y + Z * Z )

    *pm128X = _mm_mul_ps(m128X, xmm0); // RecipLength * X
    *pm128Y = _mm_mul_ps(m128Y, xmm0); // RecipLength * Y
    *pm128Z = _mm_mul_ps(m128Z, xmm0); // RecipLength * Z
}

static inline void GatherFourFloats(float *pf0, float *pf1, float *pf2, float *pf3, __m128 *pm128Result)
{
    __m128 xmm0 = _mm_load_ss(pf0); // 0 0 0 pf0
    __m128 xmm1 = _mm_load_ss(pf1); // 0 0 0 pf1
    __m128 xmm2 = _mm_load_ss(pf2); // 0 0 0 pf2
    __m128 xmm3 = _mm_load_ss(pf3); // 0 0 0 pf3

    xmm0 = _mm_movelh_ps(xmm0, xmm2);                           // 0 pf2 0 pf0
    xmm1 = _mm_shuffle_ps(xmm1, xmm3, _MM_SHUFFLE(0, 1, 0, 1)); // pf3 0 pf1 0

    *pm128Result = _mm_or_ps(xmm0, xmm1); // pf3 pf2 pf1 pf0
}

static inline void ScatterFourFloats(float *pf0, float *pf1, float *pf2, float *pf3, __m128 m128Value)
{
    __m128 xmm1 = _mm_shuffle_ps(m128Value, m128Value, _MM_SHUFFLE(3, 2, 1, 1)); // 3 2 1 1
    __m128 xmm2 = _mm_movehl_ps(_mm_setzero_ps(), m128Value);                    // 0 0 3 2
    __m128 xmm3 = _mm_shuffle_ps(m128Value, m128Value, _MM_SHUFFLE(3, 2, 1, 3)); // 3 2 1 3

    _mm_store_ss(pf0, m128Value); // 3 2 1 0
    _mm_store_ss(pf1, xmm1);      // 3 2 1 1
    _mm_store_ss(pf2, xmm2);      // 0 0 3 2
    _mm_store_ss(pf3, xmm3);      // 3 2 1 3
}

static inline void Normalise(__m128 m128X, __m128 m128Y, __m128 m128Z, __m128 *pm128X, __m128 *pm128Y, __m128 *pm128Z)
{
    __m128 xmm0 = _mm_mul_ps(m128X, m128X); // X * X
    __m128 xmm1 = _mm_mul_ps(m128Y, m128Y); // Y * Y
    __m128 xmm2 = _mm_mul_ps(m128Z, m128Z); // Z * Z

    xmm0 = _mm_add_ps(xmm0, xmm1); // X * X + Y * Y
    xmm1 = _mm_add_ps(xmm0, xmm2); // X * X + Y * Y + Z * Z

    xmm0 = _mm_rsqrt_ps(xmm1); // 1 / sqrt( X * X + Y * Y + Z * Z )

    *pm128X = _mm_mul_ps(m128X, xmm0); // RecipLength * X
    *pm128Y = _mm_mul_ps(m128Y, xmm0); // RecipLength * Y
    *pm128Z = _mm_mul_ps(m128Z, xmm0); // RecipLength * Z
}
