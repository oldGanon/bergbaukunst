
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

inline quad Quad_Face(vec3 Pos, vec3 Right, vec3 Up, vec2 UV, vec2 U, vec2 V, f32 Shadow)
{
    return (quad) {
        .Verts[0] = { .Position = Pos,                      .TexCoord = UV,                 .Shadow = Shadow },
        .Verts[1] = { .Position = Add(Pos, Right),          .TexCoord = Add(UV, U),         .Shadow = Shadow },
        .Verts[2] = { .Position = Add(Add(Pos, Right), Up), .TexCoord = Add(Add(UV, U), V), .Shadow = Shadow },
        .Verts[3] = { .Position = Add(Pos, Up),             .TexCoord = Add(UV, V),         .Shadow = Shadow },
    };
}

quad Block_LeftFace(u32 Index, vec3 Position, u32 OpaqueMask)
{
    f32 Shadow = 0.25f;
    vec3 Pos = Vec3_Add(Position, (vec3){0,1,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Quad = Quad_Face(Pos, (vec3){0,-1,0}, (vec3){0,0,1}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    
    if (OpaqueMask & (1 <<  3)) { Quad.Verts[0].Shadow += 0.25f; Quad.Verts[1].Shadow += 0.25f; }
    if (OpaqueMask & (1 <<  9)) { Quad.Verts[1].Shadow += 0.25f; Quad.Verts[2].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 21)) { Quad.Verts[2].Shadow += 0.25f; Quad.Verts[3].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 15)) { Quad.Verts[3].Shadow += 0.25f; Quad.Verts[0].Shadow += 0.25f; }

    if ((OpaqueMask & (1 <<  6)) && (Quad.Verts[0].Shadow == Shadow)) Quad.Verts[0].Shadow += 0.25f;
    if ((OpaqueMask & (1 <<  0)) && (Quad.Verts[1].Shadow == Shadow)) Quad.Verts[1].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 18)) && (Quad.Verts[2].Shadow == Shadow)) Quad.Verts[2].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 24)) && (Quad.Verts[3].Shadow == Shadow)) Quad.Verts[3].Shadow += 0.25f;
    
    if (OpaqueMask & ((1 << 6) | (1 << 18)))
        return Quad_RotateVerts(Quad);
    return Quad;
}

quad Block_RightFace(u32 Index, vec3 Position, u32 OpaqueMask)
{
    f32 Shadow = 0.25f;
    vec3 Pos = Vec3_Add(Position, (vec3){1,0,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Quad = Quad_Face(Pos, (vec3){0,1,0}, (vec3){0,0,1}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    
    if (OpaqueMask & (1 <<  5)) { Quad.Verts[0].Shadow += 0.25f; Quad.Verts[1].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 17)) { Quad.Verts[1].Shadow += 0.25f; Quad.Verts[2].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 23)) { Quad.Verts[2].Shadow += 0.25f; Quad.Verts[3].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 11)) { Quad.Verts[3].Shadow += 0.25f; Quad.Verts[0].Shadow += 0.25f; }

    if ((OpaqueMask & (1 <<  2)) && (Quad.Verts[0].Shadow == Shadow)) Quad.Verts[0].Shadow += 0.25f;
    if ((OpaqueMask & (1 <<  8)) && (Quad.Verts[1].Shadow == Shadow)) Quad.Verts[1].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 26)) && (Quad.Verts[2].Shadow == Shadow)) Quad.Verts[2].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 20)) && (Quad.Verts[3].Shadow == Shadow)) Quad.Verts[3].Shadow += 0.25f;
    
    if (OpaqueMask & ((1 << 2) | (1 << 26)))
        return Quad_RotateVerts(Quad);
    return Quad;
}

quad Block_FrontFace(u32 Index, vec3 Position, u32 OpaqueMask)
{
    f32 Shadow = 0.125f;
    vec3 Pos = Vec3_Add(Position, (vec3){0,0,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Quad = Quad_Face(Pos, (vec3){1,0,0}, (vec3){0,0,1}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    
    if (OpaqueMask & (1 <<  1)) { Quad.Verts[0].Shadow += 0.25f; Quad.Verts[1].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 11)) { Quad.Verts[1].Shadow += 0.25f; Quad.Verts[2].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 19)) { Quad.Verts[2].Shadow += 0.25f; Quad.Verts[3].Shadow += 0.25f; }
    if (OpaqueMask & (1 <<  9)) { Quad.Verts[3].Shadow += 0.25f; Quad.Verts[0].Shadow += 0.25f; }

    if ((OpaqueMask & (1 <<  0)) && (Quad.Verts[0].Shadow == Shadow)) Quad.Verts[0].Shadow += 0.25f;
    if ((OpaqueMask & (1 <<  2)) && (Quad.Verts[1].Shadow == Shadow)) Quad.Verts[1].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 20)) && (Quad.Verts[2].Shadow == Shadow)) Quad.Verts[2].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 18)) && (Quad.Verts[3].Shadow == Shadow)) Quad.Verts[3].Shadow += 0.25f;
    
    if (OpaqueMask & ((1 << 0) | (1 << 20)))
        return Quad_RotateVerts(Quad);
    return Quad;
}

quad Block_BackFace(u32 Index, vec3 Position, u32 OpaqueMask)
{
    f32 Shadow = 0.125f;
    vec3 Pos = Vec3_Add(Position, (vec3){1,1,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Quad = Quad_Face(Pos, (vec3){-1,0,0}, (vec3){0,0,1}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    
    if (OpaqueMask & (1 <<  7)) { Quad.Verts[0].Shadow += 0.25f; Quad.Verts[1].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 15)) { Quad.Verts[1].Shadow += 0.25f; Quad.Verts[2].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 25)) { Quad.Verts[2].Shadow += 0.25f; Quad.Verts[3].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 17)) { Quad.Verts[3].Shadow += 0.25f; Quad.Verts[0].Shadow += 0.25f; }

    if ((OpaqueMask & (1 <<  8)) && (Quad.Verts[0].Shadow == Shadow)) Quad.Verts[0].Shadow += 0.25f;
    if ((OpaqueMask & (1 <<  6)) && (Quad.Verts[1].Shadow == Shadow)) Quad.Verts[1].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 24)) && (Quad.Verts[2].Shadow == Shadow)) Quad.Verts[2].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 26)) && (Quad.Verts[3].Shadow == Shadow)) Quad.Verts[3].Shadow += 0.25f;
    
    if (OpaqueMask & ((1 << 6) | (1 << 26)))
        return Quad_RotateVerts(Quad);
    return Quad;
}

quad Block_BottomFace(u32 Index, vec3 Position, u32 OpaqueMask)
{
    f32 Shadow = 0.375f;
    vec3 Pos = Vec3_Add(Position, (vec3){0,1,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Quad = Quad_Face(Pos, (vec3){1,0,0}, (vec3){0,-1,0}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    
    if (OpaqueMask & (1 << 7)) { Quad.Verts[0].Shadow += 0.25f; Quad.Verts[1].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 5)) { Quad.Verts[1].Shadow += 0.25f; Quad.Verts[2].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 1)) { Quad.Verts[2].Shadow += 0.25f; Quad.Verts[3].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 3)) { Quad.Verts[3].Shadow += 0.25f; Quad.Verts[0].Shadow += 0.25f; }

    if ((OpaqueMask & (1 << 6)) && (Quad.Verts[0].Shadow == Shadow)) Quad.Verts[0].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 8)) && (Quad.Verts[1].Shadow == Shadow)) Quad.Verts[1].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 2)) && (Quad.Verts[2].Shadow == Shadow)) Quad.Verts[2].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 0)) && (Quad.Verts[3].Shadow == Shadow)) Quad.Verts[3].Shadow += 0.25f;
    
    if (OpaqueMask & ((1 << 6) | (1 << 2)))
        return Quad_RotateVerts(Quad);
    return Quad;
}

quad Block_TopFace(u32 Index, vec3 Position, u32 OpaqueMask)
{
    f32 Shadow = 0.0f;
    vec3 Pos = Vec3_Add(Position, (vec3){0,0,1});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Quad = Quad_Face(Pos, (vec3){1,0,0}, (vec3){0,1,0}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    
    if (OpaqueMask & (1 << 19)) { Quad.Verts[0].Shadow += 0.25f; Quad.Verts[1].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 23)) { Quad.Verts[1].Shadow += 0.25f; Quad.Verts[2].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 25)) { Quad.Verts[2].Shadow += 0.25f; Quad.Verts[3].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 21)) { Quad.Verts[3].Shadow += 0.25f; Quad.Verts[0].Shadow += 0.25f; }

    if ((OpaqueMask & (1 << 18)) && (Quad.Verts[0].Shadow == Shadow)) Quad.Verts[0].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 20)) && (Quad.Verts[1].Shadow == Shadow)) Quad.Verts[1].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 26)) && (Quad.Verts[2].Shadow == Shadow)) Quad.Verts[2].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 24)) && (Quad.Verts[3].Shadow == Shadow)) Quad.Verts[3].Shadow += 0.25f;
    
    if (OpaqueMask & ((1 << 18) | (1 << 26)))
        return Quad_RotateVerts(Quad);
    return Quad;
}

void Chunk_AddBlockQuads(quad_mesh *Mesh, vec3 Position, u32 OpaqueMask, 
                         u32 Left, u32 Right, u32 Front, u32 Back, u32 Bottom, u32 Top)
{
    // Left
    if (!(OpaqueMask & (1 << 12)))
    {
        Mesh_AddQuad(Mesh, Block_LeftFace(Left, Position, OpaqueMask));
    }

    // Right
    if (!(OpaqueMask & (1 << 14)))
    {
        Mesh_AddQuad(Mesh, Block_RightFace(Right, Position, OpaqueMask));
    }

    // Front
    if (!(OpaqueMask & (1 << 10)))
    {
        Mesh_AddQuad(Mesh, Block_FrontFace(Front, Position, OpaqueMask));
    }

    // Back
    if (!(OpaqueMask & (1 << 16)))
    {
        Mesh_AddQuad(Mesh, Block_BackFace(Back, Position, OpaqueMask));
    }

    // Bottom
    if (!(OpaqueMask & (1 << 4)))
    {
        Mesh_AddQuad(Mesh, Block_BottomFace(Bottom, Position, OpaqueMask));
    }

    // Top
    if (!(OpaqueMask & (1 << 22)))
    {
        Mesh_AddQuad(Mesh, Block_TopFace(Top, Position, OpaqueMask));
    }
}

#define DEFAULT_BLOCK (block){ .Id = BLOCK_ID_AIR, .Light = 0x0 }
#define DEFAULT_SKY_BLOCK (block){ .Id = BLOCK_ID_AIR, .Light = 0x0 }
#define DEFAULT_HELL_BLOCK (block){ .Id = BLOCK_ID_AIR, .Light = 0xF }

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

u32 BlockGroup_OpaqueMask(const block_group *BlockGroup)
{
    u32 Mask = 0;
    for (i32 i = 0; i < 27; ++i)
    if (Block_Opaque[BlockGroup->Blocks[0][0][i].Id])
        Mask |= 1 << i;
    return Mask;
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
        u32 OpaqueMask = BlockGroup_OpaqueMask(&BlockGroup);
        if ((OpaqueMask & 0x7FFDFFF) == 0x7FFDFFF) continue;

        vec3 BlockPosition = iVec3_ToVec3(iBlockPosition);
        switch (Block.Id)
        {
            case BLOCK_ID_GRAS:
            {
                Chunk_AddBlockQuads(&Chunk->Mesh, BlockPosition, OpaqueMask, 1, 1, 1, 1, 2, 0);
            } break;

            case BLOCK_ID_WOOD:
            {
                Chunk_AddBlockQuads(&Chunk->Mesh, BlockPosition, OpaqueMask, 7, 7, 7, 7, 15, 15);
            } break;

            case BLOCK_ID_LEAVES:
            {
                // u32 LeaveMask = Chunk_BlockMask(Chunk, BlockPosition, BLOCK_ID_LEAVES);
                Chunk_AddBlockQuads(&Chunk->Mesh, BlockPosition, OpaqueMask, 3, 3, 3, 3, 3, 3);
            } break;
        }
    }

    Chunk->MeshDirty = false;
}

void Chunk_GenerateBlocks(chunk *Chunk, i32 x, i32 y)
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
}

void Chunk_Light(chunk *Chunk)
{
    for (i32 z = 0; z < CHUNK_HEIGHT; z++)
    for (i32 y = 0; y < CHUNK_WIDTH; y++)
    for (i32 x = 0; x < CHUNK_WIDTH; x++)
    {
        ivec3 Position = (ivec3){ x, y, z };
        block Block = Chunk_GetBlock(Chunk, Position);

        Position.z += 1;
        block Top = Chunk_GetBlock(Chunk, Position);
        if (Top.Light == 0xF)
        {
            Block.Light = Top.Light;
            Chunk_SetBlock(Chunk, Position, Block);
        }
    }
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

    Chunk_GenerateBlocks(Chunk, x, y);
}

void Chunk_Delete(chunk *Chunk, i32 x, i32 y)
{
    Chunk->Allocated = false;
    assert(Chunk->x == x);
    assert(Chunk->y == y);

    Mesh_Delete(&Chunk->Mesh);
}
