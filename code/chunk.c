
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

    u64 Neighbors[3][3];

    block Blocks[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_WIDTH];
} chunk;

#define DEFAULT_BLOCK (block){ .Id = BLOCK_ID_AIR, .Shadow = 0x0 }
#define DEFAULT_SKY_BLOCK (block){ .Id = BLOCK_ID_AIR, .Shadow = 0x0 }
#define DEFAULT_HELL_BLOCK (block){ .Id = BLOCK_ID_AIR, .Shadow = 0xF }

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

void Chunk_Create(chunk *Chunk, ivec2 Position)
{
    Chunk->Allocated = true;
    Chunk->Position = Position;
}

void Chunk_Delete(chunk *Chunk, ivec2 Position)
{
    Chunk->Allocated = false;
    assert(Chunk->Position.x == Position.x);
    assert(Chunk->Position.y == Position.y);
}
