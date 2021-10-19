
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
    u8 Shades[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_WIDTH];
} chunk;

void Chunk_Init(chunk *Chunk, ivec2 Position);
void Chunk_Clear(chunk *Chunk, ivec2 Position);
block Chunk_GetBlock(const chunk *Chunk, ivec3 WorldPosition);
void Chunk_SetBlock(chunk *Chunk, ivec3 WorldPosition, block Block);
void Chunk_CalcSkyLight(chunk *Chunk);
box Chunk_Box(chunk *Chunk);
box Chunk_BoxIntersection(chunk *Chunk, const box Box);

#define DEFAULT_BLOCK (block){ .Id = BLOCK_ID_AIR }
#define DEFAULT_SKY_BLOCK (block){ .Id = BLOCK_ID_AIR }
#define DEFAULT_HELL_BLOCK (block){ .Id = BLOCK_ID_AIR }

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

/******************/
/*  PADDED CHUNK  */
/******************/

typedef struct block_group
{
    block Blocks[3][3][3];
    u8 Shades[3][3][3];
} block_group;

typedef struct padded_chunk
{
    block Blocks[CHUNK_HEIGHT+2][CHUNK_WIDTH+2][CHUNK_WIDTH+2];
    u8 Shades[CHUNK_HEIGHT+2][CHUNK_WIDTH+2][CHUNK_WIDTH+2];
} padded_chunk;

typedef const chunk *get_chunk_func(const void *Data, ivec2 Position);

void Chunk_Pad(get_chunk_func *GetChunk, const void *Data, ivec2 ChunkPosition, padded_chunk *PaddedChunk)
{
    for (i32 y = 0; y < CHUNK_WIDTH+2; ++y)
    for (i32 x = 0; x < CHUNK_WIDTH+2; ++x)
    {
        PaddedChunk->Blocks[0][y][x] = DEFAULT_HELL_BLOCK;
        PaddedChunk->Blocks[CHUNK_HEIGHT+1][y][x] = DEFAULT_SKY_BLOCK;
        PaddedChunk->Shades[CHUNK_HEIGHT+1][y][x] = 0;
    }

    const chunk *Chunk = GetChunk(Data, ChunkPosition);
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (i32 y = 0; y < CHUNK_WIDTH; ++y)
    for (i32 x = 0; x < CHUNK_WIDTH; ++x)
    {
        PaddedChunk->Blocks[z+1][y+1][x+1] = Chunk->Blocks[z][y][x];
        PaddedChunk->Shades[z+1][y+1][x+1] = Chunk->Shades[z][y][x];
    }

    Chunk = GetChunk(Data, iVec2_Add(ChunkPosition, (ivec2){-1,-1}));
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    {
        PaddedChunk->Blocks[z+1][0][0] = Chunk->Blocks[z][CHUNK_WIDTH-1][CHUNK_WIDTH-1];
        PaddedChunk->Shades[z+1][0][0] = Chunk->Shades[z][CHUNK_WIDTH-1][CHUNK_WIDTH-1];
    }

    Chunk = GetChunk(Data, iVec2_Add(ChunkPosition, (ivec2){1,-1}));
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    {
        PaddedChunk->Blocks[z+1][0][CHUNK_WIDTH+1] = Chunk->Blocks[z][CHUNK_WIDTH-1][0];
        PaddedChunk->Shades[z+1][0][CHUNK_WIDTH+1] = Chunk->Shades[z][CHUNK_WIDTH-1][0];
    }

    Chunk = GetChunk(Data, iVec2_Add(ChunkPosition, (ivec2){-1,1}));
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    {
        PaddedChunk->Blocks[z+1][CHUNK_WIDTH+1][0] = Chunk->Blocks[z][0][CHUNK_WIDTH-1];
        PaddedChunk->Shades[z+1][CHUNK_WIDTH+1][0] = Chunk->Shades[z][0][CHUNK_WIDTH-1];
    }

    Chunk = GetChunk(Data, iVec2_Add(ChunkPosition, (ivec2){1,1}));
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    {
        PaddedChunk->Blocks[z+1][CHUNK_WIDTH+1][CHUNK_WIDTH+1] = Chunk->Blocks[z][0][0];
        PaddedChunk->Shades[z+1][CHUNK_WIDTH+1][CHUNK_WIDTH+1] = Chunk->Shades[z][0][0];
    }

    Chunk = GetChunk(Data, iVec2_Add(ChunkPosition, (ivec2){0,-1}));
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (i32 x = 0; x < CHUNK_WIDTH; ++x)
    {
        PaddedChunk->Blocks[z+1][0][x+1] = Chunk->Blocks[z][CHUNK_WIDTH-1][x];
        PaddedChunk->Shades[z+1][0][x+1] = Chunk->Shades[z][CHUNK_WIDTH-1][x];
    }

    Chunk = GetChunk(Data, iVec2_Add(ChunkPosition, (ivec2){-1,0}));
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (i32 y = 0; y < CHUNK_WIDTH; ++y)
    {
        PaddedChunk->Blocks[z+1][y+1][0] = Chunk->Blocks[z][y][CHUNK_WIDTH-1];
        PaddedChunk->Shades[z+1][y+1][0] = Chunk->Shades[z][y][CHUNK_WIDTH-1];
    }

    Chunk = GetChunk(Data, iVec2_Add(ChunkPosition, (ivec2){0,1}));
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (i32 x = 0; x < CHUNK_WIDTH; ++x)
    {
        PaddedChunk->Blocks[z+1][CHUNK_WIDTH+1][x+1] = Chunk->Blocks[z][0][x];
        PaddedChunk->Shades[z+1][CHUNK_WIDTH+1][x+1] = Chunk->Shades[z][0][x];
    }

    Chunk = GetChunk(Data, iVec2_Add(ChunkPosition, (ivec2){1,0}));
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (i32 y = 0; y < CHUNK_WIDTH; ++y)
    {
        PaddedChunk->Blocks[z+1][y+1][CHUNK_WIDTH+1] = Chunk->Blocks[z][y][0];
        PaddedChunk->Shades[z+1][y+1][CHUNK_WIDTH+1] = Chunk->Shades[z][y][0];
    }
}

block_group PaddedChunk_GetBlockGroup(const padded_chunk *Chunk, ivec3 WorldPosition)
{
    block_group BlockGroup;
    for (i32 z = 0; z < 3; ++z)
    for (i32 y = 0; y < 3; ++y)
    for (i32 x = 0; x < 3; ++x)
    {
        ivec3 BlockPosition = (ivec3){ x, y, z };
        BlockPosition = iVec3_Add(BlockPosition, WorldPosition);
        BlockGroup.Blocks[z][y][x] = Chunk->Blocks[BlockPosition.z][BlockPosition.y][BlockPosition.x];
        BlockGroup.Shades[z][y][x] = Chunk->Shades[BlockPosition.z][BlockPosition.y][BlockPosition.x];
    }
    return BlockGroup;
}

block PaddedChunk_GetBlock(const padded_chunk *Chunk, ivec3 WorldPosition)
{
    ivec3 BlockPosition = World_ToBlockPosition(WorldPosition);
    return Chunk->Blocks[BlockPosition.z+1][BlockPosition.y+1][BlockPosition.x+1];
}

void PaddedChunk_ExtractChunk(const padded_chunk *PaddedChunk, chunk *Chunk)
{
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (i32 y = 0; y < CHUNK_WIDTH; ++y)
    for (i32 x = 0; x < CHUNK_WIDTH; ++x)
    {
        Chunk->Blocks[z][y][x] = PaddedChunk->Blocks[z+1][y+1][x+1];
        Chunk->Shades[z][y][x] = PaddedChunk->Shades[z+1][y+1][x+1];
    }
}

/******************/
/*      CHUNK     */
/******************/

void Chunk_CalcLight(padded_chunk *Chunk)
{
    //skylight
    for (i32 y = 1; y <= CHUNK_WIDTH; y++)
    for (i32 x = 1; x <= CHUNK_WIDTH; x++)
    for (i32 z = CHUNK_HEIGHT - 1; z >= 0; z--)
    {
        Chunk->Shades[z][y][x] = 0x0;
        if (Block_Opaque[Chunk->Blocks[z][y][x].Id])
        {
            Chunk->Shades[z][y][x] = 0xF;
            while (z-- > 0) Chunk->Shades[z][y][x] = 0xF;
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