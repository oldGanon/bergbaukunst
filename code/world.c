
enum
{
    BLOCK_ID_AIR = 0,
    BLOCK_ID_GRAS = 1,
    BLOCK_ID_WOOD = 2,
    BLOCK_ID_LEAVES = 3,
} world_block_id;

#define WORLD_CHUNK_WIDTH 16
#define WORLD_CHUNK_HEIGHT 256
#define LOADED_CHUNKS 16
#define VIEW_DISTANCE 4

// #define CHUNKS_AROUND_PLAYER 2
// #define REGION_SIZE ((CHUNKS_AROUND_PLAYER*2)+1)

typedef struct world_quad
{
    vertex Vertices[4];
    f32 Distance;
}world_quad;

typedef struct world_block
{
    u8 Id;
} world_block;

#include "mesh.c"
#include "chunk.c"
#include "chunkmap.c"

typedef struct entity
{
    f32 x, y, z;
} entity;

typedef struct world
{
    entity Entities[256];
    chunk_map ChunkMap;
} world;



void World_Create(world *World)
{
    World->ChunkMap = ChunkMap_Create();
}

void World_Update(world *World, const camera Camera)
{
    i32 xMin = F32_FloorToI32(Camera.Position.x / CHUNK_WIDTH) - LOADED_CHUNKS / 2;
    i32 zMin = F32_FloorToI32(Camera.Position.z / CHUNK_WIDTH) - LOADED_CHUNKS / 2;

    for (i32 x = 0; x < LOADED_CHUNKS; x++)
    {
        for (i32 z = 0; z < LOADED_CHUNKS; z++)
        {
            chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, x + xMin, z + zMin);
            if (Chunk) continue;
            ChunkMap_AllocateChunk(&World->ChunkMap, x + xMin, z + zMin);
        }
    }
}

void World_DrawChunk(chunk *Chunk, const bitmap Target, bitmap TerrainTexture, const camera Camera)
{
    if (!Chunk) return;
    Chunk_Draw(Camera, Target, TerrainTexture, Chunk);
}

void World_Draw(world *World, const bitmap Target, bitmap TerrainTexture, const camera Camera)
{
    i32 xMid = F32_FloorToI32(Camera.Position.x / CHUNK_WIDTH);
    i32 zMid = F32_FloorToI32(Camera.Position.z / CHUNK_WIDTH);
    i32 xMin = xMid - VIEW_DISTANCE;
    i32 zMin = zMid - VIEW_DISTANCE;
    i32 xMax = xMid + VIEW_DISTANCE;
    i32 zMax = zMid + VIEW_DISTANCE;

    vec3 Forward = Camera_Forward(Camera);

    if (Abs(Forward.x) < Abs(Forward.z))
    {
        for (i32 z = zMin; z < zMid; ++z)
        {
            for (i32 x = xMin; x < xMid; ++x)
            {
                World_DrawChunk(ChunkMap_GetChunk(&World->ChunkMap, x, z), Target, TerrainTexture, Camera);
            }
            for (i32 x = xMax; x >= xMid; --x)
            {
                World_DrawChunk(ChunkMap_GetChunk(&World->ChunkMap, x, z), Target, TerrainTexture, Camera);
            }
        }
        for (i32 z = zMax; z >= zMid; --z)
        {
            for (i32 x = xMin; x < xMid; ++x)
            {
                World_DrawChunk(ChunkMap_GetChunk(&World->ChunkMap, x, z), Target, TerrainTexture, Camera);
            }
            for (i32 x = xMax; x >= xMid; --x)
            {
                World_DrawChunk(ChunkMap_GetChunk(&World->ChunkMap, x, z), Target, TerrainTexture, Camera);
            }
        }
    }
    else
    {
        for (i32 x = xMin; x < xMid; ++x)
        {
            for (i32 z = zMin; z < zMid; ++z)
            {
                World_DrawChunk(ChunkMap_GetChunk(&World->ChunkMap, x, z), Target, TerrainTexture, Camera);
            }
            for (i32 z = zMax; z >= zMid; --z)
            {
                World_DrawChunk(ChunkMap_GetChunk(&World->ChunkMap, x, z), Target, TerrainTexture, Camera);
            }
        }
        for (i32 x = xMax; x >= xMid; --x)
        {
            for (i32 z = zMin; z < zMid; ++z)
            {
                World_DrawChunk(ChunkMap_GetChunk(&World->ChunkMap, x, z), Target, TerrainTexture, Camera);
            }
            for (i32 z = zMax; z >= zMid; --z)
            {
                World_DrawChunk(ChunkMap_GetChunk(&World->ChunkMap, x, z), Target, TerrainTexture, Camera);
            }
        }
    }
}