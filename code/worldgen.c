
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
    // terrain noise
    u64 LowNoise;
    u64 HighNoise;
    u64 SelectorNoise;

    // biome noise
    u64 OceanSeed;
    u64 PlainSeed;
    u64 ForestSeed;
    u64 DesertSeed;
    u64 MountainSeed;
    u64 BiomeContourSeedX;
    u64 BiomeContourSeedY;

    // decoration noise
    u64 TreeSeed;
} world_gen;

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

f32 WorldGen_OceanNoise(world_gen *WorldGenerator, vec2 Position)
{
    return WorldGen_Noise(WorldGenerator->OceanSeed, Position, 32, 64, 1);
}

f32 WorldGen_PlainNoise(world_gen *WorldGenerator, vec2 Position)
{
    return WorldGen_Noise(WorldGenerator->PlainSeed, Position, 64, 72, 0.25f);
}

f32 WorldGen_ForestNoise(world_gen *WorldGenerator, vec2 Position)
{
    return WorldGen_Noise(WorldGenerator->ForestSeed, Position, 64, 72, 0.25f);
}

f32 WorldGen_DesertNoise(world_gen *WorldGenerator, vec2 Position)
{
    return WorldGen_Noise(WorldGenerator->DesertSeed, Position, 64, 72, 0.25f);
}

f32 WorldGen_MountainNoise(world_gen *WorldGenerator, vec2 Position)
{
    return WorldGen_Noise(WorldGenerator->MountainSeed, Position, 64, 96, 0.25f);
}

u32 WordlGen_CellBiome(ivec2 Cell)
{
    u64 Continent = Hash_IVec2(iVec2_And(Cell, iVec2_Set1(~0xFF))); 

    return 0;
}

void WorldGen_Chunk(world_chunk *Chunk)
{
    u32 CellBiomes[5][5];
    for (u32 y = 0; y < 5; ++y)
    for (u32 x = 0; x < 5; ++x)
    {
        ivec2 Cell = iVec2_Add(iVec2_Sub(Chunk->Base.Position, iVec2_Set1(2)), (ivec2){x,y});
        CellBiomes[y][x] = WordlGen_CellBiome(Cell);
    }
}

void WorldGen_GenerateChunk(world_gen *WorldGenerator, world_chunk *Chunk)
{
    for (i32 yy = 0; yy < CHUNK_WIDTH; yy++)
    for (i32 xx = 0; xx < CHUNK_WIDTH; xx++)
    {
        vec2 p = (vec2){ (f32)(Chunk->Base.Position.x * CHUNK_WIDTH + xx),
                         (f32)(Chunk->Base.Position.y * CHUNK_WIDTH + yy) };
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
            default:             Block = (block){ .Id = BLOCK_ID_AIR }; break;
            case BIOME_OCEAN:    Block = (block){ .Id = BLOCK_ID_DIRT }; break;
            case BIOME_PLAIN:    Block = (block){ .Id = BLOCK_ID_GRAS }; break;
            case BIOME_FOREST:   Block = (block){ .Id = BLOCK_ID_GRAS }; break;
            case BIOME_DESERT:   Block = (block){ .Id = BLOCK_ID_SAND }; break;
            case BIOME_MOUNTAIN: Block = (block){ .Id = BLOCK_ID_STONE }; break;
        }

        f32 Surface;
        switch (BaseBiome)
        {
            default:             Surface = -1; break;
            case BIOME_OCEAN:    Surface = WorldGen_OceanNoise(p); break;
            case BIOME_PLAIN:    Surface = WorldGen_PlainNoise(p); break;
            case BIOME_FOREST:   Surface = WorldGen_ForestNoise(p); break;
            case BIOME_DESERT:   Surface = WorldGen_DesertNoise(p); break;
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

        f32 LowNoise = Lerp(64.0f, 80.0f, Noise_FBM2D(WorldGenerator->LowNoise, p, 16));
        f32 HighNoise = Lerp(96.0f, 128.0f, Noise_FBM2D(WorldGenerator->HighNoise, p, 16));
        
        for (u32 zz = 0; zz < (u32)LowNoise; ++zz)
            Chunk->Base.Blocks[zz][yy][xx].Id = BLOCK_ID_STONE;

        for (u32 zz = (u32)LowNoise; zz < (u32)HighNoise; ++zz)
        {
            block *CurrentBlock = &Chunk->Base.Blocks[zz][yy][xx];
            vec3 pp = (vec3){ (f32)(Chunk->Base.Position.x * CHUNK_WIDTH + xx),
                              (f32)(Chunk->Base.Position.y * CHUNK_WIDTH + yy),
                              (f32)zz };
            pp = Vec3_Div(pp, (vec3){128.0f,128.0f,256.0f});
            f32 SelectorNoise = Noise_FBM3D(WorldGenerator->SelectorNoise, pp, 4);
            SelectorNoise = Clamp((SelectorNoise - 0.0f) * 2.0f, 0, 1);
            f32 Surface = Lerp(LowNoise, HighNoise, SmootherStep(SelectorNoise));
            if (zz < Surface) CurrentBlock->Id = BLOCK_ID_STONE;
        }

        // Gras
        for (i32 zz = CHUNK_HEIGHT - 1; zz > 65; --zz)
        {
            if (Chunk->Base.Blocks[zz][yy][xx].Id != BLOCK_ID_STONE) continue;
            Chunk->Base.Blocks[zz][yy][xx].Id = BLOCK_ID_GRAS;
            if (Chunk->Base.Blocks[zz-1][yy][xx].Id != BLOCK_ID_STONE) break;
            Chunk->Base.Blocks[zz-1][yy][xx].Id = BLOCK_ID_DIRT;
            if (Chunk->Base.Blocks[zz-2][yy][xx].Id != BLOCK_ID_STONE) break;
            Chunk->Base.Blocks[zz-2][yy][xx].Id = BLOCK_ID_DIRT;
            break;
        }
        // Sand
        if (Chunk->Base.Blocks[66][yy][xx].Id == BLOCK_ID_AIR &&
            Chunk->Base.Blocks[65][yy][xx].Id == BLOCK_ID_STONE)
            Chunk->Base.Blocks[65][yy][xx].Id = BLOCK_ID_SAND;
        if (Chunk->Base.Blocks[65][yy][xx].Id == BLOCK_ID_AIR &&
            Chunk->Base.Blocks[64][yy][xx].Id == BLOCK_ID_STONE)
            Chunk->Base.Blocks[64][yy][xx].Id = BLOCK_ID_SAND;
        // Water
        for (i32 zz = 64; zz >= 0; --zz)
        {
            if (Chunk->Base.Blocks[zz][yy][xx].Id == BLOCK_ID_AIR)
                Chunk->Base.Blocks[zz][yy][xx].Id = BLOCK_ID_WATER;
        }
#endif
    }
    Chunk->DirtyMin = (ivec3){0,0,0};
    Chunk->DirtyMax = (ivec3){CHUNK_WIDTH_MASK,CHUNK_WIDTH_MASK,CHUNK_HEIGHT_MASK};
    Chunk->Flags |= (CHUNK_GENERATED | CHUNK_DIRTY);
}

void WorldGen_GrowTree(world_chunk_group *ChunkGroup, ivec2 Tree)
{
    for (i32 zz = CHUNK_HEIGHT - 1; zz >= 64; --zz)
    {
        ivec3 GrasPosition = { Tree.x, Tree.y, zz };
        if (ChunkGroup_GetBlock(ChunkGroup, GrasPosition).Id != BLOCK_ID_GRAS)
            continue;

        ivec3 LeaveCenter = (ivec3){ Tree.x, Tree.y, zz+5 };
        for (i32 x = -2; x <= 2; ++x)
        for (i32 y = -2; y <= 2; ++y)
        for (i32 z = -2; z <= 2; ++z)
        {
            ivec3 LeavePosition = (ivec3){ Tree.x+x, Tree.y+y, zz+5+z };
            i32 LeaveDist = iVec3_Sum(iVec3_Abs(iVec3_Sub(LeavePosition, LeaveCenter)));
            if (LeaveDist > 3) continue;
            if (ChunkGroup_GetBlock(ChunkGroup, LeavePosition).Id != BLOCK_ID_AIR)
                continue;
            ChunkGroup_SetBlock(ChunkGroup, LeavePosition, (block){ BLOCK_ID_LEAVES });
        }

        ChunkGroup_SetBlock(ChunkGroup, (ivec3){ Tree.x, Tree.y, zz+1}, (block){ BLOCK_ID_WOOD });
        ChunkGroup_SetBlock(ChunkGroup, (ivec3){ Tree.x, Tree.y, zz+2}, (block){ BLOCK_ID_WOOD });
        ChunkGroup_SetBlock(ChunkGroup, (ivec3){ Tree.x, Tree.y, zz+3}, (block){ BLOCK_ID_WOOD });
        ChunkGroup_SetBlock(ChunkGroup, (ivec3){ Tree.x, Tree.y, zz+4}, (block){ BLOCK_ID_WOOD });
        ChunkGroup_SetBlock(ChunkGroup, (ivec3){ Tree.x, Tree.y, zz+5}, (block){ BLOCK_ID_WOOD });
        break;
    }
}

void WorldGen_DecorateChunk(world_gen *WorldGenerator, world_chunk_group ChunkGroup, ivec2 ChunkPosition)
{
    if (!ChunkGroup_Complete(&ChunkGroup)) return;

    // get tree positions
    vec3 Trees[12][12];
    for (u32 y = 0; y < 3; ++y)
    for (u32 x = 0; x < 3; ++x)
    {
        ivec2 iOffset = { x - 1, y - 1 };
        ivec2 iChunkPos = iVec2_Add(ChunkPosition, iOffset);
        vec2 ChunkPos = Vec2_Mul(iVec2_toVec2(iChunkPos), Vec2_Set1(4));
        rng Rng = Rng_Init(Hash_IVec2Seeded(WorldGenerator->TreeSeed, iChunkPos));
        for (u32 yy = 0; yy < 4; ++yy)
        for (u32 xx = 0; xx < 4; ++xx)
        {
            vec2 Tree = ChunkPos;
            Tree = Vec2_Add(Tree, Rng_Vec2(&Rng));
            Tree = Vec2_Add(Tree, (vec2){ (f32)xx, (f32)yy });
            Tree = Vec2_Mul(Tree, Vec2_Set1(4));
            Trees[y * 4 + yy][x * 4 + xx].xy = Tree;
            Trees[y * 4 + yy][x * 4 + xx].z = Rng_F32(&Rng);
        }
    }

    // grow trees
    for (u32 y = 4; y < 8; ++y)
    for (u32 x = 4; x < 8; ++x)
    {
        vec2 NoisePos = Vec2_Div(Trees[y][x].xy, Vec2_Set1(256.0f));
        f32 Density = Noise_Simplex2D(WorldGenerator->TreeSeed, NoisePos, 0);
        if (Trees[y][x].z > Abs(Density)) continue;
        if (Vec2_Dist(Trees[y][x].xy, Trees[y-1][x-1].xy) < 3.0f) continue;
        if (Vec2_Dist(Trees[y][x].xy, Trees[y-1][x  ].xy) < 3.0f) continue;
        if (Vec2_Dist(Trees[y][x].xy, Trees[y-1][x+1].xy) < 3.0f) continue;
        if (Vec2_Dist(Trees[y][x].xy, Trees[y  ][x-1].xy) < 3.0f) continue;
        WorldGen_GrowTree(&ChunkGroup, Vec2_FloorToIVec2(Trees[y][x].xy));
    }
    
    ChunkGroup.Chunks[1][1]->Flags |= CHUNK_DECORATED;
}

world_gen WorldGen_Create(u64 Seed)
{
    rng Rng = Rng_Init(Seed);
    return (world_gen) {
        // terrain noise
        .LowNoise = Rng_U64(&Rng),
        .HighNoise = Rng_U64(&Rng),
        .SelectorNoise = Rng_U64(&Rng),

        // biome noise
        .OceanSeed = Rng_U64(&Rng),
        .PlainSeed = Rng_U64(&Rng),
        .ForestSeed = Rng_U64(&Rng),
        .DesertSeed = Rng_U64(&Rng),
        .MountainSeed = Rng_U64(&Rng),
        .BiomeContourSeedX = Rng_U64(&Rng),
        .BiomeContourSeedY = Rng_U64(&Rng),

        // decoration noise
        .TreeSeed = Rng_U64(&Rng),
    };
}
