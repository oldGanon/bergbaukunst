
#include <intrin.h>
#include <math.h>

typedef union
{
    f32 E[2];
    struct { f32 x, y; };
    struct { f32 u, v; };
} vec2;

typedef union
{
    f32 E[4];
    struct
    {
        f32 x;
        union
        {
            struct { f32 y, z; };
            vec2 yz;
        };
    };
    struct { vec2 xy; };    
} vec3;

typedef union
{
    f32 E[4];
    struct
    {
        f32 x;
        union
        {
            struct { f32 y, z, w; };
            vec2 yz;
        };
    };
    struct { vec2 xy; vec2 zw; };
    struct { vec3 xyz; };    
} vec4;

typedef union
{
    i32 E[2];
    struct { i32 x, y; };
    struct { i32 u, v; };
} ivec2;

typedef union
{
    i32 E[4];
    struct
    {
        i32 x;
        union
        {
            struct { i32 y, z; };
            ivec2 yz;
        };
    };
    struct { ivec2 xy; };
} ivec3;

#define MATH_PI 3.14159265358979323846f

#define ABS(A) (((A)<(0))?(-A):(A))
#define SIGN(A) (((A)<(0))?(-1):(1))
#define NEGATE(A) (-(A))
#define MIN(A,B) (((A)<(B))?(A):(B))
#define MAX(A,B) (((A)>(B))?(A):(B))
#define CLAMP(A,B,C) MIN(MAX(A,B),C)

inline i32 Log2(u64 n)
{
    unsigned long i;
    if (_BitScanReverse64(&i, n)) return i;
    else return (u32)-1;
}

/***********/
/*   F32   */
/***********/
// GENERAL FUNCTIONS
inline f32 F32_Abs(f32 A)        { return _mm_cvtss_f32(_mm_and_ps(_mm_set_ss(A), _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)))); }
inline f32 F32_Sign(f32 A)       { return _mm_cvtss_f32(_mm_or_ps(_mm_and_ps(_mm_set_ss(A), _mm_set_ss(-0.0f)), _mm_set_ss(1.0f))); }
inline f32 F32_Negate(f32 A)     { return _mm_cvtss_f32(_mm_xor_ps(_mm_set_ss(A), _mm_set_ss(-0.0f))); }
inline f32 F32_Min(f32 A, f32 B) { return _mm_cvtss_f32(_mm_min_ss(_mm_set_ss(A), _mm_set_ss(B))); }
inline f32 F32_Max(f32 A, f32 B) { return _mm_cvtss_f32(_mm_max_ss(_mm_set_ss(A), _mm_set_ss(B))); }
inline f32 F32_Clamp(f32 A, f32 min, f32 max) { return _mm_cvtss_f32(_mm_min_ss(_mm_max_ss(_mm_set_ss(A), _mm_set_ss(min)), _mm_set_ss(max))); }
inline f32 F32_Modulo(f32 N, f32 D) { __m128 mN = _mm_set_ss(N); __m128 mD = _mm_set_ss(D); return _mm_cvtss_f32(_mm_sub_ss(mN, _mm_mul_ss(mD, _mm_floor_ps(_mm_div_ss(mN, mD))))); }

// FLOATING POINT FUNCTIONS
inline f32 F32_Round(f32 A) { return _mm_cvtss_f32(_mm_round_ss(_mm_undefined_ps(), _mm_set_ss(A), _MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC)); }
inline f32 F32_Trunc(f32 A) { return _mm_cvtss_f32(_mm_round_ss(_mm_undefined_ps(), _mm_set_ss(A), _MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC)); }
inline f32 F32_Ceil(f32 A)  { return _mm_cvtss_f32(_mm_ceil_ss(_mm_undefined_ps(), _mm_set_ss(A))); }
inline f32 F32_Floor(f32 A) { return _mm_cvtss_f32(_mm_floor_ss(_mm_undefined_ps(), _mm_set_ss(A))); }
inline f32 F32_Sqrt(f32 A) { return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(A)));}
inline f32 F32_Lerp(f32 A, f32 B, f32 t) { return _mm_cvtss_f32(_mm_add_ss(_mm_mul_ss(_mm_sub_ss(_mm_set_ss(1),_mm_set_ss(t)),_mm_set_ss(A)),_mm_mul_ss(_mm_set_ss(t),_mm_set_ss(B)))); }
inline f32 F32_Fract(f32 A) { __m128 mA = _mm_set_ss(A); return _mm_cvtss_f32(_mm_sub_ss(mA, _mm_floor_ps(mA))); }
inline f32 F32_Inverse(f32 A) { return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(1), _mm_set_ss(A)));}
inline f32 F32_Saturate(f32 A)   { return _mm_cvtss_f32(_mm_min_ss(_mm_max_ss(_mm_set_ss(A), _mm_set_ss(0)), _mm_set_ss(1))); }

inline f32 F32_SmoothStep(f32 A)
{
    __m128 x = _mm_set_ss(A);
    x = _mm_mul_ss(x, _mm_mul_ss(x, _mm_sub_ss(_mm_set_ss(3), _mm_mul_ss(_mm_set_ss(2), x))));
    return _mm_cvtss_f32(_mm_min_ss(_mm_max_ss(x, _mm_set_ss(0)), _mm_set_ss(1)));
}

inline f32 F32_SmootherStep(f32 A)
{
    __m128 x = _mm_set_ss(A);
    x = _mm_mul_ss(x, _mm_mul_ss(x, _mm_mul_ss(x, _mm_add_ss(_mm_mul_ss(x, _mm_sub_ss(_mm_mul_ss(x, _mm_set_ss(6)), _mm_set_ss(15))), _mm_set_ss(10)))));
    return _mm_cvtss_f32(_mm_min_ss(_mm_max_ss(x, _mm_set_ss(0)), _mm_set_ss(1)));
}

f32 Sin(f32 x)
{
    x = x * (1.0f / MATH_PI) + 1.0f;
    x = F32_Modulo(x, 2.0f) - 1.0f;
    f32 x2 = x * x;
    f32 x3 =       0.000385937753182769f;
    x3 = x3 * x2 - 0.006860187425683514f;
    x3 = x3 * x2 + 0.0751872634325299f;
    x3 = x3 * x2 - 0.5240361513980939f;
    x3 = x3 * x2 + 2.0261194642649887f;
    x3 = x3 * x2 - 3.1415926444234477f;
    return (x - 1.0f) * (x + 1.0f) * x3 * x;
}

f32 Cos(f32 x)
{
    x = x * (1.0f / MATH_PI) + 1.5f;
    x = F32_Modulo(x, 2.0f) - 1.0f;
    f32 x2 = x * x;
    f32 x3 =       0.000385937753182769f;
    x3 = x3 * x2 - 0.006860187425683514f;
    x3 = x3 * x2 + 0.0751872634325299f;
    x3 = x3 * x2 - 0.5240361513980939f;
    x3 = x3 * x2 + 2.0261194642649887f;
    x3 = x3 * x2 - 3.1415926444234477f;
    return (x - 1.0f) * (x + 1.0f) * x3 * x;
}

/***************/
/* F32 <-> I32 */
/***************/
inline i32 F32_RoundToI32(f32 A) { return _mm_cvt_ss2si(_mm_round_ss(_mm_undefined_ps(), _mm_set_ss(A),_MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC)); }
inline i32 F32_TruncToI32(f32 A) { return _mm_cvt_ss2si(_mm_round_ss(_mm_undefined_ps(), _mm_set_ss(A), _MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC)); }
inline i32 F32_CeilToI32(f32 A)  { return _mm_cvt_ss2si(_mm_ceil_ss(_mm_undefined_ps(), _mm_set_ss(A))); }
inline i32 F32_FloorToI32(f32 A) { return _mm_cvt_ss2si(_mm_floor_ss(_mm_undefined_ps(), _mm_set_ss(A))); }
inline i32 F32_FloatToI32(f32 A) { return _mm_cvt_ss2si(_mm_set_ss(A)); }

/***********/
/*   I32   */
/***********/
inline i32 I32_Abs(i32 A) { return ABS(A); }
inline i32 I32_Sign(i32 A) { return SIGN(A); }
inline i32 I32_Negate(i32 A) { return NEGATE(A); }
inline i32 I32_Max(i32 A, i32 B) { return _mm_cvtsi128_si32(_mm_max_epi32(_mm_cvtsi32_si128(A),_mm_cvtsi32_si128(B))); }
inline i32 I32_Min(i32 A, i32 B) { return _mm_cvtsi128_si32(_mm_min_epi32(_mm_cvtsi32_si128(A),_mm_cvtsi32_si128(B))); }
inline i32 I32_Clamp(i32 A, i32 min, i32 max) { return I32_Min(I32_Max(A, min), max); }
inline i32 I32_Modulo(i32 N, i32 D) { return N % D; }

/***********/
/*   U16   */
/***********/
inline u16 U16_Abs(u16 A) { return A; }
inline u16 U16_Sign(u16 A) { return 1; }
// inline u16 U16_Negate(u16 A) { /* IMPOSSIBLE */ }
inline u16 U16_Max(u16 A, u16 B) { return _mm_cvtsi128_si32(_mm_max_epu16(_mm_cvtsi32_si128(A),_mm_cvtsi32_si128(B))) & 0xFFFF; }
inline u16 U16_Min(u16 A, u16 B) { return _mm_cvtsi128_si32(_mm_min_epu16(_mm_cvtsi32_si128(A),_mm_cvtsi32_si128(B))) & 0xFFFF; }
inline u16 U16_Clamp(u16 A, u16 min, u16 max) { return U16_Min(U16_Max(A, min), max); }
inline u16 U16_Modulo(u16 N, u16 D) { return N % D; }

/***********/
/*   U32   */
/***********/
inline u32 U32_Abs(u32 A) { return A; }
inline u32 U32_Sign(u32 A) { return 1; }
// inline u32 U32_Negate(u32 A) { /* IMPOSSIBLE */ }
inline u32 U32_Max(u32 A, u32 B) { return _mm_cvtsi128_si32(_mm_max_epu32(_mm_cvtsi32_si128(A),_mm_cvtsi32_si128(B))); }
inline u32 U32_Min(u32 A, u32 B) { return _mm_cvtsi128_si32(_mm_min_epu32(_mm_cvtsi32_si128(A),_mm_cvtsi32_si128(B))); }
inline u32 U32_Clamp(u32 A, u32 min, u32 max) { return U32_Min(U32_Max(A, min), max); }
inline u32 U32_Modulo(u32 N, u32 D) { return N % D; }

/************/
/*   Vec2   */
/************/
// #define LOAD_VEC2(V) _mm_maskload_ps(V.E, _mm_set_epi32(0, 0, 0xFFFFFFFF, 0xFFFFFFFF))
// #define STORE_VEC2(V,M) _mm_maskstore_ps(V.E, _mm_set_epi32(0, 0, 0xFFFFFFFF, 0xFFFFFFFF), M)
#define LOAD_VEC2(V) _mm_castpd_ps(_mm_load_sd((f64*)&V.E[0]))
#define STORE_VEC2(V,M) _mm_store_sd((f64*)V.E,_mm_castps_pd(M))
inline vec2 Vec2_Zero(void) { vec2 B; STORE_VEC2(B, _mm_setzero_ps()); return B; }
inline vec2 Vec2_Set1(f32 A) { vec2 B; STORE_VEC2(B, _mm_set1_ps(A)); return B; }
inline vec2 Vec2_Add(vec2 A, vec2 B) { STORE_VEC2(A, _mm_add_ps(LOAD_VEC2(A), LOAD_VEC2(B))); return A; }
inline vec2 Vec2_Sub(vec2 A, vec2 B) { STORE_VEC2(A, _mm_sub_ps(LOAD_VEC2(A), LOAD_VEC2(B))); return A; }
inline vec2 Vec2_Mul(vec2 A, vec2 B) { STORE_VEC2(A, _mm_mul_ps(LOAD_VEC2(A), LOAD_VEC2(B))); return A; }
inline vec2 Vec2_Div(vec2 A, vec2 B) { STORE_VEC2(A, _mm_div_ps(LOAD_VEC2(A), LOAD_VEC2(B))); return A; }

// GENERAL FUNCTIONS
inline vec2 Vec2_Sign(vec2 A)   { STORE_VEC2(A, _mm_or_ps(_mm_and_ps(LOAD_VEC2(A), _mm_set1_ps(-0.0f)), _mm_set1_ps(1.0f))); return A; }
inline vec2 Vec2_Abs(vec2 A)    { STORE_VEC2(A, _mm_and_ps(LOAD_VEC2(A), _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)))); return A; }
inline vec2 Vec2_Negate(vec2 A) { STORE_VEC2(A, _mm_xor_ps(LOAD_VEC2(A), _mm_set1_ps(-0.0f))); return A; }
inline vec2 Vec2_Min(vec2 A, vec2 B) { STORE_VEC2(A, _mm_min_ps(LOAD_VEC2(A), LOAD_VEC2(B))); return A; }
inline vec2 Vec2_Max(vec2 A, vec2 B) { STORE_VEC2(A, _mm_max_ps(LOAD_VEC2(A), LOAD_VEC2(B))); return A; }
inline vec2 Vec2_Clamp(vec2 A, vec2 min, vec2 max) { return Vec2_Min(Vec2_Max(A, min), max); }
inline vec2 Vec2_Modulo(vec2 N, vec2 D) { __m128 mN = LOAD_VEC2(N); __m128 mD = LOAD_VEC2(D); STORE_VEC2(N, _mm_sub_ps(mN, _mm_mul_ps(mD, _mm_floor_ps(_mm_div_ps(mN, mD))))); return N; }

// FLOATING POINT FUNCTIONS
inline vec2 Vec2_Round(vec2 A) { STORE_VEC2(A, (_mm_round_ps(LOAD_VEC2(A), _MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC))); return A; }
inline vec2 Vec2_Trunc(vec2 A) { STORE_VEC2(A, (_mm_round_ps(LOAD_VEC2(A), _MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC))); return A; }
inline vec2 Vec2_Ceil(vec2 A)  { STORE_VEC2(A, (_mm_ceil_ps(LOAD_VEC2(A)))); return A; }
inline vec2 Vec2_Floor(vec2 A) { STORE_VEC2(A, (_mm_floor_ps(LOAD_VEC2(A)))); return A; }
inline vec2 Vec2_Sqrt(vec2 A) { STORE_VEC2(A, _mm_sqrt_ps(LOAD_VEC2(A))); return A; }
inline vec2 Vec2_Lerp(vec2 A, vec2 B, f32 t) { __m128 T = _mm_set1_ps(t); STORE_VEC2(A, _mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_set1_ps(1.0f),T),LOAD_VEC2(A)),_mm_mul_ps(T,LOAD_VEC2(B)))); return A; }
inline vec2 Vec3_Lerp2(vec2 A, vec2 B, vec2 t) { __m128 T = LOAD_VEC2(t); STORE_VEC2(A, _mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_set1_ps(1.0f),T),LOAD_VEC2(A)),_mm_mul_ps(T,LOAD_VEC2(B)))); return A; }
inline vec2 Vec2_Fract(vec2 A) { __m128 mA = LOAD_VEC2(A); STORE_VEC2(A, _mm_sub_ps(mA, _mm_floor_ps(mA))); return A; }
inline vec2 Vec2_Inverse(vec2 A) { STORE_VEC2(A, _mm_div_ps(_mm_set1_ps(1), LOAD_VEC2(A))); return A; }
inline vec2 Vec2_Saturate(vec2 A)   { STORE_VEC2(A, _mm_min_ps(_mm_max_ps(LOAD_VEC2(A), _mm_set1_ps(0)), _mm_set1_ps(1))); return A; }

inline vec2 Vec2_SmoothStep(vec2 A)
{
    __m128 x = LOAD_VEC2(A);
    x = _mm_mul_ps(x, _mm_mul_ps(x, _mm_sub_ps(_mm_set1_ps(3), _mm_mul_ps(_mm_set1_ps(2), x))));
    STORE_VEC2(A, _mm_min_ps(_mm_max_ps(x, _mm_set1_ps(0)), _mm_set1_ps(1)));
    return A;
}

inline vec2 Vec2_SmootherStep(vec2 A)
{
    __m128 x = LOAD_VEC2(A);
    x = _mm_mul_ps(x, _mm_mul_ps(x, _mm_mul_ps(x, _mm_add_ps(_mm_mul_ps(x, _mm_sub_ps(_mm_mul_ps(x, _mm_set1_ps(6)), _mm_set1_ps(15))), _mm_set1_ps(10)))));
    STORE_VEC2(A, _mm_min_ps(_mm_max_ps(x, _mm_set1_ps(0)), _mm_set1_ps(1)));
    return A;
}

// VECTOR FUNCTIONS
inline f32 Vec2_Sum(vec2 A)
{
    __m128 V = LOAD_VEC2(A);
    
    __m128 Shf = _mm_movehdup_ps(V);
    __m128 Sum = _mm_add_ss(V, Shf);
    return _mm_cvtss_f32(Sum);
}

inline f32 Vec2_LengthSq(vec2 A)
{
    __m128 V = LOAD_VEC2(A);
    V = _mm_mul_ps(V, V);
    
    __m128 Shf = _mm_movehdup_ps(V);
    __m128 Sum = _mm_add_ss(V, Shf);
    return _mm_cvtss_f32(Sum);
}

inline f32 Vec2_Length(vec2 A)
{
    __m128 V = LOAD_VEC2(A);
    V = _mm_mul_ps(V, V);

    __m128 Shf = _mm_movehdup_ps(V);
    __m128 Sum = _mm_add_ps(V, Shf);
    return _mm_cvtss_f32(_mm_sqrt_ss(Sum));
}

inline f32 Vec2_Dist(vec2 A, vec2 B)
{
    __m128 V = _mm_sub_ps(LOAD_VEC2(A), LOAD_VEC2(B));
    V = _mm_mul_ps(V, V);

    __m128 Shf = _mm_movehdup_ps(V);
    __m128 Sum = _mm_add_ps(V, Shf);
    return _mm_cvtss_f32(_mm_sqrt_ss(Sum));
}

inline vec2 Vec2_Normalize(vec2 A)
{
    __m128 V = LOAD_VEC2(A);
    if (_mm_testz_si128(_mm_castps_si128(V), _mm_set1_epi32(0x7FFFFFFF))) return A;
    __m128 VV = _mm_mul_ps(V, V);
    __m128 Shf = _mm_movehdup_ps(VV);
    __m128 Sum = _mm_add_ps(VV, Shf);
    Sum = _mm_shuffle_ps(Sum, Sum, _MM_SHUFFLE(0,0,0,0));
    Sum = _mm_sqrt_ps(Sum);
    V = _mm_div_ps(V, Sum);
    STORE_VEC2(A, V);
    return A;
}

inline f32 Vec2_Dot(vec2 A, vec2 B)
{
    __m128 V = _mm_mul_ps(LOAD_VEC2(A), LOAD_VEC2(B));
    
    __m128 Shf = _mm_movehdup_ps(V);
    __m128 Sum = _mm_add_ps(V, Shf);
    return _mm_cvtss_f32(Sum);
}

/************/
/*   Vec3   */
/************/
// #define LOAD_VEC3(V) _mm_maskload_ps(V.E, _mm_set_epi32(0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF))
// #define STORE_VEC3(V,M) _mm_maskstore_ps(V.E, _mm_set_epi32(0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF), M)
#define LOAD_VEC3(V) _mm_loadu_ps(V.E)
#define STORE_VEC3(V,M) _mm_storeu_ps(V.E,M)
inline vec3 Vec3_Zero(void) { vec3 B; STORE_VEC3(B, _mm_setzero_ps()); return B; }
inline vec3 Vec3_Set1(f32 A) { vec3 B; STORE_VEC3(B, _mm_set_ps(0,A,A,A)); return B; }
inline vec3 Vec3_Add(vec3 A, vec3 B) { STORE_VEC3(A, _mm_add_ps(LOAD_VEC3(A), LOAD_VEC3(B))); return A; }
inline vec3 Vec3_Sub(vec3 A, vec3 B) { STORE_VEC3(A, _mm_sub_ps(LOAD_VEC3(A), LOAD_VEC3(B))); return A; }
inline vec3 Vec3_Mul(vec3 A, vec3 B) { STORE_VEC3(A, _mm_mul_ps(LOAD_VEC3(A), LOAD_VEC3(B))); return A; }
inline vec3 Vec3_Div(vec3 A, vec3 B) { STORE_VEC3(A, _mm_div_ps(LOAD_VEC3(A), LOAD_VEC3(B))); return A; }

// GENERAL FUNCTIONS
inline vec3 Vec3_Sign(vec3 A)   { STORE_VEC3(A, _mm_or_ps(_mm_and_ps(LOAD_VEC3(A), _mm_set1_ps(-0.0f)), _mm_set1_ps(1.0f))); return A; }
inline vec3 Vec3_Abs(vec3 A)    { STORE_VEC3(A, _mm_and_ps(LOAD_VEC3(A), _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)))); return A; }
inline vec3 Vec3_Negate(vec3 A) { STORE_VEC3(A, _mm_xor_ps(LOAD_VEC3(A), _mm_set1_ps(-0.0f))); return A; }
inline vec3 Vec3_Min(vec3 A, vec3 B) { STORE_VEC3(A, _mm_min_ps(LOAD_VEC3(A), LOAD_VEC3(B))); return A; }
inline vec3 Vec3_Max(vec3 A, vec3 B) { STORE_VEC3(A, _mm_max_ps(LOAD_VEC3(A), LOAD_VEC3(B))); return A; }
inline vec3 Vec3_Clamp(vec3 A, vec3 min, vec3 max) { return Vec3_Min(Vec3_Max(A, min), max); }
inline vec3 Vec3_Modulo(vec3 N, vec3 D) { __m128 mN = LOAD_VEC3(N); __m128 mD = LOAD_VEC3(D); STORE_VEC3(N, _mm_sub_ps(mN, _mm_mul_ps(mD, _mm_floor_ps(_mm_div_ps(mN, mD))))); return N; }

// FLOATING POINT FUNCTIONS
inline vec3 Vec3_Round(vec3 A) { STORE_VEC3(A, (_mm_round_ps(LOAD_VEC3(A), _MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC))); return A; }
inline vec3 Vec3_Trunc(vec3 A) { STORE_VEC3(A, (_mm_round_ps(LOAD_VEC3(A), _MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC))); return A; }
inline vec3 Vec3_Ceil(vec3 A)  { STORE_VEC3(A, (_mm_ceil_ps(LOAD_VEC3(A)))); return A; }
inline vec3 Vec3_Floor(vec3 A) { STORE_VEC3(A, (_mm_floor_ps(LOAD_VEC3(A)))); return A; }
inline vec3 Vec3_Sqrt(vec3 A) { STORE_VEC3(A, _mm_sqrt_ps(LOAD_VEC3(A))); return A; }
inline vec3 Vec3_Lerp(vec3 A, vec3 B, f32 t) { __m128 T = _mm_set1_ps(t); STORE_VEC3(A, _mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_set1_ps(1.0f),T),LOAD_VEC3(A)),_mm_mul_ps(T,LOAD_VEC3(B)))); return A; }
inline vec3 Vec3_Lerp3(vec3 A, vec3 B, vec3 t) { __m128 T = LOAD_VEC3(t); STORE_VEC3(A, _mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_set1_ps(1.0f),T),LOAD_VEC3(A)),_mm_mul_ps(T,LOAD_VEC3(B)))); return A; }
inline vec3 Vec3_Fract(vec3 A) { __m128 m = LOAD_VEC3(A); STORE_VEC3(A, _mm_sub_ps(m, _mm_floor_ps(m))); return A; }
inline vec3 Vec3_Inverse(vec3 A) { STORE_VEC3(A, _mm_div_ps(_mm_set1_ps(1), LOAD_VEC3(A))); return A; }
inline vec3 Vec3_Saturate(vec3 A)   { STORE_VEC3(A, _mm_min_ps(_mm_max_ps(LOAD_VEC3(A), _mm_set1_ps(0)), _mm_set1_ps(1))); return A; }

inline vec3 Vec3_SmoothStep(vec3 A)
{
    __m128 x = LOAD_VEC3(A);
    x = _mm_mul_ps(x, _mm_mul_ps(x, _mm_sub_ps(_mm_set1_ps(3), _mm_mul_ps(_mm_set1_ps(2), x))));
    STORE_VEC3(A, _mm_min_ps(_mm_max_ps(x, _mm_set1_ps(0)), _mm_set1_ps(1)));
    return A;
}

inline vec3 Vec3_SmootherStep(vec3 A)
{
    __m128 x = LOAD_VEC3(A);
    x = _mm_mul_ps(x, _mm_mul_ps(x, _mm_mul_ps(x, _mm_add_ps(_mm_mul_ps(x, _mm_sub_ps(_mm_mul_ps(x, _mm_set1_ps(6)), _mm_set1_ps(15))), _mm_set1_ps(10)))));
    STORE_VEC3(A, _mm_min_ps(_mm_max_ps(x, _mm_set1_ps(0)), _mm_set1_ps(1)));
    return A;
}

// VECTOR FUNCTIONS
inline f32 Vec3_Sum(vec3 A)
{
    __m128 V = LOAD_VEC3(A);
    V = _mm_and_ps(V, _mm_castsi128_ps(_mm_set_epi32(0,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF)));
    
    __m128 Shf = _mm_movehdup_ps(V);
    __m128 Sum = _mm_add_ps(V, Shf);
    Shf = _mm_movehl_ps(Shf, Sum);
    Sum = _mm_add_ss(Sum, Shf);
    return _mm_cvtss_f32(Sum);
}

inline f32 Vec3_LengthSq(vec3 A)
{
    __m128 V = LOAD_VEC3(A);
    V = _mm_and_ps(V, _mm_castsi128_ps(_mm_set_epi32(0,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF)));
    V = _mm_mul_ps(V, V);

    __m128 Shf = _mm_movehdup_ps(V);
    __m128 Sum = _mm_add_ps(V, Shf);
    Shf = _mm_movehl_ps(Shf, Sum);
    Sum = _mm_add_ss(Sum, Shf);
    return _mm_cvtss_f32(Sum);
}

inline f32 Vec3_Length(vec3 A)
{
    __m128 V = LOAD_VEC3(A);
    V = _mm_and_ps(V, _mm_castsi128_ps(_mm_set_epi32(0,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF)));
    V = _mm_mul_ps(V, V);

    __m128 Shf = _mm_movehdup_ps(V);
    __m128 Sum = _mm_add_ps(V, Shf);
    Shf = _mm_movehl_ps(Shf, Sum);
    Sum = _mm_add_ss(Sum, Shf);
    return _mm_cvtss_f32(_mm_sqrt_ss(Sum));
}

inline vec3 Vec3_Normalize(vec3 A)
{
    __m128 V = LOAD_VEC3(A);
    V = _mm_and_ps(V, _mm_castsi128_ps(_mm_set_epi32(0,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF)));
    if (_mm_testz_si128(_mm_castps_si128(V), _mm_set1_epi32(0x7FFFFFFF))) return A;
    __m128 VV = _mm_mul_ps(V, V);
    __m128 Shf = _mm_movehdup_ps(VV);
    __m128 Sum = _mm_add_ps(VV, Shf);
    Shf = _mm_movehl_ps(Shf, Sum);
    Sum = _mm_add_ss(Sum, Shf);
    Sum = _mm_shuffle_ps(Sum, Sum, _MM_SHUFFLE(0,0,0,0));
    Sum = _mm_sqrt_ps(Sum);
    V = _mm_div_ps(V, Sum);
    STORE_VEC3(A, V);
    return A;
}

inline f32 Vec3_Dot(vec3 A, vec3 B)
{
    __m128 V = _mm_mul_ps(LOAD_VEC3(A), LOAD_VEC3(B));
    V = _mm_and_ps(V, _mm_castsi128_ps(_mm_set_epi32(0,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF)));
    __m128 Shf = _mm_movehdup_ps(V);
    __m128 Sum = _mm_add_ps(V, Shf);
    Shf = _mm_movehl_ps(Shf, Sum);
    Sum = _mm_add_ss(Sum, Shf);
    return _mm_cvtss_f32(Sum);
}

inline vec3 Vec3_Cross(vec3 A, vec3 B)
{
    __m128 mA = LOAD_VEC3(A);
    __m128 mB = LOAD_VEC3(B);

    __m128 V = _mm_sub_ps(
        _mm_mul_ps(mA, _mm_shuffle_ps(mB, mB, _MM_SHUFFLE(3,0,2,1))),
        _mm_mul_ps(mB, _mm_shuffle_ps(mA, mA, _MM_SHUFFLE(3,0,2,1))));
    V = _mm_shuffle_ps(V, V, _MM_SHUFFLE(3,0,2,1));
    STORE_VEC3(A, V);
    return A;
}

/************/
/*   Vec4   */
/************/
#define LOAD_VEC4(V) _mm_loadu_ps(V.E)
#define STORE_VEC4(V,M) _mm_storeu_ps(V.E,M)
inline vec4 Vec4_Zero(void) { vec4 B; STORE_VEC4(B, _mm_setzero_ps()); return B; }
inline vec4 Vec4_Set1(f32 A) { vec4 B; STORE_VEC4(B, _mm_set1_ps(A)); return B; }
inline vec4 Vec4_Add(vec4 A, vec4 B) { STORE_VEC4(A, _mm_add_ps(LOAD_VEC4(A), LOAD_VEC4(B))); return A; }
inline vec4 Vec4_Sub(vec4 A, vec4 B) { STORE_VEC4(A, _mm_sub_ps(LOAD_VEC4(A), LOAD_VEC4(B))); return A; }
inline vec4 Vec4_Mul(vec4 A, vec4 B) { STORE_VEC4(A, _mm_mul_ps(LOAD_VEC4(A), LOAD_VEC4(B))); return A; }
inline vec4 Vec4_Div(vec4 A, vec4 B) { STORE_VEC4(A, _mm_div_ps(LOAD_VEC4(A), LOAD_VEC4(B))); return A; }

/*************/
/*   iVec2   */
/*************/
#define LOAD_IVEC2(V) _mm_loadu_si64(V.E)
#define STORE_IVEC2(V,M) _mm_storeu_si64(V.E,M)
inline ivec2 iVec2_Zero(void) { ivec2 B; STORE_IVEC2(B, _mm_setzero_si128()); return B; }
inline ivec2 iVec2_Set1(i32 A) { return (ivec2){ .x = A, .y = A }; }
inline ivec2 iVec2_Add(ivec2 A, ivec2 B) { STORE_IVEC2(A, _mm_add_epi32(LOAD_IVEC2(A), LOAD_IVEC2(B))); return A; }
inline ivec2 iVec2_Sub(ivec2 A, ivec2 B) { STORE_IVEC2(A, _mm_sub_epi32(LOAD_IVEC2(A), LOAD_IVEC2(B))); return A; }
inline ivec2 iVec2_Mul(ivec2 A, ivec2 B) { STORE_IVEC2(A, _mm_mul_epi32(LOAD_IVEC2(A), LOAD_IVEC2(B))); return A; }
inline ivec2 iVec2_Div(ivec2 A, ivec2 B) { STORE_IVEC2(A, _mm_div_epi32(LOAD_IVEC2(A), LOAD_IVEC2(B))); return A; }

inline ivec2 iVec2_ShiftLeft(ivec2 A, i32 S) { STORE_IVEC2(A, _mm_slli_epi32(LOAD_IVEC2(A), S)); return A; }
inline ivec2 iVec2_ShiftRight(ivec2 A, i32 S) { STORE_IVEC2(A, _mm_srai_epi32(LOAD_IVEC2(A), S)); return A; }
inline ivec2 iVec2_And(ivec2 A, ivec2 B) { STORE_IVEC2(A, _mm_and_si128(LOAD_IVEC2(A), LOAD_IVEC2(B))); return A; }
inline ivec2 iVec2_Or(ivec2 A, ivec2 B) { STORE_IVEC2(A, _mm_or_si128(LOAD_IVEC2(A), LOAD_IVEC2(B))); return A; }
inline ivec2 iVec2_Xor(ivec2 A, ivec2 B) { STORE_IVEC2(A, _mm_xor_si128(LOAD_IVEC2(A), LOAD_IVEC2(B))); return A; }

/******************/
/* Vec2 <-> iVec2 */
/******************/
inline vec2 iVec2_toVec2(ivec2 A) { vec2 B; STORE_VEC2(B, _mm_cvtepi32_ps(LOAD_IVEC2(A))); return B; }
inline ivec2 Vec2_toIVec2(vec2 A) { ivec2 B; STORE_IVEC2(B, _mm_cvtps_epi32(LOAD_VEC2(A))); return B; }
inline ivec2 Vec2_RoundToIVec2(vec2 A) { ivec2 B; STORE_IVEC2(B, _mm_cvtps_epi32(_mm_round_ps(LOAD_VEC2(A), _MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC))); return B; }
inline ivec2 Vec2_TruncToIVec2(vec2 A) { ivec2 B; STORE_IVEC2(B, _mm_cvtps_epi32(_mm_round_ps(LOAD_VEC2(A), _MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC))); return B; }
inline ivec2 Vec2_CeilToIVec2(vec2 A)  { ivec2 B; STORE_IVEC2(B, _mm_cvtps_epi32(_mm_ceil_ps(LOAD_VEC2(A)))); return B; }
inline ivec2 Vec2_FloorToIVec2(vec2 A) { ivec2 B; STORE_IVEC2(B, _mm_cvtps_epi32(_mm_floor_ps(LOAD_VEC2(A)))); return B; }

/*************/
/*   iVec3   */
/*************/
#define LOAD_IVEC3(V) _mm_loadu_si128((__m128i *)V.E)
#define STORE_IVEC3(V,M) _mm_storeu_si128((__m128i *)V.E,M)
inline ivec3 iVec3_Zero(void) { ivec3 B; STORE_IVEC3(B, _mm_setzero_si128()); return B; }
inline ivec3 iVec3_Set1(i32 A) { return (ivec3){ .x = A, .y = A, .z = A}; }
inline ivec3 iVec3_Add(ivec3 A, ivec3 B) { STORE_IVEC3(A, _mm_add_epi32(LOAD_IVEC3(A), LOAD_IVEC3(B))); return A; }
inline ivec3 iVec3_Sub(ivec3 A, ivec3 B) { STORE_IVEC3(A, _mm_sub_epi32(LOAD_IVEC3(A), LOAD_IVEC3(B))); return A; }
inline ivec3 iVec3_Mul(ivec3 A, ivec3 B) { STORE_IVEC3(A, _mm_mul_epi32(LOAD_IVEC3(A), LOAD_IVEC3(B))); return A; }
inline ivec3 iVec3_Div(ivec3 A, ivec3 B) { STORE_IVEC3(A, _mm_div_epi32(LOAD_IVEC3(A), LOAD_IVEC3(B))); return A; }

// GENERAL FUNCTIONS
inline ivec3 iVec3_Abs(ivec3 A)          { STORE_IVEC3(A, _mm_abs_epi32(LOAD_IVEC3(A))); return A; }
inline ivec3 iVec3_Min(ivec3 A, ivec3 B) { STORE_IVEC3(A, _mm_min_epi32(LOAD_IVEC3(A), LOAD_IVEC3(B))); return A; }
inline ivec3 iVec3_Max(ivec3 A, ivec3 B) { STORE_IVEC3(A, _mm_max_epi32(LOAD_IVEC3(A), LOAD_IVEC3(B))); return A; }

// INTEGER FUNCTIONS
inline ivec3 iVec3_ShiftLeft(ivec3 A, i32 S) { STORE_IVEC3(A, _mm_slli_epi32(LOAD_IVEC3(A), S)); return A; }
inline ivec3 iVec3_ShiftRight(ivec3 A, i32 S) { STORE_IVEC3(A, _mm_srai_epi32(LOAD_IVEC3(A), S)); return A; }
inline ivec3 iVec3_And(ivec3 A, ivec3 B) { STORE_IVEC3(A, _mm_and_si128(LOAD_IVEC3(A), LOAD_IVEC3(B))); return A; }
inline ivec3 iVec3_Or(ivec3 A, ivec3 B) { STORE_IVEC3(A, _mm_or_si128(LOAD_IVEC3(A), LOAD_IVEC3(B))); return A; }
inline ivec3 iVec3_Xor(ivec3 A, ivec3 B) { STORE_IVEC3(A, _mm_xor_si128(LOAD_IVEC3(A), LOAD_IVEC3(B))); return A; }

// VECTOR FUNCTIONS
inline i32 iVec3_Sum(ivec3 A)
{
    __m128i V = LOAD_IVEC3(A);
    
    __m128i Sum = V;
    Sum = _mm_add_epi32(Sum, _mm_shuffle_epi32(V, _MM_SHUFFLE(0,0,0,1)));
    Sum = _mm_add_epi32(Sum, _mm_shuffle_epi32(V, _MM_SHUFFLE(0,0,0,2)));
    return _mm_cvtsi128_si32(Sum);
}

/******************/
/* Vec3 <-> iVec3 */
/******************/
inline vec3 iVec3_toVec3(ivec3 A) { vec3 B; STORE_VEC3(B, _mm_cvtepi32_ps(LOAD_IVEC3(A))); return B; }
inline ivec3 Vec3_toIVec3(vec3 A) { ivec3 B; STORE_IVEC3(B, _mm_cvtps_epi32(LOAD_VEC3(A))); return B; }
inline ivec3 Vec3_RoundToIVec3(vec3 A) { ivec3 B; STORE_IVEC3(B, _mm_cvtps_epi32(_mm_round_ps(LOAD_VEC3(A), _MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC))); return B; }
inline ivec3 Vec3_TruncToIVec3(vec3 A) { ivec3 B; STORE_IVEC3(B, _mm_cvtps_epi32(_mm_round_ps(LOAD_VEC3(A), _MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC))); return B; }
inline ivec3 Vec3_CeilToIVec3(vec3 A)  { ivec3 B; STORE_IVEC3(B, _mm_cvtps_epi32(_mm_ceil_ps(LOAD_VEC3(A)))); return B; }
inline ivec3 Vec3_FloorToIVec3(vec3 A) { ivec3 B; STORE_IVEC3(B, _mm_cvtps_epi32(_mm_floor_ps(LOAD_VEC3(A)))); return B; }

/************/
/* Generics */
/************/

// ARITHMETIC FUNCTIONS
#define Add(A,B) _Generic((A), \
    vec2: Vec2_Add, \
    vec3: Vec3_Add, \
    ivec2: iVec2_Add, \
    ivec3: iVec3_Add \
)(A,B)

#define Sub(A,B) _Generic((A), \
    vec2: Vec2_Sub, \
    vec3: Vec3_Sub, \
    ivec2: iVec2_Sub, \
    ivec3: iVec3_Sub \
)(A,B)

#define Mul(A,B) _Generic((A), \
    vec2: Vec2_Mul, \
    vec3: Vec3_Mul, \
    ivec2: iVec2_Mul, \
    ivec3: iVec3_Mul \
)(A,B)

#define Div(A,B) _Generic((A), \
    vec2: Vec2_Div, \
    vec3: Vec3_Div, \
    ivec2: iVec2_Div, \
    ivec3: iVec3_Div \
)(A,B)

// GENERAL FUNCTIONS
#define Abs(A) _Generic((A), \
    f32: F32_Abs, \
    i32: I32_Abs, \
    vec2: Vec2_Abs, \
    vec3: Vec3_Abs \
)(A)

#define Sign(A)  _Generic((A), \
    f32: F32_Sign, \
    i32: I32_Sign, \
    vec2: Vec2_Sign, \
    vec3: Vec3_Sign \
)(A)

#define Negate(A) _Generic((A), \
    f32: F32_Negate, \
    i32: I32_Negate, \
    vec2: Vec2_Negate, \
    vec3: Vec3_Negate \
)(A)

#define Min(A,B) _Generic((A), \
    f32: F32_Min, \
    i32: I32_Min, \
    u16: U16_Min, \
    u32: U32_Min, \
    vec2: Vec2_Min, \
    vec3: Vec3_Min \
)(A,B)

#define Max(A,B) _Generic((A), \
    f32: F32_Max, \
    i32: I32_Max, \
    u16: U16_Max, \
    u32: U32_Max, \
    vec2: Vec2_Max, \
    vec3: Vec3_Max \
)(A,B)

#define Clamp(A,B,C) _Generic((A), \
    f32: F32_Clamp, \
    i32: I32_Clamp, \
    u16: U16_Clamp, \
    u32: U32_Clamp, \
    vec2: Vec2_Clamp, \
    vec3: Vec3_Clamp \
)(A,B,C)

#define Modulo(A,B) _Generic((A), \
    f32: F32_Modulo, \
    i32: I32_Modulo, \
    u32: U32_Modulo, \
    vec2: Vec2_Modulo, \
    vec3: Vec3_Modulo \
)(A,B)

// FLOATING POINT FUNCTIONS
#define Round(A) _Generic((A), \
    f32: F32_Round, \
    vec2: Vec2_Round, \
    vec3: Vec3_Round \
)(A)

#define Trunc(A) _Generic((A), \
    f32: F32_Trunc, \
    vec2: Vec2_Trunc, \
    vec3: Vec3_Trunc \
)(A)

#define Ceil(A) _Generic((A), \
    f32: F32_Ceil, \
    vec2: Vec2_Ceil, \
    vec3: Vec3_Ceil \
)(A)

#define Floor(A) _Generic((A), \
    f32: F32_Floor, \
    vec2: Vec2_Floor, \
    vec3: Vec3_Floor \
)(A)

#define Sqrt(A) _Generic((A), \
    f32: F32_Sqrt, \
    vec2: Vec2_Sqrt, \
    vec3: Vec3_Sqrt \
)(A)

#define Lerp(A,B,C) _Generic((A), \
    f32: F32_Lerp, \
    vec2: Vec2_Lerp, \
    vec3: Vec3_Lerp \
)(A,B,C)

#define Fract(A) _Generic((A), \
    f32: F32_Fract, \
    vec2: Vec2_Fract, \
    vec3: Vec3_Fract \
)(A)

#define Inverse(A) _Generic((A), \
    f32: F32_Inverse, \
    vec2: Vec2_Inverse, \
    vec3: Vec3_Inverse \
)(A)

#define Saturate(A) _Generic((A), \
    f32: F32_Saturate, \
    vec2: Vec2_Saturate, \
    vec3: Vec3_Saturate \
)(A)

#define SmoothStep(A) _Generic((A), \
    f32: F32_Smoothstep, \
    vec2: Vec2_Smoothstep, \
    vec3: Vec3_Smoothstep \
)(A)

#define SmootherStep(A) _Generic((A), \
    f32: F32_SmootherStep, \
    vec2: Vec2_SmootherStep, \
    vec3: Vec3_SmootherStep \
)(A)

// INTEGER FUNCTIONS
#define ShiftLeft(A,S)_Generic((A), \
    ivec2: iVec2_ShiftLeft, \
    ivec3: iVec3_ShiftLeft \
)(A,S)

#define ShiftRight(A,S)_Generic((A), \
    ivec2: iVec2_ShiftRight, \
    ivec3: iVec3_ShiftRight \
)(A,S)

#define And(A,B)_Generic((A), \
    ivec2: iVec2_And, \
    ivec3: iVec3_And \
)(A,B)

#define Or(A,B)_Generic((A), \
    ivec2: iVec2_Or, \
    ivec3: iVec3_Or \
)(A,B)

#define Xor(A,B)_Generic((A), \
    ivec2: iVec2_Xor, \
    ivec3: iVec3_Xor \
)(A,B)

// VECTOR FUNCTIONS
#define Sum(A) _Generic((A), \
    vec2: Vec2_Sum, \
    vec3: Vec3_Sum \
)(A)

/************/
/*   Mat3   */
/************/
typedef struct mat3
{
    vec3 Columns[3];
} mat3;

mat3 Mat3_Id(void)
{
    return (mat3){{
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 0, 0, 1 },
    }};    
}

mat3 Mat3_RotYaw(f32 Yaw)
{
    f32 S = Sin(Yaw);
    f32 C = Cos(Yaw);
    return (mat3){{
        { C, S, 0 },
        {-S, C, 0 },
        { 0, 0, 1 },
    }};
}

mat3 Mat3_RotPitch(f32 Pitch)
{
    f32 S = Sin(Pitch);
    f32 C = Cos(Pitch);
    return (mat3){{
        { 1, 0, 0 },
        { 0, C,-S },
        { 0, S, C },
    }};
}

vec3 Mat3_MulVec3(mat3 M, vec3 V)
{
    __m128 C = LOAD_VEC3(V);
    __m128 C0 = _mm_mul_ps(_mm_loadu_ps(M.Columns[0].E), _mm_shuffle_ps(C, C, _MM_SHUFFLE(0,0,0,0)));
    __m128 C1 = _mm_mul_ps(_mm_loadu_ps(M.Columns[1].E), _mm_shuffle_ps(C, C, _MM_SHUFFLE(1,1,1,1)));
    __m128 C2 = _mm_mul_ps(_mm_loadu_ps(M.Columns[2].E), _mm_shuffle_ps(C, C, _MM_SHUFFLE(2,2,2,2)));
    STORE_VEC3(V, _mm_add_ps(_mm_add_ps(C0, C1), C2));
    return V;
}

mat3 Mat3_Transpose(mat3 M)
{
    __m128 C0 = LOAD_VEC3(M.Columns[0]);
    __m128 C1 = LOAD_VEC3(M.Columns[1]);
    __m128 C2 = LOAD_VEC3(M.Columns[2]);
    __m128 C3 = _mm_setzero_ps();

    __m128 T0 = _mm_unpacklo_ps(C0, C1);
    __m128 T1 = _mm_unpackhi_ps(C0, C1);
    __m128 T2 = _mm_unpacklo_ps(C2, C3);
    __m128 T3 = _mm_unpackhi_ps(C2, C3);

    C0 = _mm_movelh_ps(T0, T2);
    C1 = _mm_movehl_ps(T2, T0);
    C2 = _mm_movelh_ps(T1, T3);

    STORE_VEC3(M.Columns[0], C0);
    STORE_VEC3(M.Columns[1], C1);
    STORE_VEC3(M.Columns[2], C2);

    return M;
}

mat3 Mat3_Mul(mat3 A, mat3 B)
{
    mat3 Result;
    Result.Columns[0] = Mat3_MulVec3(A, B.Columns[0]);
    Result.Columns[1] = Mat3_MulVec3(A, B.Columns[1]);
    Result.Columns[2] = Mat3_MulVec3(A, B.Columns[2]);
    return Result;
}

/************/
/*   Mat4   */
/************/
typedef struct mat4
{
    vec4 Columns[4];
} mat4;

mat4 Mat4_Id(void)
{
    return (mat4){{
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 },
    }};    
}

mat4 Mat4_Translation(vec3 Translation)
{
    return (mat4){{
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { -Translation.x, -Translation.y, -Translation.z, 1 },
    }};
}

mat4 Mat4_RotYaw(f32 Yaw)
{
    f32 S = Sin(Yaw);
    f32 C = Cos(Yaw);
    return (mat4){{
        { C, S, 0, 0 },
        {-S, C, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 },
    }};
}

mat4 Mat4_RotPitch(f32 Pitch)
{
    f32 S = Sin(Pitch);
    f32 C = Cos(Pitch);
    return (mat4){{
        { 1, 0, 0, 0 },
        { 0, C,-S, 0 },
        { 0, S, C, 0 },
        { 0, 0, 0, 1 },
    }};
}

mat4 Mat4_Transpose(mat4 M)
{
    __m128 C0 = LOAD_VEC4(M.Columns[0]);
    __m128 C1 = LOAD_VEC4(M.Columns[1]);
    __m128 C2 = LOAD_VEC4(M.Columns[2]);
    __m128 C3 = LOAD_VEC4(M.Columns[3]);

    __m128 T0 = _mm_unpacklo_ps(C0, C1);
    __m128 T1 = _mm_unpackhi_ps(C0, C1);
    __m128 T2 = _mm_unpacklo_ps(C2, C3);
    __m128 T3 = _mm_unpackhi_ps(C2, C3);

    C0 = _mm_movelh_ps(T0, T2);
    C1 = _mm_movehl_ps(T2, T0);
    C2 = _mm_movelh_ps(T1, T3);
    C3 = _mm_movehl_ps(T1, T3);

    STORE_VEC4(M.Columns[0], C0);
    STORE_VEC4(M.Columns[1], C1);
    STORE_VEC4(M.Columns[2], C2);
    STORE_VEC4(M.Columns[3], C3);

    return M;
}

vec4 Mat4_MulVec4(mat4 M, vec4 V)
{
    __m128 C = LOAD_VEC4(V);
    __m128 C0 = _mm_mul_ps(LOAD_VEC4(M.Columns[0]), _mm_shuffle_ps(C, C, _MM_SHUFFLE(0,0,0,0)));
    __m128 C1 = _mm_mul_ps(LOAD_VEC4(M.Columns[1]), _mm_shuffle_ps(C, C, _MM_SHUFFLE(1,1,1,1)));
    __m128 C2 = _mm_mul_ps(LOAD_VEC4(M.Columns[2]), _mm_shuffle_ps(C, C, _MM_SHUFFLE(2,2,2,2)));
    __m128 C3 = _mm_mul_ps(LOAD_VEC4(M.Columns[3]), _mm_shuffle_ps(C, C, _MM_SHUFFLE(3,3,3,3)));
    STORE_VEC4(V, _mm_add_ps(_mm_add_ps(C0, C1), _mm_add_ps(C2, C3)));
    return V;
}

mat4 Mat4_Mul(mat4 A, mat4 B)
{
    __m128 C0 = LOAD_VEC4(A.Columns[0]);
    __m128 C1 = LOAD_VEC4(A.Columns[1]);
    __m128 C2 = LOAD_VEC4(A.Columns[2]);
    __m128 C3 = LOAD_VEC4(A.Columns[3]);

    mat4 Result;
    for (u32 i = 0; i < 4; ++i)
    {
        __m128 V = LOAD_VEC4(B.Columns[i]);
        __m128 V0 = _mm_mul_ps(C0, _mm_shuffle_ps(V, V, _MM_SHUFFLE(0,0,0,0)));
        __m128 V1 = _mm_mul_ps(C1, _mm_shuffle_ps(V, V, _MM_SHUFFLE(1,1,1,1)));
        __m128 V2 = _mm_mul_ps(C2, _mm_shuffle_ps(V, V, _MM_SHUFFLE(2,2,2,2)));
        __m128 V3 = _mm_mul_ps(C3, _mm_shuffle_ps(V, V, _MM_SHUFFLE(3,3,3,3)));
        STORE_VEC4(Result.Columns[i], _mm_add_ps(_mm_add_ps(V0, V1), _mm_add_ps(V2, V3)));
    }
    return Result;
}
