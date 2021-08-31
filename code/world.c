
enum
{
    BLOCK_ID_AIR = 0,
    BLOCK_ID_GRAS = 1,
    BLOCK_ID_WOOD = 2,
    BLOCK_ID_LEAVES = 3,
} world_block_id;

#define WORLD_CHUNK_WIDTH 16
#define WORLD_CHUNK_HEIGHT 256
#define LOADED_CHUNKS 8
#define VIEW_DISTANCE 2

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
    vec3 ChunkDim = (vec3) { CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH };
    vec3 ChunkMin = Vec3_Mul(ChunkDim, (vec3) { (f32)Chunk->x, 0, (f32)Chunk->z});
    vec3 ChunkMax = Vec3_Add(ChunkDim, ChunkMin);
    if (Camera_BoxVisible(Camera, Target, ChunkMin, ChunkMax))
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
/*
void Chunk_BlockUnderPlayer(world *World, camera Camera)
{
    i32 xMid = F32_FloorToI32(Camera.Position.x / CHUNK_WIDTH);
    i32 zMid = F32_FloorToI32(Camera.Position.z / CHUNK_WIDTH);
    chunk* Chunk = ChunkMap_GetChunk(&World->ChunkMap, xMid, zMid);
    i32 XP = (i32)Camera.Position.x % CHUNK_WIDTH;
    if(XP < 0)
    {
        XP += CHUNK_WIDTH;
    }
    i32 ZP = (i32)Camera.Position.z % 16;
    if (ZP < 0) 
    {
        ZP += CHUNK_WIDTH;
    }
    i32 YP = Camera.Position.y;
    if (YP > 0 && YP < 256 && Chunk->Blocks[XP][ZP][YP-2].Id == 0) 
    {
        Chunk->Blocks[XP][ZP][YP-2].Id = 1;
        Chunk_GatherQuads(Chunk);
    }
}*/

void Block_PlayerLookingAt(world* World, camera Camera)
{
    vec3 RayDir = { 0, 0, 0 };
    vec3 MapSection = { F32_FloorToI32(Camera.Position.x) % 16, F32_FloorToI32(Camera.Position.y), F32_FloorToI32(Camera.Position.z) % 16};
    if(MapSection.x <0)
    {
        MapSection.x = MapSection.x + 16;
    }
    if (MapSection.z < 0)
    {
        MapSection.z = MapSection.z + 16;
    }
    f32 RayLen = 0;
    vec3 Dir = Camera_Direction(Camera);
    if(Dir.x == 0)
    {
        Dir.x = 0.00000001f;
    }
    if (Dir.y == 0)
    {
        Dir.y = 0.00000001f;
    }
    if (Dir.z == 0)
    {
        Dir.z = 0.00000001f;
    }
    vec3 RayStepSize = { Sqrt(1 + ((ABS(Dir.z) + ABS(Dir.y)) / Dir.x) * ((ABS(Dir.z) + ABS(Dir.y)) / Dir.x)),
                         Sqrt(1 + ((ABS(Dir.x) + ABS(Dir.z)) / Dir.y) * ((ABS(Dir.x) + ABS(Dir.z)) / Dir.y)),
                         Sqrt(1 + ((ABS(Dir.x) + ABS(Dir.y)) / Dir.z) * ((ABS(Dir.x) + ABS(Dir.y)) / Dir.z))};

    vec3 RayStepSizeFirst = {0};


    
    if(Dir.x > 0)
    {
        RayDir.x = 1;
        RayStepSizeFirst.x = (1 - ((Camera.Position.x - F32_FloorToI32(Camera.Position.x))))*RayStepSize.x;
    }
    else
    {
        RayDir.x = -1;
        RayStepSizeFirst.x = (Camera.Position.x - (F32_FloorToI32(Camera.Position.x))) * RayStepSize.x;
    }
    if (Dir.y > 0) 
    {
        RayDir.y = 1;
        RayStepSizeFirst.y = (1 - ((Camera.Position.y - F32_FloorToI32(Camera.Position.y)))) * RayStepSize.y;
    }
    else
    {
        RayDir.y = -1;
        RayStepSizeFirst.y = (Camera.Position.y - (F32_FloorToI32(Camera.Position.y))) * RayStepSize.y;
    }
    if(Dir.z > 0)
    {
        RayDir.z = 1;
        RayStepSizeFirst.z = (1 - ((Camera.Position.z - F32_FloorToI32(Camera.Position.z)))) * RayStepSize.z;
    }
    else
    {
        RayDir.z = -1;
        RayStepSizeFirst.z = (Camera.Position.z - (F32_FloorToI32(Camera.Position.z))) * RayStepSize.z;
    }

    i32 xMid = F32_FloorToI32(Camera.Position.x / CHUNK_WIDTH);
    i32 zMid = F32_FloorToI32(Camera.Position.z / CHUNK_WIDTH);


    for (i32 i = 0; i < 10; i++)
    {

        chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, xMid, zMid);

        if (RayLen < 5 && MapSection.y < 256 && MapSection.y >= 0 && Chunk->Blocks[(i32)MapSection.x][(i32)MapSection.z][(i32)MapSection.y].Id != 0)
        {
            Chunk->Blocks[(i32)MapSection.x][(i32)MapSection.z][(i32)MapSection.y].Id = 0;
            //Chunk_GatherQuads(Chunk);
            Chunk->Mesh = Mesh_Create();
            Chunk_GenerateMesh(Chunk);
            return;
        }
        if (RayStepSizeFirst.x <= RayStepSizeFirst.y && RayStepSizeFirst.x <= RayStepSizeFirst.z)
        {
            MapSection.x += RayDir.x;
            RayLen = RayStepSizeFirst.x;
            RayStepSizeFirst.x += RayStepSize.x;
        }
        else if (RayStepSizeFirst.y <= RayStepSizeFirst.x && RayStepSizeFirst.y <= RayStepSizeFirst.z)
        {
            MapSection.y += RayDir.y;
            RayLen = RayStepSizeFirst.y;
            RayStepSizeFirst.y += RayStepSize.y;
        }
        else if (RayStepSizeFirst.z <= RayStepSizeFirst.x && RayStepSizeFirst.z <= RayStepSizeFirst.y)
        {
            MapSection.z += RayDir.z;
            RayLen = RayStepSizeFirst.z;
            RayStepSizeFirst.z += RayStepSize.z;
        }

        if((i32)MapSection.x > 15)
        {
            xMid += (i32)MapSection.x / 16;
            MapSection.x = (i32)MapSection.x % 16;
        }
        else if ((i32)MapSection.x < 0)
        {
            xMid += (i32)MapSection.x / 16 -1;
            MapSection.x = (i32)MapSection.x % 16 + 16;
        }
        if ((i32)MapSection.z > 15)
        {
            zMid += (i32)MapSection.z / 16;
            MapSection.z = (i32)MapSection.z % 16;
        }
        else if ((i32)MapSection.z < 0)
        {
            zMid += (i32)MapSection.z / 16 -1;
            MapSection.z = (i32)MapSection.z % 16 + 16;
        }
    }
}
