
typedef struct envelope
{
    u16 Attack;
    u16 Decay;
    u16 Sustain;
    u16 Release;
    u16 AttackVolume;
    u16 SustainVolume;
} envelope;

typedef enum generator_type
{
    SINE_GENERATOR,
    SAWTOOTH_GENERATOR,
    TRIANGLE_GENERATOR,
    SQUARE_GENERATOR,
    NOISE_GENERATOR,
} generator_type;

typedef struct generator
{
    u16 Type;

    u16 FreqStart;
    i16 FreqSlide;

    u16 DutyStart;
    i16 DutySlide;
} generator;

typedef struct sound
{
    envelope Envelope;
    generator Generator;
} sound;

typedef struct sine_wave_generator
{
    f32 Frequency;
    f32 Sin;
    f32 Cos;
    f32 Alpha;
    f32 Beta;
} sine_wave_generator;

typedef struct pink_noise_generator
{
    u16 Counter;
    i16 State;
    u16 Rng;
    i16 Octaves[16];
} pink_noise_generator;

global sine_wave_generator SineWave;
global pink_noise_generator PinkNoise;

#define AUDIO_SAMPLES_PER_SECOND 48000

sine_wave_generator Audio_SineWave(f32 Frequency)
{
    f32 Alpha = Sin((    MATH_PI / AUDIO_SAMPLES_PER_SECOND) * Frequency);
    f32 Beta  = Sin((2 * MATH_PI / AUDIO_SAMPLES_PER_SECOND) * Frequency);
    Alpha = 2 * Alpha * Alpha;

    return (sine_wave_generator){
        .Frequency = Frequency,
        .Sin = 0,
        .Cos = 1,
        .Alpha = Alpha,
        .Beta = Beta,
    };
}

void Audio_SineWaveSamples(sine_wave_generator *Generator, i16 *SampleBuffer, u32 SampleCount)
{
    for (u32 i = 0; i < SampleCount; ++i)
    {
        f32 dSin = (Generator->Alpha * Generator->Sin) - (Generator->Beta * Generator->Cos);
        f32 dCos = (Generator->Alpha * Generator->Cos) + (Generator->Beta * Generator->Sin);
        Generator->Sin -= dSin;
        Generator->Cos -= dCos;

        i16 Sample = (i16)(Clamp(Generator->Sin*32767.0f,-32767.0f,32768.0f));
        *SampleBuffer++ = Sample;
        *SampleBuffer++ = Sample;
    }
}

inline u16 xorshift16_Step(u16 x)
{
    x ^= x << 7;
    x ^= x >> 9;
    x ^= x << 8;
    return x;
}

pink_noise_generator Audio_PinkNoise(void)
{
    return  (pink_noise_generator){ .Rng = __rdtsc() & 0xFFFF };
}

void Audio_PinkNoiseSamples(pink_noise_generator *Generator, i16 *SampleBuffer, u32 SampleCount)
{
    for (u64 i = 0; i < SampleCount; ++i)
    {
        Generator->Rng = xorshift16_Step(Generator->Rng);
        i16 Noise = ((i16)Generator->Rng) >> 4;

        Generator->Rng = xorshift16_Step(Generator->Rng);
        i16 OctaveNoise = ((i16)Generator->Rng) >> 4;

        DWORD k = 0;
        Generator->Counter += 2;
        if (_BitScanForward(&k, Generator->Counter))
        {
            Generator->State -= Generator->Octaves[k];
            Generator->State += OctaveNoise;
            Generator->Octaves[k] = OctaveNoise;
        }

        i16 Sample = Generator->State + Noise;
        assert((i32)Generator->State + (i32)Noise <= 32767);
        assert((i32)Generator->State + (i32)Noise >= -32768);
        *SampleBuffer++ = Sample;
        *SampleBuffer++ = Sample;
    }
}

void Audio_WriteSamples(i16 *SampleBuffer, u32 SampleCount)
{
    // Audio_SineWaveSamples(&SineWave, SampleBuffer, SampleCount);
    // Audio_PinkNoiseSamples(&PinkNoise, SampleBuffer, SampleCount);
}

void Audio_Init(void)
{
    SineWave = Audio_SineWave(400.0f);
    PinkNoise = Audio_PinkNoise();
}

/*
inline __m256i xorshift16x16_Init(u64 x)
{
    u64 Init[4];
    for (u32 i = 0; i < 4; ++i)
    {
        // splitmix64
        Seed += 0x9e3779b97f4a7c15U;
        u64 z = Seed;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9U;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebU;
        z = (z ^ (z >> 31));
        Init[i] = z;
    }
    return _mm256_loadu_si256(Init);
}

inline __m256i xorshift16x16_Step(__m256i x)
{
    x = _mm256_xor_si128(x, _mm256_slli_epi16(x, 7));
    x = _mm256_xor_si128(x, _mm256_srli_epi16(x, 9));
    x = _mm256_xor_si128(x, _mm256_slli_epi16(x, 8));
    return x;
}

u64 Seed = __rdtsc();
__m256i Rng0 = xorshift16x16_Init(Seed);
__m256i Rng1 = xorshift16x16_Init(~Seed);
for ()
{
    Rng0 = xorshift16x16_Step(Rng0);
    Rng1 = xorshift16x16_Step(Rng1);


}
*/