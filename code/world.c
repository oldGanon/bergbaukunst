
inline ivec3 World_ToBlockPosition(ivec3 WorldPosition);
inline ivec2 World_ToChunkPosition(ivec3 WorldPosition);

typedef struct chunk chunk;
void WorldGen_GenerateChunk(chunk *Chunk);

#include "mesh.c"
#include "block.c"
#include "chunk.c"
#include "chunkmap.c"
#include "worldgen.c"

#define LOADED_CHUNKS 32
#define VIEW_DISTANCE 12

typedef struct entity
{
    f32 x, y, z;
} entity;

typedef struct world
{
    entity Entities[256];
    chunk_map ChunkMap;

    u32 MeshGenerationBudget;
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

void World_Update(world *World, const camera Camera)
{
    World->MeshGenerationBudget = 1;

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



typedef struct padded_chunk
{
    block Blocks[CHUNK_HEIGHT+2][CHUNK_WIDTH+2][CHUNK_WIDTH+2];
} padded_chunk;

void World_PaddedChunk(world *World, chunk *Chunk, padded_chunk *PaddedChunk)
{
    for (i32 y = 0; y < CHUNK_WIDTH+2; ++y)
    for (i32 x = 0; x < CHUNK_WIDTH+2; ++x)
    {
        PaddedChunk->Blocks[0][y][x] = DEFAULT_HELL_BLOCK;
        PaddedChunk->Blocks[CHUNK_HEIGHT+1][y][x] = DEFAULT_SKY_BLOCK;
    }

    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (i32 y = 0; y < CHUNK_WIDTH; ++y)
    for (i32 x = 0; x < CHUNK_WIDTH; ++x)
    {
        PaddedChunk->Blocks[z+1][y+1][x+1] = Chunk->Blocks[z][y][x];
    }

    if (Chunk->Neighbors[0][0])
    {
        chunk *Chunk2 = ChunkMap_GetChunkById(&World->ChunkMap, Chunk->Neighbors[0][0]);
        for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
        {
            PaddedChunk->Blocks[z+1][0][0] = Chunk2->Blocks[z][CHUNK_WIDTH-1][CHUNK_WIDTH-1];
        }
    }

    if (Chunk->Neighbors[0][2])
    {
        chunk *Chunk2 = ChunkMap_GetChunkById(&World->ChunkMap, Chunk->Neighbors[0][2]);
        for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
        {
            PaddedChunk->Blocks[z+1][0][CHUNK_WIDTH+1] = Chunk2->Blocks[z][CHUNK_WIDTH-1][0];
        }
    }

    if (Chunk->Neighbors[2][0])
    {
        chunk *Chunk2 = ChunkMap_GetChunkById(&World->ChunkMap, Chunk->Neighbors[2][0]);
        for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
        {
            PaddedChunk->Blocks[z+1][CHUNK_WIDTH+1][0] = Chunk2->Blocks[z][0][CHUNK_WIDTH-1];
        }
    }

    if (Chunk->Neighbors[2][2])
    {
        chunk *Chunk2 = ChunkMap_GetChunkById(&World->ChunkMap, Chunk->Neighbors[2][2]);
        for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
        {
            PaddedChunk->Blocks[z+1][CHUNK_WIDTH+1][CHUNK_WIDTH+1] = Chunk2->Blocks[z][0][0];
        }
    }

    if (Chunk->Neighbors[0][1])
    {
        chunk *Chunk2 = ChunkMap_GetChunkById(&World->ChunkMap, Chunk->Neighbors[0][1]);
        for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
        for (i32 x = 0; x < CHUNK_WIDTH; ++x)
        {
            PaddedChunk->Blocks[z+1][0][x+1] = Chunk2->Blocks[z][CHUNK_WIDTH-1][x];
        }
    }

    if (Chunk->Neighbors[1][0])
    {
        chunk *Chunk2 = ChunkMap_GetChunkById(&World->ChunkMap, Chunk->Neighbors[1][0]);
        for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
        for (i32 y = 0; y < CHUNK_WIDTH; ++y)
        {
            PaddedChunk->Blocks[z+1][y+1][0] = Chunk2->Blocks[z][y][CHUNK_WIDTH-1];
        }
    }

    if (Chunk->Neighbors[2][1])
    {
        chunk *Chunk2 = ChunkMap_GetChunkById(&World->ChunkMap, Chunk->Neighbors[2][1]);
        for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
        for (i32 x = 0; x < CHUNK_WIDTH; ++x)
        {
            PaddedChunk->Blocks[z+1][CHUNK_WIDTH+1][x+1] = Chunk2->Blocks[z][0][x];
        }
    }

    if (Chunk->Neighbors[1][2])
    {
        chunk *Chunk2 = ChunkMap_GetChunkById(&World->ChunkMap, Chunk->Neighbors[1][2]);
        for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
        for (i32 y = 0; y < CHUNK_WIDTH; ++y)
        {
            PaddedChunk->Blocks[z+1][y+1][CHUNK_WIDTH+1] = Chunk2->Blocks[z][y][0];
        }
    }
}

block_group PaddedChunk_GetBlockGroup(padded_chunk *Chunk, ivec3 WorldPosition)
{
    block_group BlockGroup;
    for (i32 z = 0; z < 3; ++z)
    for (i32 y = 0; y < 3; ++y)
    for (i32 x = 0; x < 3; ++x)
    {
        ivec3 BlockPosition = (ivec3){ x, y, z };
        BlockPosition = iVec3_Add(BlockPosition, WorldPosition);
        BlockGroup.Blocks[z][y][x] = Chunk->Blocks[BlockPosition.z][BlockPosition.y][BlockPosition.x];
    }
    return BlockGroup;
}

void World_GenerateChunkMesh(world *World, chunk *Chunk)
{
    Mesh_Clear(&Chunk->Mesh);

    padded_chunk PaddedChunk = { 0 };
    World_PaddedChunk(World, Chunk, &PaddedChunk);

    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (i32 y = 0; y < CHUNK_WIDTH; ++y)
    for (i32 x = 0; x < CHUNK_WIDTH; ++x)
    {
        ivec3 iBlockPosition = (ivec3){ x, y, z };
        block Block = Chunk_GetBlock(Chunk, iBlockPosition);
        if (Block.Id == BLOCK_ID_AIR) continue;

        block_group BlockGroup = PaddedChunk_GetBlockGroup(&PaddedChunk, iBlockPosition);

        vec3 BlockPosition = iVec3_toVec3(iBlockPosition);
        switch (Block.Id)
        {
            case BLOCK_ID_DIRT:
            {
                Block_AddQuadsToMesh(&Chunk->Mesh, BlockPosition, &BlockGroup, 2, 2, 2, 2, 2, 2);
            } break;

            case BLOCK_ID_GRAS:
            {
                Block_AddQuadsToMesh(&Chunk->Mesh, BlockPosition, &BlockGroup, 1, 1, 1, 1, 2, 0);
            } break;

            case BLOCK_ID_STONE:
            {
                Block_AddQuadsToMesh(&Chunk->Mesh, BlockPosition, &BlockGroup, 5, 5, 5, 5, 5, 5);
            } break;

            case BLOCK_ID_COBBLE:
            {
                Block_AddQuadsToMesh(&Chunk->Mesh, BlockPosition, &BlockGroup, 4, 4, 4, 4, 4, 4);
            } break;

            case BLOCK_ID_WOOD:
            {
                Block_AddQuadsToMesh(&Chunk->Mesh, BlockPosition, &BlockGroup, 7, 7, 7, 7, 15, 15);
            } break;

            case BLOCK_ID_LEAVES:
            {
                Block_AddQuadsToMesh(&Chunk->Mesh, BlockPosition, &BlockGroup, 3, 3, 3, 3, 3, 3);
            } break;

            case BLOCK_ID_SAND:
            {
                Block_AddQuadsToMesh(&Chunk->Mesh, BlockPosition, &BlockGroup, 6, 6, 6, 6, 6, 6);
            } break;
        }
    }

    Chunk->MeshDirty = false;
}



void World_DrawChunk(world *World, i32 x, i32 y, const bitmap Target, bitmap TerrainTexture, const camera Camera)
{
    chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, x, y);
    if (!Chunk) return;
    if (Chunk->MeshDirty && World->MeshGenerationBudget)
    {
        --World->MeshGenerationBudget;
        World_GenerateChunkMesh(World, Chunk);
    }
    Chunk_Draw(Camera, Target, TerrainTexture, Chunk);
}

void World_Draw(world *World, const bitmap Target, bitmap TerrainTexture, const camera Camera)
{
    ivec3 iWorldPosition = Vec3_FloorToIVec3(Camera.Position);
    ivec2 Mid = World_ToChunkPosition(iWorldPosition);
    ivec2 Min = iVec2_Sub(Mid, iVec2_Set1(VIEW_DISTANCE));
    ivec2 Max = iVec2_Add(Mid, iVec2_Set1(VIEW_DISTANCE));

    for (i32 y = Min.y; y <= Max.y; ++y)
    for (i32 x = Min.x; x <= Max.x; ++x)
    {
        World_DrawChunk(World, x, y, Target, TerrainTexture, Camera);
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

void World_MarkChunkDirty(chunk_map *ChunkMap, u64 ChunkId)
{
    if (!ChunkId) return;
    chunk *Chunk = ChunkMap_GetChunkById(ChunkMap, ChunkId);
    if (!Chunk) return;
    Chunk->MeshDirty = true;
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

    ivec3 BlockPosition = World_ToBlockPosition(iWorldPosition);
    if ((BlockPosition.x == 0))
        World_MarkChunkDirty(&World->ChunkMap, Chunk->Neighbors[1][0]);
    if ((BlockPosition.y == 0))
        World_MarkChunkDirty(&World->ChunkMap, Chunk->Neighbors[0][1]);
    if ((BlockPosition.x == CHUNK_WIDTH - 1))
        World_MarkChunkDirty(&World->ChunkMap, Chunk->Neighbors[1][2]);
    if ((BlockPosition.y == CHUNK_WIDTH - 1))
        World_MarkChunkDirty(&World->ChunkMap, Chunk->Neighbors[2][1]);
    if ((BlockPosition.x == 0) && (BlockPosition.y == 0))
        World_MarkChunkDirty(&World->ChunkMap, Chunk->Neighbors[0][0]);
    if ((BlockPosition.x == 0) && (BlockPosition.y == CHUNK_WIDTH - 1))
        World_MarkChunkDirty(&World->ChunkMap, Chunk->Neighbors[2][0]);
    if ((BlockPosition.x == CHUNK_WIDTH - 1) && (BlockPosition.y == 0))
        World_MarkChunkDirty(&World->ChunkMap, Chunk->Neighbors[0][2]);
    if ((BlockPosition.x == CHUNK_WIDTH - 1) && (BlockPosition.y == CHUNK_WIDTH - 1))
        World_MarkChunkDirty(&World->ChunkMap, Chunk->Neighbors[2][2]);
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
