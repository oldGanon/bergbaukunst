
#define CHUNK_WIDTH_SHIFT 4
#define CHUNK_WIDTH (1 << CHUNK_WIDTH_SHIFT)
#define CHUNK_WIDTH_MASK (CHUNK_WIDTH - 1)
#define CHUNK_HEIGHT_SHIFT 7
#define CHUNK_HEIGHT (1 << CHUNK_HEIGHT_SHIFT)
#define CHUNK_HEIGHT_MASK (CHUNK_HEIGHT - 1)

typedef struct chunk
{
    bool Allocated;
    ivec2 Position;

    block Blocks[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_WIDTH];
} chunk;

void Chunk_Init(chunk *Chunk, ivec2 Position);
void Chunk_Clear(chunk *Chunk, ivec2 Position);
block Chunk_GetBlock(const chunk *Chunk, ivec3 WorldPosition);
void Chunk_SetBlock(chunk *Chunk, ivec3 WorldPosition, block Block);
void Chunk_CalcSkyLight(chunk *Chunk);
box Chunk_Box(chunk *Chunk);
box Chunk_BoxIntersection(chunk *Chunk, const box Box);

#define DEFAULT_BLOCK (block){ .Id = BLOCK_ID_AIR, .Shadow = 0x0 }
#define DEFAULT_SKY_BLOCK (block){ .Id = BLOCK_ID_AIR, .Shadow = 0x0 }
#define DEFAULT_HELL_BLOCK (block){ .Id = BLOCK_ID_AIR, .Shadow = 0xF }

/******************/
/* IMPLEMENTATION */
/******************/

inline ivec3 World_ToBlockPosition(ivec3 WorldPosition)
{
    ivec3 Mask = (ivec3){ CHUNK_WIDTH_MASK, CHUNK_WIDTH_MASK, CHUNK_HEIGHT_MASK };
    return iVec3_And(WorldPosition, Mask);
}

inline ivec2 World_ToChunkPosition(ivec3 WorldPosition)
{
    return iVec2_ShiftRight(WorldPosition.xy, CHUNK_WIDTH_SHIFT);
}

void Chunk_CalcSkyLight(chunk *Chunk)
{
    for (i32 y = 0; y < CHUNK_WIDTH; y++)
    for (i32 x = 0; x < CHUNK_WIDTH; x++)
    for (i32 z = CHUNK_HEIGHT - 1; z >= 0; z--)
    {
        block *Block = &Chunk->Blocks[z][y][x];
        Block->Shadow = 0x0;
        if (Block_Opaque[Block->Id])
        {
            Block->Shadow = 0xF;
            while (z-- > 0)
            {
                Block = &Chunk->Blocks[z][y][x];
                Block->Shadow = 0xF;
            }
        }
    }
}

block Chunk_GetBlock(const chunk *Chunk, ivec3 WorldPosition)
{
    if (WorldPosition.z < 0)                return DEFAULT_SKY_BLOCK;
    if (WorldPosition.z > CHUNK_HEIGHT - 1) return DEFAULT_HELL_BLOCK;

    ivec3 BlockPosition = World_ToBlockPosition(WorldPosition);
    return Chunk->Blocks[BlockPosition.z][BlockPosition.y][BlockPosition.x];
}

void Chunk_SetBlock(chunk *Chunk, ivec3 WorldPosition, block Block)
{
    if ((WorldPosition.z < 0) || (WorldPosition.z > CHUNK_HEIGHT - 1))
        return;

    ivec3 BlockPosition = World_ToBlockPosition(WorldPosition);
    Chunk->Blocks[BlockPosition.z][BlockPosition.y][BlockPosition.x] = Block;

    Chunk_CalcSkyLight(Chunk);
}

void Chunk_Init(chunk *Chunk, ivec2 Position)
{
    Chunk->Allocated = true;
    Chunk->Position = Position;
    memset(Chunk->Blocks, 0, sizeof(Chunk->Blocks));
}

void Chunk_Clear(chunk *Chunk, ivec2 Position)
{
    Chunk->Allocated = false;
    assert(Chunk->Position.x == Position.x);
    assert(Chunk->Position.y == Position.y);
}

box Chunk_Box(chunk *Chunk)
{
    vec3 ChunkDim = (vec3) { CHUNK_WIDTH, CHUNK_WIDTH, CHUNK_HEIGHT };
    vec3 ChunkPos = (vec3) { .xy = iVec2_toVec2(Chunk->Position) };
    vec3 ChunkMin = Vec3_Mul(ChunkDim, ChunkPos);
    vec3 ChunkMax = Vec3_Add(ChunkDim, ChunkMin);
    return (box){ .Min = ChunkMin, .Max = ChunkMax };
}

box Chunk_BoxIntersection(chunk *Chunk, const box Box)
{
    vec3 ChunkDim = (vec3) { CHUNK_WIDTH, CHUNK_WIDTH, CHUNK_HEIGHT };
    vec3 ChunkOrigin = (vec3) { .xy = iVec2_toVec2(iVec2_ShiftLeft(Chunk->Position, CHUNK_WIDTH_SHIFT)) };

    ivec3 Min = Vec3_FloorToIVec3(Max(Sub(Box.Min, ChunkOrigin), Vec3_Zero()));
    ivec3 Max = Vec3_CeilToIVec3(Min(Sub(Box.Max, ChunkOrigin), ChunkDim));

    box Intersection = Box_Empty();

    for (i32 z = Min.z; z < Max.z; ++z)
    for (i32 y = Min.y; y < Max.y; ++y)
    for (i32 x = Min.x; x < Max.x; ++x)
    {
        block Block = Chunk->Blocks[z][y][x];
        ivec3 BlockPosition = (ivec3){ x, y, z };
        Intersection = Box_Union(Intersection, Block_BoxIntersection(Block, BlockPosition, Box));
    }

    return Intersection;
}
