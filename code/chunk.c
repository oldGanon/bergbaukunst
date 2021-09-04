
#define CHUNK_WIDTH_SHIFT 4
#define CHUNK_WIDTH (1 << CHUNK_WIDTH_SHIFT)
#define CHUNK_WIDTH_MASK (CHUNK_WIDTH - 1)
#define CHUNK_HEIGHT_SHIFT 7
#define CHUNK_HEIGHT (1 << CHUNK_HEIGHT_SHIFT)
#define CHUNK_HEIGHT_MASK (CHUNK_HEIGHT - 1)

typedef struct chunk
{
    bool Allocated;
    i32 x, y;

    bool MeshDirty;
    quad_mesh Mesh;

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
    Chunk->MeshDirty = true;

    Chunk_CalcSkyLight(Chunk);
}

void Chunk_GenerateBlocks(chunk *Chunk)
{
    for (i32 zz = 0; zz < CHUNK_HEIGHT; zz++)
    for (i32 yy = 0; yy < CHUNK_WIDTH; yy++)
    for (i32 xx = 0; xx < CHUNK_WIDTH; xx++)
    {
        block* CurrentBlock = &Chunk->Blocks[zz][yy][xx];

        if ((zz == 0) || 
            (yy == 1 && xx == 2 && zz == 1) ||  
            (yy == 2 && xx == 3 && zz == 1) ||
            (yy == 9 && xx == 1 && zz == 1) ||
            (yy == 8 && xx == 2 && zz == 1) ||
            (yy == 8 && xx == 2 && zz == 6))
        {
            CurrentBlock->Id = BLOCK_ID_GRAS;
        }
        else if (yy == 5 && xx == 5 && zz <= 5)
        {
            CurrentBlock->Id = BLOCK_ID_WOOD;
        }
        else if (((yy >= 3) && (yy <= 7) && (xx >= 4) && (xx <= 6) && (zz >= 3) && (zz <= 5)) ||
                 ((yy >= 4) && (yy <= 6) && (xx >= 3) && (xx <= 7) && (zz >= 3) && (zz <= 5)) ||
                 ((yy == 5) && (xx >= 4) && (xx <= 6) && (zz == 6)) ||
                 ((yy >= 4) && (yy <= 6) && (xx == 5) && (zz == 6)))
        {
            CurrentBlock->Id = BLOCK_ID_LEAVES;
        }
        else
        {
            CurrentBlock->Id = BLOCK_ID_AIR;
        }
    }

    Chunk_CalcSkyLight(Chunk);
}

void Chunk_Draw(const camera Camera, const bitmap Target, bitmap TerrainTexture, chunk *Chunk)
{
    vec3 ChunkDim = (vec3) { CHUNK_WIDTH, CHUNK_WIDTH, CHUNK_HEIGHT };
    vec3 ChunkMin = Vec3_Mul(ChunkDim, (vec3) { (f32)Chunk->x, (f32)Chunk->y, 0 });
    vec3 ChunkMax = Vec3_Add(ChunkDim, ChunkMin);
    if (!Camera_BoxVisible(Camera, Target, ChunkMin, ChunkMax)) return;
    Mesh_Draw(Target, Camera, TerrainTexture, ChunkMin, &Chunk->Mesh);
}

void Chunk_Create(chunk *Chunk, i32 x, i32 y)
{
    Chunk->Allocated = true;
    Chunk->x = x;
    Chunk->y = y;

    Chunk->MeshDirty = true;
    Chunk->Mesh = Mesh_Create();

    Chunk_GenerateBlocks(Chunk);
}

void Chunk_Delete(chunk *Chunk, i32 x, i32 y)
{
    Chunk->Allocated = false;
    assert(Chunk->x == x);
    assert(Chunk->y == y);

    Mesh_Delete(&Chunk->Mesh);
}
