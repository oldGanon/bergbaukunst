
inline ivec3 World_ToBlockPosition(ivec3 WorldPosition);
inline ivec2 World_ToChunkPosition(ivec3 WorldPosition);

#include "mesh.c"
#include "block.c"
#include "chunk.c"
#include "chunkmap.c"

#define LOADED_CHUNKS 16
#define VIEW_DISTANCE 2

typedef struct entity
{
    f32 x, y, z;
} entity;

typedef struct world
{
    entity Entities[256];
    chunk_map ChunkMap;
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

typedef struct trace_result
{
    block Block;
    block_face BlockFace;

    vec3 BlockPosition;
    vec3 FreePosition;
} trace_result;



void World_Create(world *World)
{
    World->ChunkMap = ChunkMap_Create();
}

void World_Update(world *World, const camera Camera)
{
    ivec2 HalfLoadedDim = iVec2_Set1(LOADED_CHUNKS >> 1);
    ivec2 CenterChunk = World_ToChunkPosition(Vec3_FloorToIVec3(Camera.Position));
    ivec2 MinPos = iVec2_Sub(CenterChunk, HalfLoadedDim);
    ivec2 MaxPos = iVec2_Add(CenterChunk, HalfLoadedDim);

    for (i32 x = MinPos.x; x <= MaxPos.x; x++)
    {
        for (i32 y = MinPos.y; y <= MaxPos.y; y++)
        {
            chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, x, y);
            if (!Chunk) ChunkMap_AllocateChunk(&World->ChunkMap, x, y);
        }
    }
}

void World_DrawChunk(world *World, i32 x, i32 y, const bitmap Target, bitmap TerrainTexture, const camera Camera)
{
    chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, x, y);
    if (!Chunk) return;
    Chunk_Draw(Camera, Target, TerrainTexture, Chunk);
}

void World_Draw(world *World, const bitmap Target, bitmap TerrainTexture, const camera Camera)
{
    ivec3 iWorldPosition = Vec3_FloorToIVec3(Camera.Position);
    ivec2 Mid = World_ToChunkPosition(iWorldPosition);
    ivec2 Min = iVec2_Sub(Mid, iVec2_Set1(VIEW_DISTANCE));
    ivec2 Max = iVec2_Add(Mid, iVec2_Set1(VIEW_DISTANCE));

    vec3 Forward = Camera_Forward(Camera);

    if (Abs(Forward.x) < Abs(Forward.y))
    {
        for (i32 y = Min.y; y < Mid.y; ++y)
        {
            for (i32 x = Min.x; x < Mid.x; ++x)
            {
                World_DrawChunk(World, x, y, Target, TerrainTexture, Camera);
            }
            for (i32 x = Max.x; x >= Mid.x; --x)
            {
                World_DrawChunk(World, x, y, Target, TerrainTexture, Camera);
            }
        }
        for (i32 y = Max.y; y >= Mid.y; --y)
        {
            for (i32 x = Min.x; x < Mid.x; ++x)
            {
                World_DrawChunk(World, x, y, Target, TerrainTexture, Camera);
            }
            for (i32 x = Max.x; x >= Mid.x; --x)
            {
                World_DrawChunk(World, x, y, Target, TerrainTexture, Camera);
            }
        }
    }
    else
    {
        for (i32 x = Min.x; x < Mid.x; ++x)
        {
            for (i32 y = Min.y; y < Mid.y; ++y)
            {
                World_DrawChunk(World, x, y, Target, TerrainTexture, Camera);
            }
            for (i32 y = Max.y; y >= Mid.y; --y)
            {
                World_DrawChunk(World, x, y, Target, TerrainTexture, Camera);
            }
        }
        for (i32 x = Max.x; x >= Mid.x; --x)
        {
            for (i32 y = Min.y; y < Mid.y; ++y)
            {
                World_DrawChunk(World, x, y, Target, TerrainTexture, Camera);
            }
            for (i32 y = Max.y; y >= Mid.y; --y)
            {
                World_DrawChunk(World, x, y, Target, TerrainTexture, Camera);
            }
        }
    }
}

inline ivec3 World_ToBlockPosition(ivec3 WorldPosition)
{
    ivec3 Mask = (ivec3){ CHUNK_WIDTH_MASK, CHUNK_WIDTH_MASK, CHUNK_HEIGHT_MASK };
    return iVec3_And(WorldPosition, Mask);
}

inline ivec2 World_ToChunkPosition(ivec3 WorldPosition)
{
    return iVec2_ShiftRight(WorldPosition.xy, CHUNK_WIDTH_SHIFT);
}

block World_GetBlock(const world *World, vec3 WorldPosition)
{
    if (WorldPosition.z < 0)                return DEFAULT_SKY_BLOCK;
    if (WorldPosition.z > CHUNK_HEIGHT - 1) return DEFAULT_HELL_BLOCK;

    ivec3 iWorldPosition = Vec3_FloorToIVec3(WorldPosition);
    ivec2 ChunkPosition = World_ToChunkPosition(iWorldPosition);
    const chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, ChunkPosition.x, ChunkPosition.y);
    if (!Chunk) return (block){ 0 }; // maybe generate chunk instead?
    return Chunk_GetBlock(Chunk, iWorldPosition);
}

void World_SetBlock(world *World, vec3 WorldPosition, block Block)
{
    if ((WorldPosition.z < 0) || (WorldPosition.z > CHUNK_HEIGHT - 1))
        return;

    ivec3 iWorldPosition = Vec3_FloorToIVec3(WorldPosition);
    ivec2 ChunkPosition = World_ToChunkPosition(iWorldPosition);
    chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, ChunkPosition.x, ChunkPosition.y);
    if (!Chunk) return; // maybe generate chunk instead?
    Chunk_SetBlock(Chunk, iWorldPosition, Block);
}

f32 World_TraceRay(world *World, vec3 RayOrigin, vec3 RayDirection, f32 RayLength, trace_result *Result)
{
    vec3 RayPosition = RayOrigin;
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
            return RayLength;
        if ((RaySign.z < 0) && (RayPosition.z < 0))
            return INFINITY;
        if ((RaySign.z > 0) && (RayPosition.z > CHUNK_HEIGHT - 1))
            return INFINITY;

        vec3 BlockPosition = Floor(RayPosition);
        block Block = World_GetBlock(World, BlockPosition);
        if (Block_TraceRay(Block, BlockPosition, RayOrigin, RayDirection) < RayLength)
        {
            vec3 FreePosition = Add(BlockPosition, BlockFace_Normal[LastFace]);
            *Result = (trace_result) {
                .Block = Block,
                .BlockFace = LastFace,
                .BlockPosition = BlockPosition,
                .FreePosition = FreePosition,
            };
            return t;
        }

        if ((tMax.x <= tMax.y) && (tMax.x <= tMax.z))
        {
            t = tMax.x;
            tMax.x += tDelta.x;
            RayPosition.x += RaySign.x;
            if(RaySign.x > 0) LastFace = BLOCK_FACE_LEFT;
            else              LastFace = BLOCK_FACE_RIGHT;
        }
        else if ((tMax.y <= tMax.x) && (tMax.y <= tMax.z))
        {
            t = tMax.y;
            tMax.y += tDelta.y;
            RayPosition.y += RaySign.y;
            if (RaySign.y > 0) LastFace = BLOCK_FACE_FRONT;
            else               LastFace = BLOCK_FACE_BACK;
        }
        else if ((tMax.z <= tMax.x) && (tMax.z <= tMax.y))
        {
            t = tMax.z;
            tMax.z += tDelta.z;
            RayPosition.z += RaySign.z;
            if (RaySign.z > 0) LastFace = BLOCK_FACE_BOTTOM;
            else               LastFace = BLOCK_FACE_TOP;
        }
    }
}

void Block_Highlight(bitmap Buffer, camera Camera, trace_result TraceResult)
{
    vec3 BlockCorner = Vec3_Floor(TraceResult.BlockPosition);

    Draw_Line(Buffer, COLOR_WHITE, Camera_WorldToScreen(Camera, Buffer, BlockCorner),
                                   Camera_WorldToScreen(Camera, Buffer, (vec3) { BlockCorner.x + 1, BlockCorner.y, BlockCorner.z }));

    Draw_Line(Buffer, COLOR_WHITE, Camera_WorldToScreen(Camera, Buffer, BlockCorner),
                                   Camera_WorldToScreen(Camera, Buffer, (vec3) { BlockCorner.x, BlockCorner.y, BlockCorner.z + 1 }));

    Draw_Line(Buffer, COLOR_WHITE, Camera_WorldToScreen(Camera, Buffer, (vec3) { BlockCorner.x + 1, BlockCorner.y, BlockCorner.z}),
                                   Camera_WorldToScreen(Camera, Buffer, (vec3) { BlockCorner.x + 1, BlockCorner.y, BlockCorner.z + 1 }));

    Draw_Line(Buffer, COLOR_WHITE, Camera_WorldToScreen(Camera, Buffer, (vec3) { BlockCorner.x + 1, BlockCorner.y, BlockCorner.z + 1 }),
                                   Camera_WorldToScreen(Camera, Buffer, (vec3) { BlockCorner.x    , BlockCorner.y, BlockCorner.z + 1 }));
}
