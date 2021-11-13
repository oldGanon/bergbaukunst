
#define AUDIO_SAMPLES_PER_SECOND 48000
#define AUDIO_SECOND_PER_SAMPLE (1.0f / AUDIO_SAMPLES_PER_SECOND)
#define AUDIO_SWEEP_UPDATES_PER_SECOND 100

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
    u16 dFrequency;
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

typedef struct sound
{
    envelope_generator Envelope;
    phase_generator Phase;
    wave_generator Wave;
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
} audio_state;

global audio_state GlobalAudioState;

#if 0

generator Audio_SineWaveGenerator(f32 Frequency, f32 dFrequency)
{
    f32 Alpha = Sin((    MATH_PI / AUDIO_SAMPLES_PER_SECOND) * Frequency);
    f32 Beta  = Sin((2 * MATH_PI / AUDIO_SAMPLES_PER_SECOND) * Frequency);
    Alpha = 2 * Alpha * Alpha;

    return (generator){
        .Type = SINE_WAVE_GENERATOR,
        .SineWave = {
            .Frequency = Clamp(Frequency, 20.0f, 20000.0f),
            .dFrequency = dFrequency,
            .Sin = 0,
            .Cos = 1,
            .Alpha = Alpha,
            .Beta = Beta,
        }
    };
}


void Audio_SineWaveSamples(sine_wave_generator *Generator, i16 *SampleBuffer, u32 SampleCount)
{
    while (SampleCount--)
    {
        if ((Generator->dFrequency != 0.0f) &&
            (Generator->Counter++ == AUDIO_SAMPLES_PER_SECOND / AUDIO_SWEEP_UPDATES_PER_SECOND))
        {
            Generator->Counter = 0;
            Generator->Frequency += Generator->dFrequency * (1.0f / AUDIO_SWEEP_UPDATES_PER_SECOND);
            Generator->Frequency = Clamp(Generator->Frequency, 20.0f, 20000.0f);
            
            Generator->Alpha = Sin((    MATH_PI / AUDIO_SAMPLES_PER_SECOND) * Generator->Frequency);
            Generator->Beta  = Sin((2 * MATH_PI / AUDIO_SAMPLES_PER_SECOND) * Generator->Frequency);
            Generator->Alpha = 2 * Generator->Alpha * Generator->Alpha;
        }

        f32 dSin = (Generator->Alpha * Generator->Sin) - (Generator->Beta * Generator->Cos);
        f32 dCos = (Generator->Alpha * Generator->Cos) + (Generator->Beta * Generator->Sin);
        Generator->Sin -= dSin;
        Generator->Cos -= dCos;

        i16 Sample = (i16)(Clamp(Generator->Sin*32767.0f,-32768.0f,32767.0f));
        *SampleBuffer++ = Sample;
    }
}

generator Audio_TriangleWaveGenerator(f32 Frequency, f32 dFrequency)
{
    Frequency = Clamp(Frequency, 20.0f, 20000.0f);
    u16 dState = (u16)(Frequency * (65535.0f / AUDIO_SAMPLES_PER_SECOND));

    return (generator){
        .Type = TRIANGLE_WAVE_GENERATOR,
        .TriangleWave = {
            .Frequency = Frequency,
            .dFrequency = dFrequency,
            .State = 0,
            .dState = dState,
        }
    };
}

void Audio_TriangleWaveSamples(triangle_wave_generator *Generator, i16 *SampleBuffer, u32 SampleCount)
{
    while (SampleCount--)
    {
        if ((Generator->dFrequency != 0.0f) && 
            (Generator->Counter++ == AUDIO_SAMPLES_PER_SECOND / AUDIO_SWEEP_UPDATES_PER_SECOND))
        {
            Generator->Counter = 0;
            Generator->Frequency += Generator->dFrequency * (1.0f / AUDIO_SWEEP_UPDATES_PER_SECOND);
            Generator->Frequency = Clamp(Generator->Frequency, 20.0f, 20000.0f);
            
            Generator->dState = (u16)(Generator->Frequency * (65535.0f / AUDIO_SAMPLES_PER_SECOND));
        }

        Generator->State += Generator->dState;
        i16 Sample = Generator->State;
        if (Generator->State > 0x3FFF && Generator->State < 0xBFFF)
            Sample = 0x7FFF - Sample;
        *SampleBuffer++ = Sample;
    }
}

generator Audio_SawtoothWaveGenerator(f32 Frequency, f32 dFrequency)
{
    Frequency = Clamp(Frequency, 20.0f, 20000.0f);
    u16 dState = (u16)(Frequency * (65535.0f / AUDIO_SAMPLES_PER_SECOND));

    return (generator){
        .Type = SAWTOOTH_WAVE_GENERATOR,
        .SawtoothWave = {
            .Frequency = Frequency,
            .dFrequency = dFrequency,
            .State = 0,
            .dState = dState,
        }
    };
}

void Audio_SawtoothWaveSamples(sawtooth_wave_generator *Generator, i16 *SampleBuffer, u32 SampleCount)
{
    while (SampleCount--)
    {
        if ((Generator->dFrequency != 0.0f) &&
            (Generator->Counter++ == AUDIO_SAMPLES_PER_SECOND / AUDIO_SWEEP_UPDATES_PER_SECOND))
        {
            Generator->Counter = 0;
            Generator->Frequency += Generator->dFrequency * (1.0f / AUDIO_SWEEP_UPDATES_PER_SECOND);
            Generator->Frequency = Clamp(Generator->Frequency, 20.0f, 20000.0f);
            
            Generator->dState = (u16)(Generator->Frequency * (65535.0f / AUDIO_SAMPLES_PER_SECOND));
        }

        Generator->State += Generator->dState;
        i16 Sample = Generator->State;
        *SampleBuffer++ = Sample;
    }
}

generator Audio_SquareWaveGenerator(f32 Frequency, f32 dFrequency, f32 Duty, f32 dDuty)
{
    Frequency = Clamp(Frequency, 20.0f, 20000.0f);
    Duty = Clamp(Duty, 0.0f, 1.0f);
    u16 dState = (u16)(Frequency * (65535.0f / AUDIO_SAMPLES_PER_SECOND));

    return (generator){
        .Type = SQUARE_WAVE_GENERATOR,
        .SquareWave = {
            .Frequency = Frequency,
            .dFrequency = dFrequency,
            .State = 0,
            .dState = dState,
            .Duty = (u16)(0x4000 * Duty),
            .dDuty = (i16)(0x4000 * dDuty),
        }
    };
}

void Audio_SquareWaveSamples(square_wave_generator *Generator, i16 *SampleBuffer, u32 SampleCount)
{
    while (SampleCount--)
    {
        if ((Generator->dFrequency != 0.0f) &&
            (Generator->Counter++ == AUDIO_SAMPLES_PER_SECOND / AUDIO_SWEEP_UPDATES_PER_SECOND))
        {
            Generator->Counter = 0;
            Generator->Frequency += Generator->dFrequency * (1.0f / AUDIO_SWEEP_UPDATES_PER_SECOND);
            Generator->Frequency = Clamp(Generator->Frequency, 20.0f, 20000.0f);
            
            Generator->dState = (u16)(Generator->Frequency * (65535.0f / AUDIO_SAMPLES_PER_SECOND));

            Generator->Duty += (u16)(Generator->dDuty * (1.0f / AUDIO_SWEEP_UPDATES_PER_SECOND));
            Generator->Duty = Clamp(Generator->Duty, 0, 0x4000);
        }

        Generator->State += Generator->dState;
        u16 Sample = 0x8000;
        if (Generator->State < Generator->Duty)
            Sample = 0x7FFF;
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

generator Audio_WhiteNoiseGenerator(void)
{
    u16 Rng = __rdtsc() & 0xFFFF;
    return (generator) {
        .Type = WHITE_NOISE_GENERATOR,
        .WhiteNoise = {
            .Rng = Rng ? Rng : 1
        }
    };
}

void Audio_WhiteNoiseSamples(white_noise_generator *Generator, i16 *SampleBuffer, u32 SampleCount)
{
    while (SampleCount--)
    {
        Generator->Rng = xorshift16_Step(Generator->Rng);
        i16 Noise = (i16)Generator->Rng;
        *SampleBuffer++ = Noise;
    }
}

generator Audio_PinkNoiseGenerator(void)
{
    u16 Rng = __rdtsc() & 0xFFFF;
    return (generator) {
        .Type = PINK_NOISE_GENERATOR,
        .PinkNoise = {
            .Rng = Rng ? Rng : 1
        }
    };
}

void Audio_PinkNoiseSamples(pink_noise_generator *Generator, i16 *SampleBuffer, u32 SampleCount)
{
    while (SampleCount--)
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
    }
}

generator Audio_BrownNoiseGenerator(void)
{
    u16 Rng = __rdtsc() & 0xFFFF;
    return (generator) {
        .Type = BROWN_NOISE_GENERATOR,
        .BrownNoise = {
            .Rng = Rng ? Rng : 1
        }
    };
}

void Audio_BrownNoiseSamples(brown_noise_generator *Generator, i16 *SampleBuffer, u32 SampleCount)
{
    while (SampleCount--)
    {
        Generator->Rng = xorshift16_Step(Generator->Rng);
        i16 Noise = Generator->Rng & 0x80;
        if (Noise) Generator->State += 0x7F;
        else       Generator->State -= 0x7F;
        i16 Sample = Generator->State;
        *SampleBuffer++ = Sample;
    }
}

void Audio_GeneratorSamples(generator *Generator, i16 *SampleBuffer, u32 SampleCount)
{
    switch (Generator->Type)
    {
        case SINE_WAVE_GENERATOR:
        {
            Audio_SineWaveSamples(&Generator->SineWave, SampleBuffer, SampleCount);
        } break;

        case TRIANGLE_WAVE_GENERATOR:
        {
            Audio_TriangleWaveSamples(&Generator->TriangleWave, SampleBuffer, SampleCount);
        } break;

        case SAWTOOTH_WAVE_GENERATOR:
        {
            Audio_SawtoothWaveSamples(&Generator->SawtoothWave, SampleBuffer, SampleCount);
        } break;

        case SQUARE_WAVE_GENERATOR:
        {
            Audio_SquareWaveSamples(&Generator->SquareWave, SampleBuffer, SampleCount);
        } break;

        case WHITE_NOISE_GENERATOR:
        {
            Audio_WhiteNoiseSamples(&Generator->WhiteNoise, SampleBuffer, SampleCount);
        } break;

        case PINK_NOISE_GENERATOR:
        {
            Audio_PinkNoiseSamples(&Generator->PinkNoise, SampleBuffer, SampleCount);
        } break;

        case BROWN_NOISE_GENERATOR:
        {
            Audio_BrownNoiseSamples(&Generator->BrownNoise, SampleBuffer, SampleCount);
        } break;
    }
}

envelope Audio_Envelope(f32 AttackVolume, f32 SustainVolume, f32 Attack, f32 Decay, f32 Sustain, f32 Release)
{
    return (envelope) {
        .AttackVolume  = (u16)(Clamp(AttackVolume,  0, 1) * 0xFFFF),
        .SustainVolume = (u16)(Clamp(SustainVolume, 0, 1) * 0xFFFF),
        .Attack        = (u16)(Clamp(Attack,        0, 1) * 0xFFFF),
        .Decay         = (u16)(Clamp(Decay,         0, 1) * 0xFFFF),
        .Sustain       = (u16)(Clamp(Sustain,       0, 1) * 0xFFFF),
        .Release       = (u16)(Clamp(Release,       0, 1) * 0xFFFF),
    };
}

bool Audio_EnvelopeSamples(envelope *Envelope, u16 *SampleBuffer, u32 SampleCount)
{
    bool Ended = false;

    while (SampleCount--)
    {
        u32 Counter = ++Envelope->Counter;

        if (Counter < Envelope->Attack)
        {
            Counter *= Envelope->AttackVolume;
            Counter /= Envelope->Attack;
            *SampleBuffer++ = Counter & 0xFFFF;
            continue;
        }

        Counter -= Envelope->Attack;
        if (Counter < Envelope->Decay)
        {
            Counter *= Envelope->AttackVolume - Envelope->SustainVolume;
            Counter /= Envelope->Decay;
            *SampleBuffer++ = (Envelope->AttackVolume - Counter) & 0xFFFF;
            continue;
        }
        
        Counter -= Envelope->Decay;
        if (Counter < Envelope->Sustain)
        {
            *SampleBuffer++ = Envelope->SustainVolume;
            continue;
        }

        Counter -= Envelope->Sustain;
        if (Counter < Envelope->Release)
        {
            Counter *= Envelope->SustainVolume;
            Counter /= Envelope->Release;
            *SampleBuffer++ = (Envelope->SustainVolume - Counter) & 0xFFFF;
            continue;
        }

        *SampleBuffer++ = 0;
        Ended = true;
    }

    return Ended;
}

void Audio_WriteSamples(i16 *SampleBuffer, u32 SampleCount)
{
    u8 TempSampleBuffer[32];

    for (u32 i = 0; i < SampleCount >> 4; ++i)
    {
        __m256i Samples = _mm256_setzero_si256();

        for (sound_list *SoundList = GlobalAudioState.FirstUsedSound; SoundList; SoundList = SoundList->Next)
        {
            sound *Sound = &SoundList->Sound;

            Audio_EnvelopeSamples(&Sound->Envelope, (u16 *)TempSampleBuffer, 16);
            __m256i EnvelopeSamples = _mm256_loadu_si256((__m256i *)TempSampleBuffer);

            Audio_GeneratorSamples(&Sound->Generator, (i16 *)TempSampleBuffer, 16);
            __m256i GeneratorSamples = _mm256_loadu_si256((__m256i *)TempSampleBuffer);

            Samples = _mm256_adds_epi16(Samples, _mm256_mulhi_epi16(EnvelopeSamples, GeneratorSamples));
        }

        __m256i Samples0 = _mm256_cvtepu16_epi32(_mm256_castsi256_si128(Samples));
        Samples0 = _mm256_or_si256(Samples0, _mm256_slli_epi32(Samples0, 16));
        _mm256_storeu_si256((__m256i *)&SampleBuffer[i*32], Samples0);

        __m256i Samples1 = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(Samples, 1));
        Samples1 = _mm256_or_si256(Samples1, _mm256_slli_epi32(Samples1, 16));
        _mm256_storeu_si256((__m256i *)&SampleBuffer[i*32+16], Samples1);
    }
}

#else

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
        Counter += 0;
        return Counter & 0xFFFF;
    }

    return 0;
}

phase_generator Audio_PhaseGenerator(f32 Frequency, f32 dFrequency)
{
    Frequency = Max(0.0f, Frequency);

    return (phase_generator) {
        .Frequency = (u16)(Frequency * (AUDIO_SECOND_PER_SAMPLE * 0xFFFF)),
        .dFrequency = (u16)(dFrequency * (AUDIO_SECOND_PER_SAMPLE * 0xFFFF)),
    };
}

u16 Audio_PhaseSample(phase_generator *Phase)
{
    Phase->Frequency += Phase->dFrequency;
    Phase->Phase += Phase->Frequency;
    return Phase->Phase;
}

i16 Sin_U16(u16 x)
{
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

i16 Audio_SineWaveSample(u16 Phase)
{
    return Sin_U16(Phase);
}

i16 Audio_TriangleWaveSample(u16 Phase)
{
    if (Phase > 0x3FFF && Phase < 0xBFFF)
        return 0x7FFF - Phase;
    return Phase;
}

i16 Audio_SawtoothWaveSample(u16 Phase)
{
    return Phase;
}

i16 Audio_SquareWaveSample(square_wave_generator *Wave, u16 Phase)
{
    Wave->Duty += Wave->dDuty;
    u16 Sample = 0x8000;
    if (Phase < Wave->Duty >> 1)
        Sample = 0x7FFF;
    return Sample;
}

i16 Audio_WaveSample(wave_generator *Wave, u16 Phase)
{
    switch (Wave->Type)
    {
        case SINE_WAVE_GENERATOR:     return Audio_SineWaveSample(Phase); break;
        case TRIANGLE_WAVE_GENERATOR: return Audio_TriangleWaveSample(Phase); break;
        case SAWTOOTH_WAVE_GENERATOR: return Audio_SawtoothWaveSample(Phase); break;
        case SQUARE_WAVE_GENERATOR:   return Audio_SquareWaveSample(&Wave->Square, Phase); break;
        // case WHITE_NOISE_GENERATOR:   return Audio_WhiteNoiseSample(Phase); break;
        // case PINK_NOISE_GENERATOR:    return Audio_PinkNoiseSample(Phase); break;
        // case BROWN_NOISE_GENERATOR:   return Audio_BrownNoiseSample(Phase); break;
    }
    return 0;
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
            Sample += (Envelope * Wave) >> 16;
        }
        *SampleBuffer++ = Sample;
        *SampleBuffer++ = Sample;
    }
}

#endif

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

void Audio_Init(void)
{
    GlobalAudioState.FirstUsedSound = 0;
    GlobalAudioState.FirstFreeSound = &GlobalAudioState.Sounds[0];
    for (u32 i = 0; i < AUDIO_MAX_SOUNDS - 1; ++i)
        GlobalAudioState.Sounds[i].Next = &GlobalAudioState.Sounds[i+1];
    GlobalAudioState.Sounds[AUDIO_MAX_SOUNDS-1].Next = 0;
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