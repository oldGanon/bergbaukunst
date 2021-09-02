
enum
{
    BLOCK_ID_AIR = 0,
    BLOCK_ID_GRAS = 1,
    BLOCK_ID_WOOD = 2,
    BLOCK_ID_LEAVES = 3,
} block_id;

typedef struct block
{
    u8 Id;
    u8 Light;
} block;

#define LOADED_CHUNKS 16
#define VIEW_DISTANCE 2

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

typedef enum block_face
{
    BLOCK_FACE_LEFT,   // -x
    BLOCK_FACE_RIGHT,  // +x
    BLOCK_FACE_FRONT,  // -y
    BLOCK_FACE_BACK,   // +y
    BLOCK_FACE_BOTTOM, // +z
    BLOCK_FACE_TOP,    // -z
} block_face;

typedef enum world_direction
{
    DIR_WEST,  // -x
    DIR_EAST,  // +x
    DIR_SOUTH, // -y
    DIR_NORTH, // +y
    DIR_UP,    // +z
    DIR_DOWN,  // -z
} world_direction;

typedef struct trace_result
{
    block Block;
    block_face BlockFace;
    vec3 BlockPosition;
    vec3 HitPosition;
} trace_result;



void World_Create(world *World)
{
    World->ChunkMap = ChunkMap_Create();
}

void World_Update(world *World, const camera Camera)
{
    i32 xMin = F32_FloorToI32(Camera.Position.x / CHUNK_WIDTH) - LOADED_CHUNKS / 2;
    i32 yMin = F32_FloorToI32(Camera.Position.y / CHUNK_WIDTH) - LOADED_CHUNKS / 2;

    for (i32 x = 0; x < LOADED_CHUNKS; x++)
    {
        for (i32 y = 0; y < LOADED_CHUNKS; y++)
        {
            chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, x + xMin, y + yMin);
            if (Chunk) continue;
            ChunkMap_AllocateChunk(&World->ChunkMap, x + xMin, y + yMin);
        }
    }
}

bool World_GetChunkGroup(const world *World, const chunk *Chunk, chunk_group *Result)
{
    Result->Chunks[0][0] = ChunkMap_GetChunk(&World->ChunkMap, Chunk->x - 1, Chunk->y - 1);
    if (!Result->Chunks[0][0]) return false;
    Result->Chunks[0][1] = ChunkMap_GetChunk(&World->ChunkMap, Chunk->x - 1, Chunk->y);
    if (!Result->Chunks[0][1]) return false;
    Result->Chunks[0][2] = ChunkMap_GetChunk(&World->ChunkMap, Chunk->x - 1, Chunk->y + 1);
    if (!Result->Chunks[0][2]) return false;
    Result->Chunks[1][0] = ChunkMap_GetChunk(&World->ChunkMap, Chunk->x, Chunk->y - 1);
    if (!Result->Chunks[1][0]) return false;
    Result->Chunks[1][1] = ChunkMap_GetChunk(&World->ChunkMap, Chunk->x, Chunk->y);
    if (!Result->Chunks[1][1]) return false;
    Result->Chunks[1][2] = ChunkMap_GetChunk(&World->ChunkMap, Chunk->x, Chunk->y + 1);
    if (!Result->Chunks[1][2]) return false;
    Result->Chunks[2][0] = ChunkMap_GetChunk(&World->ChunkMap, Chunk->x + 1, Chunk->y - 1);
    if (!Result->Chunks[2][0]) return false;
    Result->Chunks[2][1] = ChunkMap_GetChunk(&World->ChunkMap, Chunk->x + 1, Chunk->y);
    if (!Result->Chunks[2][1]) return false;
    Result->Chunks[2][2] = ChunkMap_GetChunk(&World->ChunkMap, Chunk->x + 1, Chunk->y + 1);
    if (!Result->Chunks[2][2]) return false;

    return true;
}

void World_DrawChunk(world *World, i32 x, i32 y, const bitmap Target, bitmap TerrainTexture, const camera Camera)
{
    chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, x, y);
    if (!Chunk) return;

    if (Chunk->MeshDirty)
    {
        chunk_group ChunkGroup;
        if (!World_GetChunkGroup(World, Chunk, &ChunkGroup))
            return;
        Chunk_GenerateMesh(ChunkGroup);
    }
    Chunk_Draw(Camera, Target, TerrainTexture, Chunk);
}

void World_Draw(world *World, const bitmap Target, bitmap TerrainTexture, const camera Camera)
{
    i32 xMid = F32_FloorToI32(Camera.Position.x / CHUNK_WIDTH);
    i32 yMid = F32_FloorToI32(Camera.Position.y / CHUNK_WIDTH);
    i32 xMin = xMid - VIEW_DISTANCE;
    i32 yMin = yMid - VIEW_DISTANCE;
    i32 xMax = xMid + VIEW_DISTANCE;
    i32 yMax = yMid + VIEW_DISTANCE;

    vec3 Forward = Camera_Forward(Camera);

    if (Abs(Forward.x) < Abs(Forward.y))
    {
        for (i32 y = yMin; y < yMid; ++y)
        {
            for (i32 x = xMin; x < xMid; ++x)
            {
                World_DrawChunk(World, x, y, Target, TerrainTexture, Camera);
            }
            for (i32 x = xMax; x >= xMid; --x)
            {
                World_DrawChunk(World, x, y, Target, TerrainTexture, Camera);
            }
        }
        for (i32 y = yMax; y >= yMid; --y)
        {
            for (i32 x = xMin; x < xMid; ++x)
            {
                World_DrawChunk(World, x, y, Target, TerrainTexture, Camera);
            }
            for (i32 x = xMax; x >= xMid; --x)
            {
                World_DrawChunk(World, x, y, Target, TerrainTexture, Camera);
            }
        }
    }
    else
    {
        for (i32 x = xMin; x < xMid; ++x)
        {
            for (i32 y = yMin; y < yMid; ++y)
            {
                World_DrawChunk(World, x, y, Target, TerrainTexture, Camera);
            }
            for (i32 y = yMax; y >= yMid; --y)
            {
                World_DrawChunk(World, x, y, Target, TerrainTexture, Camera);
            }
        }
        for (i32 x = xMax; x >= xMid; --x)
        {
            for (i32 y = yMin; y < yMid; ++y)
            {
                World_DrawChunk(World, x, y, Target, TerrainTexture, Camera);
            }
            for (i32 y = yMax; y >= yMid; --y)
            {
                World_DrawChunk(World, x, y, Target, TerrainTexture, Camera);
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

block World_GetBlock(const world *World, vec3 Position)
{
    if ((Position.z < 0) || (Position.z > CHUNK_HEIGHT - 1))
        return (block){ 0 };

    ivec3 iPosition = Vec3_FloorToIVec3(Position);

    i32 ChunkX = iPosition.x >> CHUNK_WIDTH_SHIFT;
    i32 ChunkY = iPosition.y >> CHUNK_WIDTH_SHIFT;

    const chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, ChunkX, ChunkY);
    if (!Chunk) return (block){ 0 }; // maybe generate chunk instead?

    return Chunk_GetBlock(Chunk, iPosition);
}

void World_SetBlock(world *World, vec3 Position, block Block)
{
    if ((Position.z < 0) || (Position.z > CHUNK_HEIGHT - 1))
        return;

    ivec3 iPosition = Vec3_FloorToIVec3(Position);

    i32 ChunkX = iPosition.x >> CHUNK_WIDTH_SHIFT;
    i32 ChunkY = iPosition.y >> CHUNK_WIDTH_SHIFT;

    chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, ChunkX, ChunkY);
    if (!Chunk) return; // maybe generate chunk instead?

    Chunk_SetBlock(Chunk, iPosition, Block);
}

bool World_TraceRay(world *World, vec3 Origin, vec3 Direction, f32 Length, trace_result *Result)
{
    f32 RayLength = Length;
    vec3 RayDirection = Direction;
    vec3 RayPosition = Origin;
    vec3 RaySign = Sign(RayDirection);

    f32 t = 0;
    vec3 tDelta = Min(Inverse(Abs(RayDirection)), Vec3_Set1(0x100000));
    vec3 tInit = Fract(RayPosition);
    if (RaySign.x > 0) tInit.x = (1 - tInit.x);
    if (RaySign.y > 0) tInit.y = (1 - tInit.y);
    if (RaySign.z > 0) tInit.z = (1 - tInit.z);
    vec3 tMax = Mul(tInit, tDelta);

    block_face LastFace = 0;

    for (;;)
    {
        if (t > RayLength)
            return false;
        if ((RaySign.z < 0) && (RayPosition.z < 0))
            return false;
        if ((RaySign.z > 0) && (RayPosition.z > CHUNK_HEIGHT - 1))
            return false;

        block Block = World_GetBlock(World, RayPosition);
        if (Block.Id != BLOCK_ID_AIR)
        {
            vec3 t3 = (vec3){ t, t, t };
            vec3 HitPosition = Add(Origin, Mul(Direction, t3));
            vec3 BlockPosition = Floor(RayPosition);
            *Result = (trace_result) {
                .HitPosition = HitPosition,
                .Block = Block,
                .BlockFace = LastFace,
                .BlockPosition = BlockPosition,
            };
            return true;
        }

        if ((tMax.x <= tMax.y) && (tMax.x <= tMax.z))
        {
            t = tMax.x;
            tMax.x += tDelta.x;
            RayPosition.x += RaySign.x;
            if(RaySign.x > 0)
            {
                LastFace = BLOCK_FACE_LEFT;
            }
            else
            {
                LastFace = BLOCK_FACE_RIGHT;
            }
        }
        else if ((tMax.y <= tMax.x) && (tMax.y <= tMax.z))
        {
            t = tMax.y;
            tMax.y += tDelta.y;
            RayPosition.y += RaySign.y;
            if (RaySign.y > 0)
            {
                LastFace = BLOCK_FACE_FRONT;
            }
            else
            {
                LastFace = BLOCK_FACE_BACK;
            }
        }
        else if ((tMax.z <= tMax.x) && (tMax.z <= tMax.y))
        {
            t = tMax.z;
            tMax.z += tDelta.z;
            RayPosition.z += RaySign.z;
            if (RaySign.z > 0)
            {
                LastFace = BLOCK_FACE_BOTTOM;
            }
            else
            {
                LastFace = BLOCK_FACE_TOP;
            }
        }
    }
}

bool World_TraceCameraRay(world *World, camera Camera, f32 Length, trace_result *Result)
{
    return World_TraceRay(World, Camera.Position, Camera_Direction(Camera), Length, Result);
}

void Block_PlaceOnSide(world *World, trace_result TraceResult)
{
    switch (TraceResult.BlockFace)
    {
        case BLOCK_FACE_LEFT:   TraceResult.BlockPosition.x -= 1; break;
        case BLOCK_FACE_RIGHT:  TraceResult.BlockPosition.x += 1; break;
        case BLOCK_FACE_FRONT:  TraceResult.BlockPosition.y -= 1; break;
        case BLOCK_FACE_BACK:   TraceResult.BlockPosition.y += 1; break;
        case BLOCK_FACE_BOTTOM: TraceResult.BlockPosition.z -= 1; break;
        case BLOCK_FACE_TOP:    TraceResult.BlockPosition.z += 1; break;
        default: break;
    }
    block Block = { .Id = BLOCK_ID_GRAS };
    World_SetBlock(World, TraceResult.BlockPosition, Block);
}

void Block_Highlight(bitmap Buffer, camera Camera, trace_result TraceResult)
{
    vec3 BlockCorner = Vec3_Floor(TraceResult.BlockPosition);

    Draw_Line(Buffer, COLOR_WHITE, Camera_WorldToScreen(Camera, Buffer, BlockCorner),
                                   Camera_WorldToScreen(Camera, Buffer, (vec3) { BlockCorner.x + 1, BlockCorner.y, BlockCorner.z }));
}
