
enum
{
	BIOME_OCEAN,
	BIOME_GRAS,
	BIOME_STONE,
	BIOME_SAND,
} biome;

typedef struct world_gen
{
	noise ContinentNoise;
	noise ContourNoiseX;
	noise ContourNoiseY;
} world_gen;

global world_gen GlobalWorldGen;

u32 WorldGen_GetBiome(vec2 Position)
{
	u32 ContinentSize = 64;
	vec2 ContinentSize2 = Vec2_Set1((f32)ContinentSize);

#if 0
	u32 Octaves = Log2(ContinentSize);
	vec2 ContourPosition = Vec2_Div(Position, ContinentSize2);
	f32 ContourX = Noise_FBM2D(&GlobalWorldGen.ContourNoiseX, ContourPosition, Octaves);
	f32 ContourY = Noise_FBM2D(&GlobalWorldGen.ContourNoiseY, ContourPosition, Octaves);
	vec2 ContourOffset = (vec2){ ContourX, ContourY };
	Position = Vec2_Add(Position, Vec2_Mul(ContourOffset, ContinentSize2));

	Position = Vec2_Div(Position, ContinentSize2);
	Position = Vec2_Floor(Position);

	vec2 Grad;
	f32 Continent = Noise_Simplex2D(&GlobalWorldGen.ContinentNoise, Position, &Grad);
#endif
	u32 Octaves = Log2(ContinentSize);
	vec2 ContourPosition = Vec2_Div(Position, ContinentSize2);
	f32 Continent = Noise_FBM2D(&GlobalWorldGen.ContourNoiseX, ContourPosition, Octaves);
	

	if (Continent > 0.0f) return BIOME_GRAS;
	return BIOME_OCEAN;
}

void WorldGen_GenerateChunk(chunk *Chunk)
{
	for (i32 yy = 0; yy < CHUNK_WIDTH; yy++)
    for (i32 xx = 0; xx < CHUNK_WIDTH; xx++)
    {
        vec2 p = (vec2){ (f32)(Chunk->x * CHUNK_WIDTH + xx),
                         (f32)(Chunk->y * CHUNK_WIDTH + yy) };
#if 0
        switch (WorldGen_GetBiome(p))
        {
        	case BIOME_GRAS:  Chunk->Blocks[0][yy][xx] = (block){ .Id = BLOCK_ID_GRAS }; break;
			case BIOME_STONE: Chunk->Blocks[0][yy][xx] = (block){ .Id = BLOCK_ID_STONE }; break;
			case BIOME_SAND:  Chunk->Blocks[0][yy][xx] = (block){ .Id = BLOCK_ID_SAND }; break;
        }
#else
        f32 DistanceBetweenPeaks = 128.0f;
        f32 SeaLevel = 64;
        f32 PeakHeight = 96;

        p = Vec2_Mul(p, Vec2_Set1(1.0f / DistanceBetweenPeaks));

        u32 Octaves = 7; // Log2(DistanceBetweenPeaks)
        f32 Surface = Lerp(SeaLevel, PeakHeight, Noise_FBM2D(0, p, Octaves));
        
        i32 iSurface = F32_FloorToI32(Surface);
        for (i32 zz = iSurface; zz >= 0; --zz)
        {
            block *CurrentBlock = &Chunk->Blocks[zz][yy][xx];
            CurrentBlock->Id = BLOCK_ID_GRAS;
        }
#endif
    }
}

void WorldGen_Init(void)
{
	u64 Seed = 0xDEADDEADDEADDEAD;
	rng Rng = Rng_Init(Seed);

	GlobalWorldGen.ContinentNoise = Noise_Random(&Rng);
	GlobalWorldGen.ContourNoiseX = Noise_Random(&Rng);
	GlobalWorldGen.ContourNoiseY = Noise_Random(&Rng);
}
