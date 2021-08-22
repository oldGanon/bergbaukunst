
struct square_wave
{
	// Envelope
	u16 Volume;
	u16 Attack;
	u16 Sustain;
	u16 Decay;

	// Frequency
	u16 StartFreq;
	i16 SlideFreq;
};

static void
Audio_WriteSamples(i16 *SampleBuffer, u32 SampleCount)
{
	
}
