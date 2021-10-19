
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

	u64 LowNoise;
	u64 HighNoise;
	u64 SelectorNoise;
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

u32 WordlGen_CellBiome(ivec2 Cell)
{
	u64 Continent = Hash_IVec2(iVec2_And(Cell, iVec2_Set1(~0xFF)));	

	return 0;
}

void WorldGen_Chunk(chunk *Chunk)
{
	u32 CellBiomes[5][5];
	for (u32 y = 0; y < 5; ++y)
	for (u32 x = 0; x < 5; ++x)
	{
		ivec2 Cell = iVec2_Add(iVec2_Sub(Chunk->Position, iVec2_Set1(2)), (ivec2){x,y});
		CellBiomes[y][x] = WordlGen_CellBiome(Cell);
	}
}

global f32 MINIMUM_LOW = 11111;
global f32 MINIMUM_HIGH = 11111;

void WorldGen_GenerateChunk(chunk *Chunk)
{
	for (i32 yy = 0; yy < CHUNK_WIDTH; yy++)
    for (i32 xx = 0; xx < CHUNK_WIDTH; xx++)
    {
        vec2 p = (vec2){ (f32)(Chunk->Position.x * CHUNK_WIDTH + xx),
                         (f32)(Chunk->Position.y * CHUNK_WIDTH + yy) };
#if 0
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
        p = Vec2_Div(p, (vec2){384.0f,384.0f});

        f32 LowNoise = Lerp(64.0f, 80.0f, Noise_FBM2D(GlobalWorldGen.LowNoise, p, 16));
        f32 HighNoise = Lerp(96.0f, 128.0f, Noise_FBM2D(GlobalWorldGen.HighNoise, p, 16));
        
        for (u32 zz = 0; zz < CHUNK_HEIGHT; ++zz)
        {
        	vec3 pp = (vec3){ (f32)(Chunk->Position.x * CHUNK_WIDTH + xx),
            	              (f32)(Chunk->Position.y * CHUNK_WIDTH + yy),
            	              (f32)zz };
            block *CurrentBlock = &Chunk->Blocks[zz][yy][xx];

        	pp = Vec3_Div(pp, (vec3){128.0f,128.0f,256.0f});

        	f32 SelectorNoise = Clamp((Noise_FBM3D(GlobalWorldGen.SelectorNoise, pp, 4) - 0.0f) * 2.0f, 0, 1);
        	f32 Surface = Lerp(LowNoise, HighNoise, F32_Smootherstep(SelectorNoise));
        	if (zz < Surface) CurrentBlock->Id = BLOCK_ID_STONE;


			MINIMUM_LOW = Min(MINIMUM_LOW, LowNoise);
			MINIMUM_HIGH = Min(MINIMUM_HIGH, HighNoise);
        }

        for (i32 zz = CHUNK_HEIGHT - 1; zz >= 64; --zz)
        {
            if (Chunk->Blocks[zz][yy][xx].Id != BLOCK_ID_STONE) continue;
        	Chunk->Blocks[zz][yy][xx].Id = BLOCK_ID_GRAS;
            if (Chunk->Blocks[zz-1][yy][xx].Id != BLOCK_ID_STONE) break;
        	Chunk->Blocks[zz-1][yy][xx].Id = BLOCK_ID_DIRT;
            if (Chunk->Blocks[zz-2][yy][xx].Id != BLOCK_ID_STONE) break;
        	Chunk->Blocks[zz-2][yy][xx].Id = BLOCK_ID_DIRT;
        	break;
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

	GlobalWorldGen.LowNoise = Rng_U64(&Rng);
	GlobalWorldGen.HighNoise = Rng_U64(&Rng);
	GlobalWorldGen.SelectorNoise = Rng_U64(&Rng);
}
