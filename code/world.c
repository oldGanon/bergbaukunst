
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
    BLOCK_FACE_LEFT,
    BLOCK_FACE_RIGHT,
    BLOCK_FACE_FRONT,
    BLOCK_FACE_BACK,
    BLOCK_FACE_BOTTOM,
    BLOCK_FACE_TOP,
} block_face;

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

block World_GetBlock(world *World, vec3 Position)
{
    if ((Position.y < 0) || (Position.y > CHUNK_HEIGHT - 1))
        return (block){ 0 };

    ivec3 iPosition = Vec3_FloorToIVec3(Position);

    i32 ChunkX = iPosition.x >> CHUNK_WIDTH_SHIFT;
    i32 ChunkZ = iPosition.z >> CHUNK_WIDTH_SHIFT;

    chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, ChunkX, ChunkZ);
    if (!Chunk) return (block){ 0 }; // maybe generate chunk instead?

    i32 BlockX = iPosition.x & CHUNK_WIDTH_MASK;
    i32 BlockY = iPosition.y & CHUNK_HEIGHT_MASK;
    i32 BlockZ = iPosition.z & CHUNK_WIDTH_MASK;

    return Chunk->Blocks[BlockX][BlockZ][BlockY];
}

void World_SetBlock(world *World, vec3 Position, block Block)
{
    if ((Position.y < 0) || (Position.y > CHUNK_HEIGHT - 1))
        return;

    ivec3 iPosition = Vec3_FloorToIVec3(Position);

    i32 ChunkX = iPosition.x >> CHUNK_WIDTH_SHIFT;
    i32 ChunkZ = iPosition.z >> CHUNK_WIDTH_SHIFT;

    chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, ChunkX, ChunkZ);
    if (!Chunk) return; // maybe generate chunk instead?

    i32 BlockX = iPosition.x & CHUNK_WIDTH_MASK;
    i32 BlockY = iPosition.y & CHUNK_HEIGHT_MASK;
    i32 BlockZ = iPosition.z & CHUNK_WIDTH_MASK;

    Chunk->Blocks[BlockX][BlockZ][BlockY] = Block;

    Chunk_GenerateMesh(Chunk);
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
        if ((RaySign.y < 0) && (RayPosition.y < 0))
            return false;
        if ((RaySign.y > 0) && (RayPosition.y > CHUNK_HEIGHT - 1))
            return false;

        block Block = World_GetBlock(World, RayPosition);
        if (Block.Id != BLOCK_ID_AIR)
        {
            *Result = (trace_result) {
                .HitPosition = Add(Origin, Mul(Direction, Vec3_Set1(t))),
                .Block = Block,
                .BlockFace = LastFace,
                .BlockPosition = RayPosition,
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
        else if ((tMax.z <= tMax.x) && (tMax.z <= tMax.y))
        {
            t = tMax.z;
            tMax.z += tDelta.z;
            RayPosition.z += RaySign.z;
            if (RaySign.z > 0)
            {
                LastFace = BLOCK_FACE_FRONT;
            }
            else
            {
                LastFace = BLOCK_FACE_BACK;
            }
        }
        else if ((tMax.y <= tMax.x) && (tMax.y <= tMax.z))
        {
            t = tMax.y;
            tMax.y += tDelta.y;
            RayPosition.y += RaySign.y;
            if (RaySign.y > 0)
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
        case BLOCK_FACE_FRONT:  TraceResult.BlockPosition.z -= 1; break;
        case BLOCK_FACE_BACK:   TraceResult.BlockPosition.z += 1; break;
        case BLOCK_FACE_BOTTOM: TraceResult.BlockPosition.y -= 1; break;
        case BLOCK_FACE_TOP:    TraceResult.BlockPosition.y += 1; break;
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
