
#include <intrin.h>

typedef union
{
    struct { f32 x, y; };
    struct { f32 u, v; };
} vec2;

typedef union
{
    alignas(16) f32 E[4];
    struct { f32 x, y, z; };
    struct { f32 r, g, b; };
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



inline vec3 Vec3_Add(vec3 A, vec3 B) { _mm_store_ps(A.E, _mm_add_ps(_mm_load_ps(A.E), _mm_load_ps(B.E))); return A; }
inline vec3 Vec3_Sub(vec3 A, vec3 B) { _mm_store_ps(A.E, _mm_sub_ps(_mm_load_ps(A.E), _mm_load_ps(B.E))); return A; }
inline vec3 Vec3_Mul(vec3 A, vec3 B) { _mm_store_ps(A.E, _mm_mul_ps(_mm_load_ps(A.E), _mm_load_ps(B.E))); return A; }
inline vec3 Vec3_Div(vec3 A, vec3 B) { _mm_store_ps(A.E, _mm_div_ps(_mm_load_ps(A.E), _mm_load_ps(B.E))); return A; }
