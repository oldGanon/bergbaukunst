
enum
{
    BLOCK_ID_AIR = 0,
    BLOCK_ID_GRAS = 1,
    BLOCK_ID_WOOD = 2,
    BLOCK_ID_LEAVES = 3,
} world_block_id;

typedef struct world_block
{
    u8 Id;
} world_block;

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

enum
{
    Top,
    Front,
    Right,
    Back,
    Left,
    Bottom
}side;

typedef struct block_info
{
    world_block Block;
    vec3 Position;
    enum side BlockSide;
    u8 BlockFound;
}block_info;


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

world_block World_GetBlock(world *World, vec3 Position)
{
    if ((Position.y < 0) || (Position.y > CHUNK_HEIGHT - 1))
        return (world_block){ 0 };

    ivec3 iPosition = Vec3_FloorToIVec3(Position);

    i32 ChunkX = iPosition.x >> CHUNK_WIDTH_SHIFT;
    i32 ChunkZ = iPosition.z >> CHUNK_WIDTH_SHIFT;

    chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, ChunkX, ChunkZ);
    if (!Chunk) return (world_block){ 0 }; // maybe generate chunk instead?

    i32 BlockX = iPosition.x & CHUNK_WIDTH_MASK;
    i32 BlockY = iPosition.y & CHUNK_HEIGHT_MASK;
    i32 BlockZ = iPosition.z & CHUNK_WIDTH_MASK;

    return Chunk->Blocks[BlockX][BlockZ][BlockY];
}

void World_SetBlock(world *World, vec3 Position, world_block Block)
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

block_info Block_PlayerLookingAt(world* World, camera Camera)
{
    vec3 Dir = Camera_Direction(Camera);
    block_info BlockInfo = {0};
    vec3 RayDir = Sign(Dir);
    vec3 RayStepSize = { 1 / Abs(Dir.x), 1 / Abs(Dir.y), 1 / Abs(Dir.z)};

#if 0
    // doesnt work for xyz == 0
    vec3 RayStepSizeFirst = Fract(Mul(Camera.Position, Negate(RayDir)));
#else
    vec3 RayStepSizeFirst = { 0 };
    vec3 FractPos = Fract(Camera.Position);
    if (Dir.x > 0)
    {
        RayStepSizeFirst.x = (1 - FractPos.x);
    }
    else
    {
        RayStepSizeFirst.x = FractPos.x;
    }

    if (Dir.y > 0) 
    {
        RayStepSizeFirst.y = (1 - FractPos.y);
    }
    else
    {
        RayStepSizeFirst.y = FractPos.y;
    }
    
    if(Dir.z > 0)
    {
        RayStepSizeFirst.z = (1 - FractPos.z);
    }
    else
    {
        RayStepSizeFirst.z = FractPos.z;
    }    
#endif
    RayStepSizeFirst = Mul(RayStepSizeFirst, RayStepSize);

    f32 RayLength = 0;
    f32 MaxLength = 5.0f;
    vec3 RayPosition = Camera.Position;
    for (i32 i = 0; i < 10; i++)
    {
        if ((RayPosition.y < 0) && (RayDir.y < 0)) return BlockInfo;
        if ((RayPosition.y > CHUNK_HEIGHT - 1) && (RayDir.y > 0)) return BlockInfo;
        if (RayLength > MaxLength) return BlockInfo;

        world_block Block = World_GetBlock(World, RayPosition);

        if (Block.Id != 0)
        {
            BlockInfo.Position = RayPosition;
            BlockInfo.Block = Block;
            BlockInfo.BlockFound = 1;
            //World_SetBlock(World, RayPosition, (world_block){ 0 });
            return BlockInfo;
        }

        if (RayStepSizeFirst.x <= RayStepSizeFirst.y && RayStepSizeFirst.x <= RayStepSizeFirst.z)
        {
            RayPosition.x += RayDir.x;
            RayLength = RayStepSizeFirst.x;
            RayStepSizeFirst.x += RayStepSize.x;
            if(RayDir.x > 0)
            {
                BlockInfo.BlockSide = Left;
            }
            else
            {
                BlockInfo.BlockSide = Right;

            }
        }
        else if (RayStepSizeFirst.y <= RayStepSizeFirst.x && RayStepSizeFirst.y <= RayStepSizeFirst.z)
        {
            RayPosition.y += RayDir.y;
            RayLength = RayStepSizeFirst.y;
            RayStepSizeFirst.y += RayStepSize.y;
            if (RayDir.y > 0)
            {
                BlockInfo.BlockSide = Bottom;
            }
            else
            {
                BlockInfo.BlockSide = Top;

            }
        }
        else if (RayStepSizeFirst.z <= RayStepSizeFirst.x && RayStepSizeFirst.z <= RayStepSizeFirst.y)
        {
            RayPosition.z += RayDir.z;
            RayLength = RayStepSizeFirst.z;
            RayStepSizeFirst.z += RayStepSize.z;
            if (RayDir.z > 0)
            {
                BlockInfo.BlockSide = Front;
            }
            else
            {
                BlockInfo.BlockSide = Back;

            }
        }
    }
    return BlockInfo;
}

void Block_PlaceOnSide(world *World, block_info BlockInfo)
{
    switch (BlockInfo.BlockSide)
    {
        case Top: 
        {
            BlockInfo.Position.y += 1;
            break;
        }
        case Front:
        {
            BlockInfo.Position.z -= 1;
            break;
        }
        case Right:
        {
            BlockInfo.Position.x += 1;
            break;
        }
        case Back:
        {
            BlockInfo.Position.z += 1;
            break;
        }
        case Left:
        {
            BlockInfo.Position.x -= 1;
            break;
        }
        case Bottom:
        {
            BlockInfo.Position.y -= 1;
            break;
        }
        default:
            break;
    }
    world_block Block = { 0 };
    Block.Id = BLOCK_ID_GRAS;
    World_SetBlock(World, BlockInfo.Position, Block);
}

void Block_Highlight(bitmap Buffer,camera Camera ,block_info BlockInfo)
{
    vec3 BlockCorner = Vec3_Floor(BlockInfo.Position);

    Draw_Line(Buffer, COLOR_WHITE, Camera_WorldToScreen(Camera, Buffer, BlockCorner),
                                   Camera_WorldToScreen(Camera, Buffer, (vec3) { BlockCorner.x + 1, BlockCorner.y, BlockCorner.z }));
}
