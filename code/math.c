
#include <intrin.h>

typedef union
{
    struct { f32 x, y; };
    struct { f32 u, v; };
    f32 E[2];
} vec2;

typedef union
{
    struct { f32 x, y, z; };
    struct { f32 r, g, b; };
    f32 E[4];
} vec3;

#define MATH_PI 3.14159265358979323846f

#define ABS(A) (((A)<(0))?(-A):(A))
#define SIGN(A) (((A)<(0))?(-1):(1))
#define NEGATE(A) (-(A))
#define MIN(A,B) (((A)<(B))?(A):(B))
#define MAX(A,B) (((A)>(B))?(A):(B))
#define CLAMP(A,B,C) MIN(MAX(A,B),C)



inline f32 Round(f32 n) { return _mm_cvtss_f32(_mm_round_ps(_mm_set_ss(n), _MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC)); }
inline f32 Trunc(f32 n) { return _mm_cvtss_f32(_mm_round_ps(_mm_set_ss(n), _MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC)); }
inline f32 Ceil(f32 n)  { return _mm_cvtss_f32(_mm_ceil_ps(_mm_set_ss(n))); }
inline f32 Floor(f32 n) { return _mm_cvtss_f32(_mm_floor_ps(_mm_set_ss(n))); }

inline i32 Round_toInt(f32 n) { return _mm_cvt_ss2si(_mm_round_ps(_mm_set_ss(n), _MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC)); }
inline i32 Trunc_toInt(f32 n) { return _mm_cvt_ss2si(_mm_round_ps(_mm_set_ss(n), _MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC)); }
inline i32 Ceil_toInt(f32 n)  { return _mm_cvt_ss2si(_mm_ceil_ps(_mm_set_ss(n))); }
inline i32 Floor_toInt(f32 n) { return _mm_cvt_ss2si(_mm_floor_ps(_mm_set_ss(n))); }
inline i32 Float_toInt(f32 n) { return _mm_cvt_ss2si(_mm_set_ss(n)); }

inline f32 Abs(f32 n)    { return _mm_cvtss_f32(_mm_and_ps(_mm_set_ss(n), _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)))); }
inline f32 Sign(f32 n)   { return _mm_cvtss_f32(_mm_or_ps(_mm_and_ps(_mm_set_ss(n), _mm_set_ss(-0.0f)), _mm_set_ss(1.0f))); }
inline f32 Negate(f32 n) { return _mm_cvtss_f32(_mm_xor_ps(_mm_set_ss(n), _mm_set_ss(-0.0f))); }
inline f32 Min(f32 A, f32 B) { return _mm_cvtss_f32(_mm_min_ps(_mm_set_ss(A), _mm_set_ss(B))); }
inline f32 Max(f32 A, f32 B) { return _mm_cvtss_f32(_mm_max_ps(_mm_set_ss(A), _mm_set_ss(B))); }
inline f32 Sqrt(f32 n) { return _mm_cvtss_f32(_mm_sqrt_ps(_mm_set_ss(n)));}
inline f32 Clamp(f32 A, f32 min, f32 max) { return Min(Max(A, min), max); }
inline f32 Lerp(f32 A, f32 B, f32 t) { return (1.0f-t)*A + t*B; }
inline f32 Fract(f32 n) { return n - Floor(n); }
inline f32 Modulo(f32 n, f32 d) { return n - d * Floor(n / d); }



f32 Sin(f32 x)
{
    x = x * (1.0f / MATH_PI) + 1.0f;
    x = Modulo(x, 2.0f) - 1.0f;
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
    x = Modulo(x, 2.0f) - 1.0f;
    f32 x2 = x * x;
    f32 x3 =       0.000385937753182769f;
    x3 = x3 * x2 - 0.006860187425683514f;
    x3 = x3 * x2 + 0.0751872634325299f;
    x3 = x3 * x2 - 0.5240361513980939f;
    x3 = x3 * x2 + 2.0261194642649887f;
    x3 = x3 * x2 - 3.1415926444234477f;
    return (x - 1.0f) * (x + 1.0f) * x3 * x;
}

#define LOAD_VEC2(V) _mm_castpd_ps(_mm_load_sd((f64*)&V.E[0]))
#define STORE_VEC2(V,M) _mm_store_sd((f64*)V.E,_mm_castps_pd(M))
inline vec2 Vec2_Add(vec2 A, vec2 B) { STORE_VEC2(A, _mm_add_ps(LOAD_VEC2(A), LOAD_VEC2(B))); return A; }
inline vec2 Vec2_Sub(vec2 A, vec2 B) { STORE_VEC2(A, _mm_sub_ps(LOAD_VEC2(A), LOAD_VEC2(B))); return A; }
inline vec2 Vec2_Mul(vec2 A, vec2 B) { STORE_VEC2(A, _mm_mul_ps(LOAD_VEC2(A), LOAD_VEC2(B))); return A; }
inline vec2 Vec2_Div(vec2 A, vec2 B) { STORE_VEC2(A, _mm_div_ps(LOAD_VEC2(A), LOAD_VEC2(B))); return A; }

#define LOAD_VEC3(V) _mm_loadu_ps(V.E)
#define STORE_VEC3(V,M) _mm_storeu_ps(V.E,M)
inline vec3 Vec3_Add(vec3 A, vec3 B) { STORE_VEC3(A, _mm_add_ps(LOAD_VEC3(A), LOAD_VEC3(B))); return A; }
inline vec3 Vec3_Sub(vec3 A, vec3 B) { STORE_VEC3(A, _mm_sub_ps(LOAD_VEC3(A), LOAD_VEC3(B))); return A; }
inline vec3 Vec3_Mul(vec3 A, vec3 B) { STORE_VEC3(A, _mm_mul_ps(LOAD_VEC3(A), LOAD_VEC3(B))); return A; }
inline vec3 Vec3_Div(vec3 A, vec3 B) { STORE_VEC3(A, _mm_div_ps(LOAD_VEC3(A), LOAD_VEC3(B))); return A; }

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
