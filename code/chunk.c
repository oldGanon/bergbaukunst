
#define CHUNK_WIDTH_SHIFT 4
#define CHUNK_WIDTH (1 << CHUNK_WIDTH_SHIFT)
#define CHUNK_WIDTH_MASK (CHUNK_WIDTH - 1)
#define CHUNK_HEIGHT_SHIFT 7
#define CHUNK_HEIGHT (1 << CHUNK_HEIGHT_SHIFT)
#define CHUNK_HEIGHT_MASK (CHUNK_HEIGHT - 1)

enum chunk_flags
{
    CHUNK_EMPTY     = 0,
    CHUNK_ALLOCATED = 1 << 0,
    CHUNK_GENERATED = 1 << 1,
    CHUNK_DECORATED = 1 << 2,
    CHUNK_CHANGED   = 1 << 3,

    CHUNK_COMPLETE  = (CHUNK_ALLOCATED | CHUNK_GENERATED | CHUNK_DECORATED),
};

typedef struct block_group
{
    block Blocks[3][3][3];
    u8 Shades[3][3][3];
} block_group;

typedef struct chunk
{
    ivec2 Position;
    u32 Flags;

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

typedef chunk *get_chunk_func(void *Data, ivec2 Position);
typedef const chunk *get_const_chunk_func(const void *Data, ivec2 Position);

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
/*      CHUNK     */
/******************/

void Chunk_CalcLight(void *Data, get_chunk_func *GetChunk, ivec2 ChunkPosition)
{
    // get chunks
    chunk *Chunks[3][3];
    for (u32 y = 0; y < 3; ++y)
    for (u32 x = 0; x < 3; ++x)
    {
        ivec2 Offset = { x - 1, y - 1 };
        ivec2 ChunkPos = iVec2_Add(ChunkPosition, Offset);
        Chunks[y][x] = GetChunk(Data, ChunkPos);
    }

    // read values
    u8 Shades[CHUNK_HEIGHT][CHUNK_WIDTH * 3][CHUNK_WIDTH * 3] = { 0 };
    u8 Opaque[CHUNK_HEIGHT][CHUNK_WIDTH * 3][CHUNK_WIDTH * 3] = { 0 };
    u8 Transp[CHUNK_HEIGHT][CHUNK_WIDTH * 3][CHUNK_WIDTH * 3] = { 0 };
    for (u32 cy = 0; cy < 3; ++cy)
    for (u32 cx = 0; cx < 3; ++cx)
    {
        chunk *Chunk = Chunks[cy][cx];
        if (!Chunk) continue;
        for (u32 bz = 0; bz < CHUNK_HEIGHT; ++bz)
        for (u32 by = 0; by < CHUNK_WIDTH; ++by)
        for (u32 bx = 0; bx < CHUNK_WIDTH; ++bx)
        {
            u8 Block = Chunk->Blocks[bz][by][bx].Id;
            Shades[bz][cy * CHUNK_WIDTH + by][cx * CHUNK_WIDTH + bx] = Chunk->Shades[bz][by][bx];
            Opaque[bz][cy * CHUNK_WIDTH + by][cx * CHUNK_WIDTH + bx] = (Block_Opaque[Block]) ? 0xF : 0x0;
            Transp[bz][cy * CHUNK_WIDTH + by][cx * CHUNK_WIDTH + bx] = (Block_Transparent[Block]) ? 0x0 : 0xF;
        }
    }

    // sky light
    for (i32 y = 1; y < CHUNK_WIDTH * 3 - 1; ++y)
    {
        __m256i S0 = _mm256_set1_epi8(0);
        __m256i S1 = _mm256_set1_epi8(0);
        for (i32 z = CHUNK_HEIGHT - 1; z >= 0; --z)
        {
            __m256i O0 = _mm256_loadu_si256((__m256i*)&Transp[z][y][1]);
            __m256i O1 = _mm256_loadu_si256((__m256i*)&Transp[z][y][15]);
            S0 = _mm256_or_si256(S0, O0);
            S1 = _mm256_or_si256(S1, O1);
            _mm256_store_si256((__m256i*)&Shades[z][y][1], S0);
            _mm256_store_si256((__m256i*)&Shades[z][y][15], S1);
        }
    }

    // block light
    // TODO

    // propagate light
    __m256i One = _mm256_set1_epi8(1);
    for (i32 i = 0; i < 15; ++i)
    for (i32 y = 1; y < CHUNK_WIDTH * 3 - 1; ++y)
    for (i32 z = 1; z < CHUNK_HEIGHT - 1; ++z)
    {
        __m256i Dst0 = _mm256_loadu_si256((__m256i*)&Shades[z][y][ 1]);
        __m256i Dst1 = _mm256_loadu_si256((__m256i*)&Shades[z][y][15]);

        Dst0 = _mm256_sub_epi8(Dst0, One);
        Dst0 = _mm256_min_epi8(Dst0, _mm256_loadu_si256((__m256i*)&Shades[z  ][y  ][0]));
        Dst0 = _mm256_min_epi8(Dst0, _mm256_loadu_si256((__m256i*)&Shades[z  ][y-1][1]));
        Dst0 = _mm256_min_epi8(Dst0, _mm256_loadu_si256((__m256i*)&Shades[z-1][y  ][1]));
        Dst0 = _mm256_min_epi8(Dst0, _mm256_loadu_si256((__m256i*)&Shades[z+1][y  ][1]));
        Dst0 = _mm256_min_epi8(Dst0, _mm256_loadu_si256((__m256i*)&Shades[z  ][y+1][1]));
        Dst0 = _mm256_min_epi8(Dst0, _mm256_loadu_si256((__m256i*)&Shades[z  ][y  ][2]));
        Dst0 = _mm256_add_epi8(Dst0, One);
        Dst0 = _mm256_or_si256(Dst0, _mm256_loadu_si256((__m256i*)&Opaque[z][y][1]));

        Dst1 = _mm256_sub_epi8(Dst1, One);
        Dst1 = _mm256_min_epi8(Dst1, _mm256_loadu_si256((__m256i*)&Shades[z  ][y  ][14]));
        Dst1 = _mm256_min_epi8(Dst1, _mm256_loadu_si256((__m256i*)&Shades[z  ][y-1][15]));
        Dst1 = _mm256_min_epi8(Dst1, _mm256_loadu_si256((__m256i*)&Shades[z-1][y  ][15]));
        Dst1 = _mm256_min_epi8(Dst1, _mm256_loadu_si256((__m256i*)&Shades[z+1][y  ][15]));
        Dst1 = _mm256_min_epi8(Dst1, _mm256_loadu_si256((__m256i*)&Shades[z  ][y+1][15]));
        Dst1 = _mm256_min_epi8(Dst1, _mm256_loadu_si256((__m256i*)&Shades[z  ][y  ][16]));
        Dst1 = _mm256_add_epi8(Dst1, One);
        Dst1 = _mm256_or_si256(Dst1, _mm256_loadu_si256((__m256i*)&Opaque[z][y][15]));

        _mm256_store_si256((__m256i*)&Shades[z][y][ 1], Dst0);
        _mm256_store_si256((__m256i*)&Shades[z][y][15], Dst1);
    }

    // write new values
    for (u32 cy = 0; cy < 3; ++cy)
    for (u32 cx = 0; cx < 3; ++cx)
    {
        chunk *Chunk = Chunks[cy][cx];
        if (!Chunk) continue;
        for (u32 bz = 0; bz < CHUNK_HEIGHT; ++bz)
        for (u32 by = 0; by < CHUNK_WIDTH; ++by)
        for (u32 bx = 0; bx < CHUNK_WIDTH; ++bx)
        {
            Chunk->Shades[bz][by][bx] = Shades[bz][cy * CHUNK_WIDTH + by][cx * CHUNK_WIDTH + bx];
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

    Chunk->Flags |= CHUNK_CHANGED;
}

void Chunk_Init(chunk *Chunk, ivec2 Position)
{
    memset(Chunk, 0, sizeof(chunk));

    Chunk->Flags |= CHUNK_ALLOCATED;
    Chunk->Position = Position;
}

void Chunk_Clear(chunk *Chunk, ivec2 Position)
{
    assert(Chunk->Position.x == Position.x);
    assert(Chunk->Position.y == Position.y);

    Chunk->Flags = CHUNK_EMPTY;
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

/******************/
/*  CHUNK GROUP   */
/******************/

typedef struct chunk_group
{
    ivec2 Position;
    chunk *Chunks[3][3];
} chunk_group;

chunk_group Chunk_Group(void *Data, get_chunk_func *GetChunk, ivec2 ChunkPosition)
{
    chunk_group ChunkGroup = { .Position = ChunkPosition };
    for (u32 y = 0; y < 3; ++y)
    for (u32 x = 0; x < 3; ++x)
    {
        ivec2 Offset = { x - 1, y - 1 };
        ivec2 ChunkPos = iVec2_Add(ChunkPosition, Offset);
        ChunkGroup.Chunks[y][x] = GetChunk(Data, ChunkPos);
    }
    return ChunkGroup;
}

bool ChunkGroup_Complete(const chunk_group *ChunkGroup)
{
    for (u32 y = 0; y < 3; ++y)
    for (u32 x = 0; x < 3; ++x)
        if (!ChunkGroup->Chunks[y][x])
            return false;
    return true;
}

block ChunkGroup_GetBlock(const chunk_group *ChunkGroup, ivec3 WorldPosition)
{
    ivec2 GroupPosition = iVec2_Sub(iVec2_ShiftRight(WorldPosition.xy, CHUNK_WIDTH_SHIFT), ChunkGroup->Position);
    if (GroupPosition.x < -1 || GroupPosition.x > 1 || 
        GroupPosition.y < -1 || GroupPosition.y > 1)
        return DEFAULT_BLOCK;

    chunk *Chunk = ChunkGroup->Chunks[GroupPosition.y + 1][GroupPosition.x + 1];
    return Chunk_GetBlock(Chunk, WorldPosition);
}

void ChunkGroup_SetBlock(const chunk_group *ChunkGroup, ivec3 WorldPosition, block Block)
{
    ivec2 GroupPosition = iVec2_Sub(iVec2_ShiftRight(WorldPosition.xy, CHUNK_WIDTH_SHIFT), ChunkGroup->Position);
    if (GroupPosition.x < -1 || GroupPosition.x > 1 || 
        GroupPosition.y < -1 || GroupPosition.y > 1)
        return;

    chunk *Chunk = ChunkGroup->Chunks[GroupPosition.y + 1][GroupPosition.x + 1];
    Chunk_SetBlock(Chunk, WorldPosition, Block);
}

/******************/
/*  PADDED CHUNK  */
/******************/

typedef struct padded_chunk
{
    block Blocks[CHUNK_HEIGHT+2][CHUNK_WIDTH+2][CHUNK_WIDTH+2];
    u8 Shades[CHUNK_HEIGHT+2][CHUNK_WIDTH+2][CHUNK_WIDTH+2];
} padded_chunk;

void Chunk_Pad(get_const_chunk_func *GetChunk, const void *Data, ivec2 ChunkPosition, padded_chunk *PaddedChunk)
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
