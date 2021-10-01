
inline u32 Hash_U32(u32 x);
inline u32 Hash_U32Inverse(u32 x);
inline u32 Hash_U32Seeded(u64 Seed, u32 x);
inline u64 Hash_U64(u64 x);
inline u64 Hash_U64Inverse(u64 x);
inline u64 Hash_U64Seeded(u64 Seed, u64 x);
inline u64 Hash_IVec2(ivec2 v);
inline u64 Hash_IVec2Seeded(u64 Seed, ivec2 v);
inline u64 Hash_Bytes(const void *Data, size Length);
inline f32 Hash_toF32(u64 Hash);
inline f64 Hash_toF64(u64 Hash);
inline vec2 Hash_toVec2(u64 Hash);

/******************/
/* IMPLEMENTATION */
/******************/

inline u32 Hash_U32(u32 x)
{
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

inline u32 Hash_U32Inverse(u32 x)
{
    x ^= x >> 16;
    x *= 0x43021123U;
    x ^= x >> 15 ^ x >> 30;
    x *= 0x1d69e2a5U;
    x ^= x >> 16;
    return x;
}

inline u32 Hash_U32Seeded(u64 Seed, u32 x)
{
    x ^= Seed >> 32;
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

inline u64 Hash_U64(u64 x)
{
    x ^= x >> 30;
    x *= 0xbf58476d1ce4e5b9U;
    x ^= x >> 27;
    x *= 0x94d049bb133111ebU;
    x ^= x >> 31;
    return x;
}

inline u64 Hash_U64Inverse(u64 x)
{
    x ^= x >> 31 ^ x >> 62;
    x *= 0x319642b2d24d8ec3U;
    x ^= x >> 27 ^ x >> 54;
    x *= 0x96de1b173f119089U;
    x ^= x >> 30 ^ x >> 60;
    return x;
}

inline u64 Hash_U64Seeded(u64 Seed, u64 x)
{
    x ^= Seed;
    x ^= x >> 30;
    x *= 0xbf58476d1ce4e5b9U;
    x ^= x >> 27;
    x *= 0x94d049bb133111ebU;
    x ^= x >> 31;
    return x;
}

inline u64 Hash_IVec2(ivec2 v)
{
    u64 x = ((u64)v.x << 32) | ((u64)v.y & 0xFFFFFFFFULL);
    return Hash_U64(x);
}

inline u64 Hash_IVec2Seeded(u64 Seed, ivec2 v)
{
    u64 x = (((u64)v.x << 32) | ((u64)v.y & 0xFFFFFFFFULL)) ^ Seed;
    return Hash_U64(x);
}

inline u64 Hash_Bytes(const void *Data, size Length)
{
    // FNV1a
    u64 Hash = 14695981039346656037U;
    const u8 *Bytes = Data;
    while (Length--) Hash = (Hash ^ *Bytes++) * 1099511628211U;
    return Hash;
}

inline f32 Hash_toF32(u64 Hash)
{
    const union { u32 i; f32 f; } u = { .i = (0x7FULL << 23) | (Hash >> 41) };
    return u.f - 1.0f;
}

inline f64 Hash_toF64(u64 Hash)
{
    const union { u64 i; f64 f; } u = { .i = (0x3FFULL << 52) | (Hash >> 12) };
    return u.f - 1.0;
}

inline vec2 Hash_toVec2(u64 Hash)
{
    return (vec2){ .x = Hash_toF32(Hash), .y = Hash_toF32(Hash << 32) };
}
