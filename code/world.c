
#include "chunkmap.c"
#include "worldgen.c"

#define LOADED_CHUNKS_DIM_SHIFT 5
#define LOADED_CHUNKS_DIM (1 << LOADED_CHUNKS_DIM_SHIFT)
#define LOADED_CHUNKS_DIM_MASK (LOADED_CHUNKS_DIM - 1)
#define LOADED_CHUNKS_DIST_SHIFT (LOADED_CHUNKS_DIM_SHIFT - 1)
#define LOADED_CHUNKS_DIST (1 << LOADED_CHUNKS_DIST_SHIFT)
#define LOADED_CHUNKS_DIST_MASK (LOADED_CHUNKS_DIST - 1)

typedef struct world
{
    chunk_map ChunkMap;
    entity_manager EntityManager;
} world;

// typedef enum world_direction
// {
//     WEST,  // -x
//     EAST,  // +x
//     SOUTH, // -y
//     NORTH, // +y
//     UP,    // +z
//     DOWN,  // -z
// } world_direction;

void World_Init(world *World)
{
    World->ChunkMap = ChunkMap_Create();
    World->EntityManager = EntityManager_Create();
    WorldGen_Init();
}

void World_Update(world *World, vec3 Player)
{
    ivec2 CenterChunk = World_ToChunkPosition(Vec3_FloorToIVec3(Player));
    ivec2 LoadedChunkDist = iVec2_Set1(LOADED_CHUNKS_DIST);
    ivec2 MinPos = iVec2_Sub(CenterChunk, LoadedChunkDist);
    ivec2 MaxPos = iVec2_Add(CenterChunk, LoadedChunkDist);

    for (i32 x = MinPos.x; x < MaxPos.x; x++)
    for (i32 y = MinPos.y; y < MaxPos.y; y++)
    {
        ivec2 ChunkPos = (ivec2){ x, y };
        chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, ChunkPos);
        if (Chunk) continue;

        Chunk = ChunkMap_AllocateChunk(&World->ChunkMap, ChunkPos);
        WorldGen_GenerateChunk(Chunk);
        Chunk_CalcSkyLight(Chunk);
    }
}


chunk *World_GetChunk(world *World, ivec2 ChunkPosition)
{
    return ChunkMap_GetChunk(&World->ChunkMap, ChunkPosition);
}

const chunk *World_GetConstChunk(const world *World, ivec2 ChunkPosition)
{
    return ChunkMap_GetChunk(&World->ChunkMap, ChunkPosition);
}

block World_GetBlock(const world *World, ivec3 WorldPosition)
{
    if (WorldPosition.z < 0)                return DEFAULT_SKY_BLOCK;
    if (WorldPosition.z > CHUNK_HEIGHT - 1) return DEFAULT_HELL_BLOCK;

    ivec2 ChunkPosition = World_ToChunkPosition(WorldPosition);
    const chunk *Chunk = World_GetConstChunk(World, ChunkPosition);
    if (!Chunk) return DEFAULT_BLOCK; // maybe generate chunk instead?
    return Chunk_GetBlock(Chunk, WorldPosition);
}

void World_SetBlock(world *World, ivec3 WorldPosition, block Block)
{
    if ((WorldPosition.z < 0) || (WorldPosition.z > CHUNK_HEIGHT - 1))
        return;

    ivec2 ChunkPosition = World_ToChunkPosition(WorldPosition);
    chunk *Chunk = World_GetChunk(World, ChunkPosition);
    if (!Chunk) return; // maybe generate chunk instead?
    Chunk_SetBlock(Chunk, WorldPosition, Block);
}



u32 World_SpawnPlayer(world *World, vec3 Position)
{
    return Entity_Spawn(&World->EntityManager, (entity) {
        .Type = ENTITY_PLAYER,
        .Position = Position,
    });
}



f32 World_TraceRay(world *World, vec3 RayOrigin, vec3 RayDirection, f32 RayLength, trace_result *Result)
{
    return Phys_TraceRay(World_GetBlock, World, RayOrigin, RayDirection, RayLength, Result);
}

vec3 World_CheckMoveBox(world *World, box Box, vec3 Move)
{
    return Phys_CheckMoveBox(World_GetBlock, World, Box, Move);
}
