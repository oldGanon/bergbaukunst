
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
	u64 OceanSeed;
	u64 PlainSeed;
	u64 ForestSeed;
	u64 DesertSeed;
	u64 MountainSeed;
	u64 BiomeContourSeedX;
	u64 BiomeContourSeedY;
} world_gen;

global world_gen GlobalWorldGen;

f32 WorldGen_Noise(u64 Seed, vec2 Position, f32 Lowest, f32 Highest, f32 MaxIncline)
{
	f32 Period = (Highest - Lowest) / MaxIncline;
	f32 Frequency = 1.0f / Period;
	u32 Octaves = Log2(F32_FloorToI32(Period));
	Position = Vec2_Mul(Position, Vec2_Set1(Frequency));
	f32 Value = Noise_FBM2D(Seed, Position, Octaves);
	f32 Height = Lerp(Lowest, Highest, Value * 0.5f + 0.5f);
	return Height;
}

f32 WorldGen_OceanNoise(vec2 Position)
{
	return WorldGen_Noise(GlobalWorldGen.OceanSeed, Position, 32, 64, 1);
}

f32 WorldGen_PlainNoise(vec2 Position)
{
	return WorldGen_Noise(GlobalWorldGen.PlainSeed, Position, 64, 72, 0.25f);
}

f32 WorldGen_ForestNoise(vec2 Position)
{
	return WorldGen_Noise(GlobalWorldGen.ForestSeed, Position, 64, 72, 0.25f);
}

f32 WorldGen_DesertNoise(vec2 Position)
{
	return WorldGen_Noise(GlobalWorldGen.DesertSeed, Position, 64, 72, 0.25f);
}

f32 WorldGen_MountainNoise(vec2 Position)
{
	return WorldGen_Noise(GlobalWorldGen.MountainSeed, Position, 64, 96, 0.25f);
}

// u32 WordlGen_CellBiome(ivec2 Cell)
// {
// 	u64 Continent = Hash_IVec2(iVec2_And(Cell, iVec2_Set1(~0xFF)));
	
// }

void WorldGen_GenerateChunk(chunk *Chunk)
{
	for (i32 yy = 0; yy < CHUNK_WIDTH; yy++)
    for (i32 xx = 0; xx < CHUNK_WIDTH; xx++)
    {
        vec2 p = (vec2){ (f32)(Chunk->Position.x * CHUNK_WIDTH + xx),
                         (f32)(Chunk->Position.y * CHUNK_WIDTH + yy) };
#if 1
        u32 BiomeSize = 64;
		vec2 BiomeSize2 = Vec2_Set1((f32)BiomeSize);
		vec2 BiomePosition = Vec2_Div(p, BiomeSize2);

		voronoi_result VoronoiBase;
		Noise_Voronoi(BiomePosition, &VoronoiBase);

		u32 ContourSize = 16;
		vec2 ContourSize2 = Vec2_Set1((f32)ContourSize);
		vec2 ContourPosition = Vec2_Div(p, ContourSize2);
		u32 Octaves = Log2(ContourSize);
		f32 ContourX = Noise_FBM2D(GlobalWorldGen.BiomeContourSeedX, ContourPosition, Octaves);
		f32 ContourY = Noise_FBM2D(GlobalWorldGen.BiomeContourSeedY, ContourPosition, Octaves);
		vec2 ContourOffset = (vec2){ ContourX, ContourY };
		BiomePosition = Vec2_Add(BiomePosition, Vec2_Mul(ContourOffset, Vec2_Div(ContourSize2, BiomeSize2)));

		voronoi_result Voronoi;
		Noise_Voronoi(BiomePosition, &Voronoi);


		u64 BiomeIndex = Hash_IVec2(Voronoi.PointCell);
		u32 Biome = WorldGen_BiomeList[BiomeIndex % sizeof(WorldGen_BiomeList)];
		u64 BorderBiomeIndex = Hash_IVec2(Voronoi.BorderCell);
		u32 BorderBiome = WorldGen_BiomeList[BorderBiomeIndex % sizeof(WorldGen_BiomeList)];

		u64 BaseBiomeIndex = Hash_IVec2(VoronoiBase.PointCell);
		u32 BaseBiome = WorldGen_BiomeList[BaseBiomeIndex % sizeof(WorldGen_BiomeList)];
		u64 BaseBorderBiomeIndex = Hash_IVec2(VoronoiBase.BorderCell);
		u32 BaseBorderBiome = WorldGen_BiomeList[BaseBorderBiomeIndex % sizeof(WorldGen_BiomeList)];

        block Block;
        switch (Biome)
        {
        	default: 			 Block = (block){ .Id = BLOCK_ID_AIR }; break;
			case BIOME_OCEAN: 	 Block = (block){ .Id = BLOCK_ID_DIRT }; break;
			case BIOME_PLAIN: 	 Block = (block){ .Id = BLOCK_ID_GRAS }; break;
			case BIOME_FOREST: 	 Block = (block){ .Id = BLOCK_ID_GRAS }; break;
			case BIOME_DESERT: 	 Block = (block){ .Id = BLOCK_ID_SAND }; break;
			case BIOME_MOUNTAIN: Block = (block){ .Id = BLOCK_ID_STONE }; break;
        }

        f32 Surface;
        switch (BaseBiome)
        {
        	default:             Surface = -1; break;
			case BIOME_OCEAN: 	 Surface = WorldGen_OceanNoise(p); break;
			case BIOME_PLAIN: 	 Surface = WorldGen_PlainNoise(p); break;
			case BIOME_FOREST: 	 Surface = WorldGen_ForestNoise(p); break;
			case BIOME_DESERT: 	 Surface = WorldGen_DesertNoise(p); break;
			case BIOME_MOUNTAIN: Surface = WorldGen_MountainNoise(p); break;
        }

        if (BaseBiome != BaseBorderBiome)
        {
	        f32 Blend = Clamp(VoronoiBase.BorderDist * 8, 0.0f, 1.0f);
	        Surface = Lerp(64.0f, Surface, Blend);        	
        }

        i32 iSurface = F32_FloorToI32(Surface);
        for (i32 zz = iSurface; zz >= 0; --zz)
        {
            Chunk->Blocks[zz][yy][xx] = Block;
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

	GlobalWorldGen.OceanSeed = Rng_U64(&Rng);
	GlobalWorldGen.PlainSeed = Rng_U64(&Rng);
	GlobalWorldGen.ForestSeed = Rng_U64(&Rng);
	GlobalWorldGen.DesertSeed = Rng_U64(&Rng);
	GlobalWorldGen.MountainSeed = Rng_U64(&Rng);
	GlobalWorldGen.BiomeContourSeedX = Rng_U64(&Rng);
	GlobalWorldGen.BiomeContourSeedY = Rng_U64(&Rng);
}
