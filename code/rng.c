
typedef struct rng { u64 State[4]; } rng;

inline u64 Rng_U64(rng *RNG);
inline f32 Rng_F32(rng *RNG);
inline f64 Rng_F64(rng *RNG);
inline vec2 Rng_Vec2(rng *RNG);
rng Rng_Init(u64 Seed);
rng Rng_Jump(rng Old);

/******************/
/* IMPLEMENTATION */
/******************/

u64 Rng_U64(rng *RNG)
{
    // xoshiro256**
    const u64 R = _rotl64(RNG->State[1] * 5, 7) * 9;
    const u64 T = RNG->State[1] << 17;
    RNG->State[2] ^= RNG->State[0];
    RNG->State[3] ^= RNG->State[1];
    RNG->State[1] ^= RNG->State[2];
    RNG->State[0] ^= RNG->State[3];
    RNG->State[2] ^= T;
    RNG->State[3] = _rotl64(RNG->State[3], 45);
    return R;
}

inline f32 Rng_F32(rng *RNG)
{
    return  Hash_toF32(Rng_U64(RNG));
}

inline f64 Rng_F64(rng *RNG)
{
    return  Hash_toF64(Rng_U64(RNG));
}

inline vec2 Rng_Vec2(rng *RNG)
{
    return  Hash_toVec2(Rng_U64(RNG));
}

rng Rng_Init(u64 Seed)
{
    struct rng RNG;

    for (u32 i = 0; i < 4; ++i)
    {
        // splitmix64
        Seed += 0x9e3779b97f4a7c15U;
        u64 z = Seed;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9U;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebU;
        z = (z ^ (z >> 31));
        RNG.State[i] = z;
    }

    return RNG;
}

rng Rng_Jump(rng Old)
{
    static const u64 Jump[4] = { 0x180ec6d33cfd0abaU,
                                 0xd5a61266f0c9392cU,
                                 0xa9582618e03fc9aaU,
                                 0x39abdc4529b1661cU };
    rng New = { 0 };
    for(u32 i = 0; i < 4; i++)
    for(u32 b = 0; b < 64; b++)
    {
        if (Jump[i] & (1ULL << b))
        {
            New.State[0] ^= Old.State[0];
            New.State[1] ^= Old.State[1];
            New.State[2] ^= Old.State[2];
            New.State[3] ^= Old.State[3];
        }
        Rng_U64(&Old);
    }
    return New;
}
