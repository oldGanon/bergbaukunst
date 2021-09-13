
enum
{
	BIOME_OCEAN,
	BIOME_PLAIN,
	BIOME_FOREST,
	BIOME_DESERT,
	BIOME_MOUNTAIN,
} biome;

global const u8 WorldGen_BiomeList[] = {
	BIOME_OCEAN,
	BIOME_OCEAN,
	BIOME_OCEAN,
	BIOME_OCEAN,
	BIOME_PLAIN,
	BIOME_FOREST,
	BIOME_DESERT,
	BIOME_MOUNTAIN,
};

typedef struct world_gen
{
	noise ContinentNoise;
	noise ContourNoiseX;
	noise ContourNoiseY;
} world_gen;

global world_gen GlobalWorldGen;

u32 WorldGen_GetBiome(vec2 Position)
{
	u32 BiomeSize = 64;
	vec2 BiomeSize2 = Vec2_Set1((f32)BiomeSize);
	vec2 BiomePosition = Vec2_Div(Position, BiomeSize2);

	u32 ContourSize = 16;
	vec2 ContourSize2 = Vec2_Set1((f32)ContourSize);
	vec2 ContourPosition = Vec2_Div(Position, ContourSize2);
	u32 Octaves = Log2(ContourSize);
	f32 ContourX = Noise_FBM2D(&GlobalWorldGen.ContourNoiseX, ContourPosition, Octaves);
	f32 ContourY = Noise_FBM2D(&GlobalWorldGen.ContourNoiseY, ContourPosition, Octaves);
	vec2 ContourOffset = (vec2){ ContourX, ContourY };
	BiomePosition = Vec2_Add(BiomePosition, Vec2_Mul(ContourOffset, Vec2_Div(ContourSize2, BiomeSize2)));

	voronoi_result Voronoi;
	Noise_Voronoi(BiomePosition, &Voronoi);

	u64 BiomeIndex = Hash_IVec2(Voronoi.PointCell);
	u32 Biome = WorldGen_BiomeList[BiomeIndex % sizeof(WorldGen_BiomeList)];
	return Biome;
}

void WorldGen_GenerateChunk(chunk *Chunk)
{
	for (i32 yy = 0; yy < CHUNK_WIDTH; yy++)
    for (i32 xx = 0; xx < CHUNK_WIDTH; xx++)
    {
        vec2 p = (vec2){ (f32)(Chunk->x * CHUNK_WIDTH + xx),
                         (f32)(Chunk->y * CHUNK_WIDTH + yy) };
#if 1
        switch (WorldGen_GetBiome(p))
        {
			case BIOME_OCEAN: break;
			case BIOME_PLAIN: Chunk->Blocks[0][yy][xx] = (block){ .Id = BLOCK_ID_GRAS }; break;
			case BIOME_FOREST: Chunk->Blocks[0][yy][xx] = (block){ .Id = BLOCK_ID_GRAS }; break;
			case BIOME_DESERT: Chunk->Blocks[0][yy][xx] = (block){ .Id = BLOCK_ID_SAND }; break;
			case BIOME_MOUNTAIN: Chunk->Blocks[0][yy][xx] = (block){ .Id = BLOCK_ID_STONE }; break;
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
