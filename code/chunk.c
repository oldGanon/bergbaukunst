
#define CHUNK_WIDTH_SHIFT 4
#define CHUNK_WIDTH (1 << CHUNK_WIDTH_SHIFT)
#define CHUNK_WIDTH_MASK (CHUNK_WIDTH - 1)
#define CHUNK_HEIGHT_SHIFT 7
#define CHUNK_HEIGHT (1 << CHUNK_HEIGHT_SHIFT)
#define CHUNK_HEIGHT_MASK (CHUNK_HEIGHT - 1)

typedef struct chunk_group
{
    struct chunk *Chunks[3][3];
} chunk_group;

typedef struct chunk
{
    bool Allocated;
    i32 x, y;

    bool MeshDirty;
    quad_mesh Mesh;

    chunk_group Neighbors;

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

    if ((BlockPosition.x == 0) && (Chunk->Neighbors.Chunks[1][0]))
        Chunk->Neighbors.Chunks[1][0]->MeshDirty = true;
    if ((BlockPosition.y == 0) && (Chunk->Neighbors.Chunks[0][1]))
        Chunk->Neighbors.Chunks[0][1]->MeshDirty = true;
    if ((BlockPosition.x == CHUNK_WIDTH - 1) && (Chunk->Neighbors.Chunks[1][2]))
        Chunk->Neighbors.Chunks[1][2]->MeshDirty = true;
    if ((BlockPosition.y == CHUNK_WIDTH - 1) && (Chunk->Neighbors.Chunks[2][1]))
        Chunk->Neighbors.Chunks[2][1]->MeshDirty = true;
    if ((BlockPosition.x == 0) && (BlockPosition.y == 0) && (Chunk->Neighbors.Chunks[0][0]))
        Chunk->Neighbors.Chunks[0][0]->MeshDirty = true;
    if ((BlockPosition.x == 0) && (BlockPosition.y == CHUNK_WIDTH - 1) && (Chunk->Neighbors.Chunks[2][0]))
        Chunk->Neighbors.Chunks[2][0]->MeshDirty = true;
    if ((BlockPosition.x == CHUNK_WIDTH - 1) && (BlockPosition.y == 0) && (Chunk->Neighbors.Chunks[0][2]))
        Chunk->Neighbors.Chunks[0][2]->MeshDirty = true;
    if ((BlockPosition.x == CHUNK_WIDTH - 1) && (BlockPosition.y == CHUNK_WIDTH - 1) && (Chunk->Neighbors.Chunks[2][2]))
        Chunk->Neighbors.Chunks[2][2]->MeshDirty = true;
}

block ChunkGroup_GetBlock(const chunk_group *ChunkGroup, ivec3 WorldPosition)
{
    ivec2 ChunkPosition = World_ToChunkPosition(WorldPosition);
    const chunk *Chunk = ChunkGroup->Chunks[ChunkPosition.y + 1][ChunkPosition.x + 1];
    if (!Chunk) return DEFAULT_BLOCK;
    return Chunk_GetBlock(Chunk, WorldPosition);
}

block_group ChunkGroup_GetBlockGroup(const chunk_group *ChunkGroup, ivec3 WorldPosition)
{
    block_group BlockGroup;
    WorldPosition = iVec3_Sub(WorldPosition, iVec3_Set1(1));
    for (i32 z = 0; z < 3; ++z)
    for (i32 y = 0; y < 3; ++y)
    for (i32 x = 0; x < 3; ++x)
    {
        ivec3 BlockPosition = (ivec3){ x, y, z };
        BlockPosition = iVec3_Add(BlockPosition, WorldPosition);
        BlockGroup.Blocks[z][y][x] = ChunkGroup_GetBlock(ChunkGroup, BlockPosition);
    }
    return BlockGroup;
}

void Chunk_GenerateMesh(chunk *Chunk)
{
    Mesh_Clear(&Chunk->Mesh);

    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (i32 y = 0; y < CHUNK_WIDTH; ++y)
    for (i32 x = 0; x < CHUNK_WIDTH; ++x)
    {
        ivec3 iBlockPosition = (ivec3){ x, y, z };
        block Block = Chunk_GetBlock(Chunk, iBlockPosition);
        if (Block.Id == BLOCK_ID_AIR) continue;

        block_group BlockGroup = ChunkGroup_GetBlockGroup(&Chunk->Neighbors, iBlockPosition);

        vec3 BlockPosition = iVec3_ToVec3(iBlockPosition);
        switch (Block.Id)
        {
            case BLOCK_ID_GRAS:
            {
                Block_AddQuadsToMesh(&Chunk->Mesh, BlockPosition, &BlockGroup, 1, 1, 1, 1, 2, 0);
            } break;

            case BLOCK_ID_WOOD:
            {
                Block_AddQuadsToMesh(&Chunk->Mesh, BlockPosition, &BlockGroup, 7, 7, 7, 7, 15, 15);
            } break;

            case BLOCK_ID_LEAVES:
            {
                Block_AddQuadsToMesh(&Chunk->Mesh, BlockPosition, &BlockGroup, 3, 3, 3, 3, 3, 3);
            } break;
        }
    }

    Chunk->MeshDirty = false;
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
    if (Chunk->MeshDirty) Chunk_GenerateMesh(Chunk);
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
