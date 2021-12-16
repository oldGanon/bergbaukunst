
#define AUDIO_SAMPLES_PER_SECOND 48000
#define AUDIO_SECOND_PER_SAMPLE (1.0f / AUDIO_SAMPLES_PER_SECOND)
#define AUDIO_SWEEP_UPDATES_PER_SECOND 100
#define AUDIO_FILTER_SIZE 256

typedef struct envelope_generator
{
    u32 Counter;
    u16 AttackVolume;
    u16 SustainVolume;
    u16 Attack;
    u16 Decay;
    u16 Sustain;
    u16 Release;
} envelope_generator;

typedef struct phase_generator
{
    u16 Phase;
    u16 Frequency;
    i16 dFrequency;
} phase_generator;

typedef enum wave_generator_type
{
    SINE_WAVE_GENERATOR,
    TRIANGLE_WAVE_GENERATOR,
    SAWTOOTH_WAVE_GENERATOR,
    SQUARE_WAVE_GENERATOR,
    WHITE_NOISE_GENERATOR,
    PINK_NOISE_GENERATOR,
    BROWN_NOISE_GENERATOR,
} wave_generator_type;

typedef struct square_wave_generator
{
    u16 Duty;
    i16 dDuty;
} square_wave_generator;

typedef struct white_noise_generator
{
    u16 Rng;
} white_noise_generator;

typedef struct pink_noise_generator
{
    u16 Counter;
    i16 State;
    u16 Rng;
    i16 Octaves[16];
} pink_noise_generator;

typedef struct brown_noise_generator
{
    i16 State;
    u16 Rng;
} brown_noise_generator;

typedef struct wave_generator
{
    u16 Type;

    union
    {
        square_wave_generator Square;
        white_noise_generator WhiteNoise;
        pink_noise_generator PinkNoise;
        brown_noise_generator BrownNoise;
    };
} wave_generator;

enum filter_type
{
    FILTER_NONE = 0,
    FILTER_LOWPASS,
};

typedef struct filter
{
    u16 Type;
    i16 Coefficients[AUDIO_FILTER_SIZE];
    i16 Samples[AUDIO_FILTER_SIZE];
} filter;

typedef struct sound
{
    envelope_generator Envelope;
    phase_generator Phase;
    wave_generator Wave;
    filter Filter;
} sound;

typedef struct sound_list
{
    sound Sound;

    struct sound_list *volatile Next;
} sound_list;

#define AUDIO_MAX_SOUNDS 256

typedef struct audio_state
{
    sound_list *volatile FirstUsedSound;
    sound_list *volatile FirstFreeSound;
    
    sound_list Sounds[AUDIO_MAX_SOUNDS];

    i16 WhiteNoise[65535];
    i16 PinkNoise[65535];
    i16 BrownNoise[65535];
} audio_state;

global audio_state GlobalAudioState;

envelope_generator Audio_EnvelopeGenerator(f32 AttackVolume, f32 SustainVolume, f32 Attack, f32 Decay, f32 Sustain, f32 Release)
{
    AttackVolume  = Saturate(AttackVolume)  * 0xFFFF;
    SustainVolume = Saturate(SustainVolume) * 0xFFFF;
    Attack        = Clamp(Attack,  0, 1) * AUDIO_SAMPLES_PER_SECOND;
    Decay         = Clamp(Decay,   0, 1) * AUDIO_SAMPLES_PER_SECOND;
    Sustain       = Clamp(Sustain, 0, 1) * AUDIO_SAMPLES_PER_SECOND;
    Release       = Clamp(Release, 0, 1) * AUDIO_SAMPLES_PER_SECOND;

    return (envelope_generator) {
        .AttackVolume  = (u16)AttackVolume,
        .SustainVolume = (u16)SustainVolume,
        .Attack        = (u16)Attack,
        .Decay         = (u16)Decay,
        .Sustain       = (u16)Sustain,
        .Release       = (u16)Release,
    };
}

u16 Audio_EnvelopeSample(envelope_generator *Envelope)
{
    i32 Counter = ++Envelope->Counter;

    if (Counter < Envelope->Attack)
    {
        Counter *= Envelope->AttackVolume;
        Counter /= Envelope->Attack;
        Counter += 0;
        return Counter & 0xFFFF;
    }

    Counter -= Envelope->Attack;
    if (Counter < Envelope->Decay)
    {
        Counter *= Envelope->SustainVolume - Envelope->AttackVolume;
        Counter /= Envelope->Decay;
        Counter += Envelope->AttackVolume;
        return Counter & 0xFFFF;
    }
    
    Counter -= Envelope->Decay;
    if (Counter < Envelope->Sustain)
    {
        Counter *= 0;
        Counter /= 1;
        Counter += Envelope->SustainVolume;
        return Counter & 0xFFFF;
    }

    Counter -= Envelope->Sustain;
    if (Counter < Envelope->Release)
    {
        Counter *= -Envelope->SustainVolume;
        Counter /= Envelope->Release;
        Counter += Envelope->SustainVolume;
        return Counter & 0xFFFF;
    }

    return 0;
}

phase_generator Audio_PhaseGenerator(f32 Frequency, f32 dFrequency)
{
    Frequency = Max(0.0f, Frequency);

    return (phase_generator) {
        .Frequency = (u16)(Frequency * (AUDIO_SECOND_PER_SAMPLE * 0xFFFF)),
        .dFrequency = (i16)(dFrequency * (AUDIO_SECOND_PER_SAMPLE * 0x7FFF)),
    };
}

phase_generator Audio_NoisePhaseGenerator(void)
{
    return (phase_generator) {
        .Frequency = 1,
        .dFrequency = 0,
    };
}

u16 Audio_PhaseSample(phase_generator *Phase)
{
    Phase->Frequency += Phase->dFrequency;
    Phase->Phase += Phase->Frequency;
    return Phase->Phase;
}

wave_generator Audio_SineWaveGenerator(void)
{
    return (wave_generator){ .Type = SINE_WAVE_GENERATOR };
}

i16 Audio_SineWaveSample(u16 Phase)
{
    u16 x = Phase;
    i16 s = ((i16)x >> 16);
    x &= 0x7FFF;

    if (x & 0x4000) x = 0x3FFF - x;

    x <<= 2;
    u16 x2 = (x * x) >> 16;

    u16 a = 51436; // 1.569778813
    u16 b = 20956; // 0.6395576276
    u16 c =  2286; // 0.06977881382

    u16 y = c;
    y = (y * x2) >> 16;
    y = b - y;
    y = (y * x2) >> 16;
    y = a - y;
    y = (y * x) >> 16;

    return y ^ s;
}

wave_generator Audio_TriangleWaveGenerator(void)
{
    return (wave_generator){ .Type = TRIANGLE_WAVE_GENERATOR };
}

i16 Audio_TriangleWaveSample(u16 Phase)
{
    if (Phase > 0x3FFF && Phase < 0xBFFF)
        return 0x7FFF - Phase;
    return Phase;
}

wave_generator Audio_SawtoothWaveGenerator(void)
{
    return (wave_generator){ .Type = SAWTOOTH_WAVE_GENERATOR };
}

i16 Audio_SawtoothWaveSample(u16 Phase)
{
    return Phase;
}

wave_generator Audio_SquareWaveGenerator(void)
{
    return (wave_generator){ .Type = SQUARE_WAVE_GENERATOR };
}

i16 Audio_SquareWaveSample(square_wave_generator *Wave, u16 Phase)
{
    Wave->Duty += Wave->dDuty;
    u16 Sample = 0x8000;
    if (Phase < Wave->Duty >> 1)
        Sample = 0x7FFF;
    return Sample;
}

wave_generator Audio_WhiteNoiseGenerator(void)
{
    return (wave_generator){ .Type = WHITE_NOISE_GENERATOR };
}

i16 Audio_WhiteNoiseSample(u16 Phase)
{
    return GlobalAudioState.WhiteNoise[Phase];
}

wave_generator Audio_PinkNoiseGenerator(void)
{
    return (wave_generator){ .Type = PINK_NOISE_GENERATOR };
}

i16 Audio_PinkNoiseSample(u16 Phase)
{
    return GlobalAudioState.PinkNoise[Phase];
}

wave_generator Audio_BrownNoiseGenerator(void)
{
    return (wave_generator){ .Type = BROWN_NOISE_GENERATOR };
}

i16 Audio_BrownNoiseSample(u16 Phase)
{
    return GlobalAudioState.BrownNoise[Phase];
}

i16 Audio_WaveSample(wave_generator *Wave, u16 Phase)
{
    switch (Wave->Type)
    {
        case SINE_WAVE_GENERATOR:     return Audio_SineWaveSample(Phase); break;
        case TRIANGLE_WAVE_GENERATOR: return Audio_TriangleWaveSample(Phase); break;
        case SAWTOOTH_WAVE_GENERATOR: return Audio_SawtoothWaveSample(Phase); break;
        case SQUARE_WAVE_GENERATOR:   return Audio_SquareWaveSample(&Wave->Square, Phase); break;
        case WHITE_NOISE_GENERATOR:   return Audio_WhiteNoiseSample(Phase); break;
        case PINK_NOISE_GENERATOR:    return Audio_PinkNoiseSample(Phase); break;
        case BROWN_NOISE_GENERATOR:   return Audio_BrownNoiseSample(Phase); break;
    }
    return 0;
}

f32 Sinc(f32 x)
{
    if (x == 0.0f) return 1.0f;
    return Sin(x) / x;
}

filter Audio_BandpassFilter(f32 MinCutOff, f32 MaxCutOff)
{
    filter Filter = { 0 };

    f32 MinCutOffSamples = MinCutOff / AUDIO_SAMPLES_PER_SECOND;
    f32 MaxCutOffSamples = MaxCutOff / AUDIO_SAMPLES_PER_SECOND;
    for (u32 n = 0; n < AUDIO_FILTER_SIZE; ++n)
    {
        u32 x = n + 1;
        f32 Window = 0.42f - 0.5f * Cos(x * (MATH_PI / AUDIO_FILTER_SIZE)) + 0.08f * Cos(x * (2.0f * MATH_PI / AUDIO_FILTER_SIZE));
        f32 Max = MaxCutOffSamples * Sinc((AUDIO_FILTER_SIZE - x) * 2.0f * MATH_PI * MaxCutOffSamples);
        f32 Min = MinCutOffSamples * Sinc((AUDIO_FILTER_SIZE - x) * 2.0f * MATH_PI * MinCutOffSamples);
        Filter.Coefficients[n] = (u16)(Window * (Max - Min) * 0xFFFF);
    }

    return Filter;
}

filter Audio_LowpassFilter(f32 CutOff)
{
    filter Filter = { .Type = FILTER_LOWPASS };

    f32 CutOffSamples = CutOff / AUDIO_SAMPLES_PER_SECOND;
    for (u32 n = 0; n < AUDIO_FILTER_SIZE; ++n)
    {
        u32 x = n + 1;
        f32 Window = 0.42f - 0.5f * Cos(x * (MATH_PI / AUDIO_FILTER_SIZE)) + 0.08f * Cos(x * (2.0f * MATH_PI / AUDIO_FILTER_SIZE));
        f32 LowPass = CutOffSamples * Sinc((AUDIO_FILTER_SIZE - x) * 2.0f * MATH_PI * CutOffSamples);
        Filter.Coefficients[n] = (i16)((Window * LowPass) * 0xFFFF);
    }

    return Filter;
}

i16 Audio_Filter(filter *Filter, i16 Sample)
{
    if (Filter->Type == FILTER_NONE)
        return Sample;

    for (u32 i = 0; i < AUDIO_FILTER_SIZE - 1; ++i)
        Filter->Samples[i] = Filter->Samples[i + 1];
    Filter->Samples[AUDIO_FILTER_SIZE - 1] = Sample;

    i16 Result = 0;
    for (u32 i = 0; i < AUDIO_FILTER_SIZE; ++i)
        Result += (Filter->Coefficients[i] * Filter->Samples[i]) >> 16;
    return Result;
}

void Audio_WriteSamples(i16 *SampleBuffer, u32 SampleCount)
{
    for (u32 i = 0; i < SampleCount; ++i)
    {
        i16 Sample = 0;

        for (sound_list *SoundList = GlobalAudioState.FirstUsedSound; SoundList; SoundList = SoundList->Next)
        {
            sound *Sound = &SoundList->Sound;

            u16 Envelope = Audio_EnvelopeSample(&Sound->Envelope);
            u16 Phase = Audio_PhaseSample(&Sound->Phase);
            i16 Wave = Audio_WaveSample(&Sound->Wave, Phase);
            i16 Filtered = Audio_Filter(&Sound->Filter, Wave);
            Sample += (Envelope * Filtered) >> 16;
        }
        *SampleBuffer++ = Sample;
        *SampleBuffer++ = Sample;
    }
}

void Audio_PlaySound(sound Sound)
{
    if (!GlobalAudioState.FirstFreeSound) return;

    sound_list *NewSound;
    sound_list *NextFreeSound;
    sound_list *FirstUsedSound;

    do
    {
        NewSound = GlobalAudioState.FirstFreeSound;
        NextFreeSound = NewSound->Next;
    } while (InterlockedCompareExchangePointer(&GlobalAudioState.FirstFreeSound, NextFreeSound, NewSound) != NewSound);

    NewSound->Sound = Sound;

    do
    {
        FirstUsedSound = GlobalAudioState.FirstUsedSound;
        NewSound->Next = FirstUsedSound;
    } while (InterlockedCompareExchangePointer(&GlobalAudioState.FirstUsedSound, NewSound, FirstUsedSound) != FirstUsedSound);
}

inline u16 xorshift16_Step(u16 x)
{
    x ^= x << 7;
    x ^= x >> 9;
    x ^= x << 8;
    return x;
}

void Audio_GenerateWhiteNoise(i16 *SampleBuffer, u32 SampleCount)
{
    u16 Rng = 0x119E;

    while (SampleCount--)
    {
        Rng = xorshift16_Step(Rng);
        i16 Noise = (i16)Rng;
        *SampleBuffer++ = Noise;
    }
}

void Audio_GeneratePinkNoise(i16 *SampleBuffer, u32 SampleCount)
{
    u16 Counter = 0;
    i16 State = 0;
    u16 Rng0 = 0x119E;
    u16 Rng1 =  0xEE61;
    i16 Octaves[16] = { 0 };

    while (SampleCount--)
    {
        Rng0 = xorshift16_Step(Rng0);
        i16 Noise = ((i16)Rng0) >> 4;

        Rng1 = xorshift16_Step(Rng1);
        i16 OctaveNoise = ((i16)Rng1) >> 4;

        DWORD k = 0;
        if (_BitScanForward(&k, ++Counter))
        {
            State -= Octaves[k];
            State += OctaveNoise;
            Octaves[k] = OctaveNoise;
        }

        i16 Sample = State + Noise;
        assert((i32)State + (i32)Noise <= 32767);
        assert((i32)State + (i32)Noise >= -32768);
        *SampleBuffer++ = Sample;
    }
}

void Audio_GenerateBrownNoise(i16 *SampleBuffer, u32 SampleCount)
{
    i16 State = 0;
    u16 Rng = 0x119E;

    while (SampleCount--)
    {
        Rng = xorshift16_Step(Rng);
        i16 Noise = Rng & 0x80;
        if (Noise) State += 0x7F;
        else       State -= 0x7F;
        i16 Sample = State;
        *SampleBuffer++ = Sample;
    }
}

void Audio_Init(void)
{
    GlobalAudioState.FirstUsedSound = 0;
    GlobalAudioState.FirstFreeSound = &GlobalAudioState.Sounds[0];
    for (u32 i = 0; i < AUDIO_MAX_SOUNDS - 1; ++i)
        GlobalAudioState.Sounds[i].Next = &GlobalAudioState.Sounds[i+1];
    GlobalAudioState.Sounds[AUDIO_MAX_SOUNDS-1].Next = 0;

    Audio_GenerateWhiteNoise(GlobalAudioState.WhiteNoise, 65535);
    Audio_GeneratePinkNoise(GlobalAudioState.PinkNoise, 65535);
    Audio_GenerateBrownNoise(GlobalAudioState.BrownNoise, 65535);
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