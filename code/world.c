
inline ivec3 World_ToBlockPosition(ivec3 WorldPosition);
inline ivec2 World_ToChunkPosition(ivec3 WorldPosition);

#include "block.c"
#include "chunk.c"
#include "chunkmap.c"
#include "worldgen.c"

#define LOADED_CHUNKS_DIST 16

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



void World_Init(world *World)
{
    World->ChunkMap = ChunkMap_Create();
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



inline ivec3 World_ToBlockPosition(ivec3 WorldPosition)
{
    ivec3 Mask = (ivec3){ CHUNK_WIDTH_MASK, CHUNK_WIDTH_MASK, CHUNK_HEIGHT_MASK };
    return iVec3_And(WorldPosition, Mask);
}

inline ivec2 World_ToChunkPosition(ivec3 WorldPosition)
{
    return iVec2_ShiftRight(WorldPosition.xy, CHUNK_WIDTH_SHIFT);
}

chunk *World_GetChunk(const world *World, ivec2 ChunkPosition)
{
    return ChunkMap_GetChunk(&World->ChunkMap, ChunkPosition);
}

block World_GetBlock(const world *World, vec3 WorldPosition)
{
    if (WorldPosition.z < 0)                return DEFAULT_SKY_BLOCK;
    if (WorldPosition.z > CHUNK_HEIGHT - 1) return DEFAULT_HELL_BLOCK;

    ivec3 iWorldPosition = Vec3_FloorToIVec3(WorldPosition);
    ivec2 ChunkPosition = World_ToChunkPosition(iWorldPosition);
    const chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, ChunkPosition);
    if (!Chunk) return DEFAULT_BLOCK; // maybe generate chunk instead?
    return Chunk_GetBlock(Chunk, iWorldPosition);
}

void World_SetBlock(world *World, vec3 WorldPosition, block Block)
{
    if ((WorldPosition.z < 0) || (WorldPosition.z > CHUNK_HEIGHT - 1))
        return;

    ivec3 iWorldPosition = Vec3_FloorToIVec3(WorldPosition);
    ivec2 ChunkPosition = World_ToChunkPosition(iWorldPosition);
    chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, ChunkPosition);
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

    block_face LastFace = BLOCK_FACE_NONE;

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

box World_BoxIntersection(world *World, box Box)
{
    ivec3 Min = Vec3_FloorToIVec3(Box.Min);
    ivec3 Max = Vec3_CeilToIVec3(Box.Max);

    box Intersection = BOX_EMPTY;

    for (i32 z = Min.z; z < Max.z; ++z)
    for (i32 y = Min.y; y < Max.y; ++y)
    for (i32 x = Min.x; x < Max.x; ++x)
    {
        ivec3 WorldPosition = { x, y, z };
        vec3 BlockPosition = iVec3_toVec3(WorldPosition);
        block Block = World_GetBlock(World, BlockPosition);
        Intersection = Box_Union(Intersection, Block_BoxIntersection(Block, BlockPosition, Box));
    }

    return Intersection;
}

vec3 World_CheckMoveBox(world *World, box Box, vec3 Move)
{
    {
        box MoveBox = Box;
        if (Move.x < 0) { MoveBox.Max.x = MoveBox.Min.x; MoveBox.Min.x += Move.x;}
        if (Move.x > 0) { MoveBox.Min.x = MoveBox.Max.x; MoveBox.Max.x += Move.x;}
        box CollisionBox = World_BoxIntersection(World, MoveBox);
        if (!Box_Empty(CollisionBox))
        {
            if (Move.x < 0) Move.x = CollisionBox.Max.x - Box.Min.x;
            if (Move.x > 0) Move.x = CollisionBox.Min.x - Box.Max.x;
        }
        Box.Min.x += Move.x;
        Box.Max.x += Move.x;
    }

    {
        box MoveBox = Box;
        if (Move.y < 0) { MoveBox.Max.y = MoveBox.Min.y; MoveBox.Min.y += Move.y; }
        if (Move.y > 0) { MoveBox.Min.y = MoveBox.Max.y; MoveBox.Max.y += Move.y; }
        box CollisionBox = World_BoxIntersection(World, MoveBox);
        if (!Box_Empty(CollisionBox))
        {
            if (Move.y < 0) Move.y = CollisionBox.Max.y - Box.Min.y;
            if (Move.y > 0) Move.y = CollisionBox.Min.y - Box.Max.y;
        }
        Box.Min.y += Move.y;
        Box.Max.y += Move.y;
    }

    {
        box MoveBox = Box;
        if (Move.z < 0) { MoveBox.Max.z = MoveBox.Min.z; MoveBox.Min.z += Move.z; }
        if (Move.z > 0) { MoveBox.Min.z = MoveBox.Max.z; MoveBox.Max.z += Move.z; }
        box CollisionBox = World_BoxIntersection(World, MoveBox);
        if (!Box_Empty(CollisionBox))
        {
            if (Move.z < 0) Move.z = CollisionBox.Max.z - Box.Min.z;
            if (Move.z > 0) Move.z = CollisionBox.Min.z - Box.Max.z;
        }
        Box.Min.z += Move.z;
        Box.Max.z += Move.z;
    }

    return Move;
}
