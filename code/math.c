
#include <intrin.h>

typedef union
{
    struct { f32 x, y; };
    struct { f32 u, v; };
    f32 E[2];
} vec2;

typedef union
{
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
    struct
    {
        f32 r;
        union
        {
            struct { f32 g, b; };
            vec2 gb;
        };
    };
    struct { vec2 rg; };
    f32 E[4];
} vec3;

#define MATH_PI 3.14159265358979323846f

#define ABS(A) (((A)<(0))?(-A):(A))
#define SIGN(A) (((A)<(0))?(-1):(1))
#define NEGATE(A) (-(A))
#define MIN(A,B) (((A)<(B))?(A):(B))
#define MAX(A,B) (((A)>(B))?(A):(B))
#define CLAMP(A,B,C) MIN(MAX(A,B),C)



/***********/
/*   F32   */
/***********/
inline f32 F32_Round(f32 A) { return _mm_cvtss_f32(_mm_round_ps(_mm_set_ss(A), _MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC)); }
inline f32 F32_Trunc(f32 A) { return _mm_cvtss_f32(_mm_round_ps(_mm_set_ss(A), _MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC)); }
inline f32 F32_Ceil(f32 A)  { return _mm_cvtss_f32(_mm_ceil_ps(_mm_set_ss(A))); }
inline f32 F32_Floor(f32 A) { return _mm_cvtss_f32(_mm_floor_ps(_mm_set_ss(A))); }
inline f32 F32_Sqrt(f32 A) { return _mm_cvtss_f32(_mm_sqrt_ps(_mm_set_ss(A)));}
inline f32 F32_Lerp(f32 A, f32 B, f32 t) { return (1.0f-t)*A + t*B; }
inline f32 F32_Fract(f32 A) { return A - F32_Floor(A); }

inline f32 F32_Abs(f32 A)    { return _mm_cvtss_f32(_mm_and_ps(_mm_set_ss(A), _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)))); }
inline f32 F32_Sign(f32 A)   { return _mm_cvtss_f32(_mm_or_ps(_mm_and_ps(_mm_set_ss(A), _mm_set_ss(-0.0f)), _mm_set_ss(1.0f))); }
inline f32 F32_Negate(f32 A) { return _mm_cvtss_f32(_mm_xor_ps(_mm_set_ss(A), _mm_set_ss(-0.0f))); }
inline f32 F32_Min(f32 A, f32 B) { return _mm_cvtss_f32(_mm_min_ps(_mm_set_ss(A), _mm_set_ss(B))); }
inline f32 F32_Max(f32 A, f32 B) { return _mm_cvtss_f32(_mm_max_ps(_mm_set_ss(A), _mm_set_ss(B))); }
inline f32 F32_Clamp(f32 A, f32 min, f32 max) { return F32_Min(F32_Max(A, min), max); }
inline f32 F32_Modulo(f32 N, f32 D) { return N - D * F32_Floor(N / D); }

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

/**************/
/* F32 -> I32 */
/**************/
inline i32 F32_RoundToI32(f32 A) { return _mm_cvt_ss2si(_mm_round_ps(_mm_set_ss(A), _MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC)); }
inline i32 F32_TruncToI32(f32 A) { return _mm_cvt_ss2si(_mm_round_ps(_mm_set_ss(A), _MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC)); }
inline i32 F32_CeilToI32(f32 A)  { return _mm_cvt_ss2si(_mm_ceil_ps(_mm_set_ss(A))); }
inline i32 F32_FloorToI32(f32 A) { return _mm_cvt_ss2si(_mm_floor_ps(_mm_set_ss(A))); }
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
/*   U32   */
/***********/
inline u32 U32_Max(u32 A, u32 B) { return _mm_cvtsi128_si32(_mm_max_epu32(_mm_cvtsi32_si128(A),_mm_cvtsi32_si128(B))); }
inline u32 U32_Min(u32 A, u32 B) { return _mm_cvtsi128_si32(_mm_min_epu32(_mm_cvtsi32_si128(A),_mm_cvtsi32_si128(B))); }
inline u32 U32_Clamp(u32 A, u32 min, u32 max) { return U32_Min(U32_Max(A, min), max); }
inline u32 U32_Modulo(u32 N, u32 D) { return N % D; }

/************/
/*   Vec2   */
/************/
#define LOAD_VEC2(V) _mm_castpd_ps(_mm_load_sd((f64*)&V.E[0]))
#define STORE_VEC2(V,M) _mm_store_sd((f64*)V.E,_mm_castps_pd(M))
inline vec2 Vec2_Add(vec2 A, vec2 B) { STORE_VEC2(A, _mm_add_ps(LOAD_VEC2(A), LOAD_VEC2(B))); return A; }
inline vec2 Vec2_Sub(vec2 A, vec2 B) { STORE_VEC2(A, _mm_sub_ps(LOAD_VEC2(A), LOAD_VEC2(B))); return A; }
inline vec2 Vec2_Mul(vec2 A, vec2 B) { STORE_VEC2(A, _mm_mul_ps(LOAD_VEC2(A), LOAD_VEC2(B))); return A; }
inline vec2 Vec2_Div(vec2 A, vec2 B) { STORE_VEC2(A, _mm_div_ps(LOAD_VEC2(A), LOAD_VEC2(B))); return A; }

inline vec2 Vec2_Round(vec2 A) { STORE_VEC2(A, (_mm_round_ps(LOAD_VEC2(A), _MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC))); return A; }
inline vec2 Vec2_Trunc(vec2 A) { STORE_VEC2(A, (_mm_round_ps(LOAD_VEC2(A), _MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC))); return A; }
inline vec2 Vec2_Ceil(vec2 A)  { STORE_VEC2(A, (_mm_ceil_ps(LOAD_VEC2(A)))); return A; }
inline vec2 Vec2_Floor(vec2 A) { STORE_VEC2(A, (_mm_floor_ps(LOAD_VEC2(A)))); return A; }
inline vec2 Vec2_Sqrt(vec2 A) { STORE_VEC2(A, _mm_sqrt_ps(LOAD_VEC2(A))); return A; }
inline vec2 Vec2_Lerp(vec2 A, vec2 B, f32 t) { STORE_VEC2(A, _mm_add_ps(_mm_mul_ps(_mm_set1_ps(1.0f-t),LOAD_VEC2(A)),_mm_mul_ps(_mm_set1_ps(t),LOAD_VEC2(B)))); return A; }
inline vec2 Vec2_Fract(vec2 A) { __m128 m = LOAD_VEC2(A); STORE_VEC2(A, _mm_sub_ps(m, _mm_floor_ps(m))); return A; }

inline vec2 Vec2_Sign(vec2 A)   { STORE_VEC2(A, _mm_or_ps(_mm_and_ps(LOAD_VEC2(A), _mm_set1_ps(-0.0f)), _mm_set1_ps(1.0f))); return A; }
inline vec2 Vec2_Abs(vec2 A)    { STORE_VEC2(A, _mm_and_ps(LOAD_VEC2(A), _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)))); return A; }
inline vec2 Vec2_Negate(vec2 A) { STORE_VEC2(A, _mm_xor_ps(LOAD_VEC2(A), _mm_set1_ps(-0.0f))); return A; }
inline vec2 Vec2_Min(vec2 A, vec2 B) { STORE_VEC2(A, _mm_min_ps(LOAD_VEC2(A), LOAD_VEC2(B))); return A; }
inline vec2 Vec2_Max(vec2 A, vec2 B) { STORE_VEC2(A, _mm_max_ps(LOAD_VEC2(A), LOAD_VEC2(B))); return A; }
inline vec2 Vec2_Clamp(vec2 A, vec2 min, vec2 max) { return Vec2_Min(Vec2_Max(A, min), max); }
inline vec2 Vec2_Modulo(vec2 N, vec2 D) { return Vec2_Sub(N, Vec2_Mul(D, Vec2_Floor(Vec2_Div(N, D)))); }

/************/
/*   Vec3   */
/************/
#define LOAD_VEC3(V) _mm_loadu_ps(V.E)
#define STORE_VEC3(V,M) _mm_storeu_ps(V.E,M)
inline vec3 Vec3_Add(vec3 A, vec3 B) { STORE_VEC3(A, _mm_add_ps(LOAD_VEC3(A), LOAD_VEC3(B))); return A; }
inline vec3 Vec3_Sub(vec3 A, vec3 B) { STORE_VEC3(A, _mm_sub_ps(LOAD_VEC3(A), LOAD_VEC3(B))); return A; }
inline vec3 Vec3_Mul(vec3 A, vec3 B) { STORE_VEC3(A, _mm_mul_ps(LOAD_VEC3(A), LOAD_VEC3(B))); return A; }
inline vec3 Vec3_Div(vec3 A, vec3 B) { STORE_VEC3(A, _mm_div_ps(LOAD_VEC3(A), LOAD_VEC3(B))); return A; }

inline vec3 Vec3_Round(vec3 A) { STORE_VEC3(A, (_mm_round_ps(LOAD_VEC3(A), _MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC))); return A; }
inline vec3 Vec3_Trunc(vec3 A) { STORE_VEC3(A, (_mm_round_ps(LOAD_VEC3(A), _MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC))); return A; }
inline vec3 Vec3_Ceil(vec3 A)  { STORE_VEC3(A, (_mm_ceil_ps(LOAD_VEC3(A)))); return A; }
inline vec3 Vec3_Floor(vec3 A) { STORE_VEC3(A, (_mm_floor_ps(LOAD_VEC3(A)))); return A; }
inline vec3 Vec3_Sqrt(vec3 A) { STORE_VEC3(A, _mm_sqrt_ps(LOAD_VEC3(A))); return A; }
inline vec3 Vec3_Lerp(vec3 A, vec3 B, f32 t) { STORE_VEC3(A, _mm_add_ps(_mm_mul_ps(_mm_set1_ps(1.0f-t),LOAD_VEC3(A)),_mm_mul_ps(_mm_set1_ps(t),LOAD_VEC3(B)))); return A; }
inline vec3 Vec3_Fract(vec3 A) { __m128 m = LOAD_VEC3(A); STORE_VEC3(A, _mm_sub_ps(m, _mm_floor_ps(m))); return A; }

inline vec3 Vec3_Sign(vec3 A)   { STORE_VEC3(A, _mm_or_ps(_mm_and_ps(LOAD_VEC3(A), _mm_set1_ps(-0.0f)), _mm_set1_ps(1.0f))); return A; }
inline vec3 Vec3_Abs(vec3 A)    { STORE_VEC3(A, _mm_and_ps(LOAD_VEC3(A), _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)))); return A; }
inline vec3 Vec3_Negate(vec3 A) { STORE_VEC3(A, _mm_xor_ps(LOAD_VEC3(A), _mm_set1_ps(-0.0f))); return A; }
inline vec3 Vec3_Min(vec3 A, vec3 B) { STORE_VEC3(A, _mm_min_ps(LOAD_VEC3(A), LOAD_VEC3(B))); return A; }
inline vec3 Vec3_Max(vec3 A, vec3 B) { STORE_VEC3(A, _mm_max_ps(LOAD_VEC3(A), LOAD_VEC3(B))); return A; }
inline vec3 Vec3_Clamp(vec3 A, vec3 min, vec3 max) { return Vec3_Min(Vec3_Max(A, min), max); }
inline vec3 Vec3_Modulo(vec3 N, vec3 D) { return Vec3_Sub(N, Vec3_Mul(D, Vec3_Floor(Vec3_Div(N, D)))); }

inline f32 Vec3_Sum(vec3 A)
{
    __m128 mA = LOAD_VEC3(A);
    mA = _mm_add_ps(mA, _mm_shuffle_ps(mA, mA, _MM_SHUFFLE(1,0,3,2)));
    mA = _mm_add_ss(mA, _mm_shuffle_ps(mA, mA, _MM_SHUFFLE(2,3,0,1)));
    return _mm_cvtss_f32(mA);
}

inline f32 Vec3_LengthSq(vec3 A)
{
    __m128 mA = LOAD_VEC3(A);
    mA = _mm_mul_ps(mA, mA);
    mA = _mm_add_ps(mA, _mm_shuffle_ps(mA, mA, _MM_SHUFFLE(1,0,3,2)));
    mA = _mm_add_ss(mA, _mm_shuffle_ps(mA, mA, _MM_SHUFFLE(2,3,0,1)));
    return _mm_cvtss_f32(mA);
}

inline f32 Vec3_Length(vec3 A)   { return F32_Sqrt(Vec3_LengthSq(A)); }

inline f32 Vec3_Dot(vec3 A, vec3 B)
{
    __m128 mA = LOAD_VEC3(A);
    __m128 mB = LOAD_VEC3(B);
    __m128 V = _mm_mul_ps(mA, mB);
    V = _mm_add_ps(V, _mm_shuffle_ps(V, V, _MM_SHUFFLE(1,0,3,2)));
    V = _mm_add_ss(V, _mm_shuffle_ps(V, V, _MM_SHUFFLE(2,3,0,1)));
    return _mm_cvtss_f32(V);
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
/* Generics */
/************/

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
    u32: U32_Min, \
    vec2: Vec2_Min, \
    vec3: Vec3_Min \
)(A,B)

#define Max(A,B) _Generic((A), \
    f32: F32_Max, \
    i32: I32_Max, \
    u32: U32_Max, \
    vec2: Vec2_Max, \
    vec3: Vec3_Max \
)(A,B)

#define Clamp(A,B,C) _Generic((A), \
    f32: F32_Clamp, \
    i32: I32_Clamp, \
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
