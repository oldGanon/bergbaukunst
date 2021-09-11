
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

inline u64 Hash_Bytes(const void *Data, size Length)
{
    // FNV1a
    u64 Hash = 14695981039346656037ULL;
    const u8 *Bytes = Data;
    while (Length--) Hash = (Hash ^ *Bytes++) * 1099511628211ULL;
    return Hash;
}
