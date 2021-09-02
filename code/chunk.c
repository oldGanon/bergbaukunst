
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

    block Blocks[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_WIDTH];
} chunk;

typedef struct chunk_group
{
    chunk *Chunks[3][3];
} chunk_group;

inline quad Quad_RotateVerts(quad Quad)
{
    vertex T = Quad.Verts[0];
    Quad.Verts[0] = Quad.Verts[1];
    Quad.Verts[1] = Quad.Verts[2];
    Quad.Verts[2] = Quad.Verts[3];
    Quad.Verts[3] = T;
    return Quad;
}

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

inline block Chunk_GetBlock(const chunk *Chunk, ivec3 Position)
{
    ivec3 ChunkMask = (ivec3){ CHUNK_WIDTH_MASK, CHUNK_WIDTH_MASK, CHUNK_HEIGHT_MASK };
    ivec3 BlockPosition = And(Position, ChunkMask);
    return Chunk->Blocks[BlockPosition.z][BlockPosition.y][BlockPosition.x];
}

inline void Chunk_SetBlock(chunk *Chunk, ivec3 Position, block Block)
{
    ivec3 ChunkMask = (ivec3){ CHUNK_WIDTH_MASK, CHUNK_WIDTH_MASK, CHUNK_HEIGHT_MASK };
    ivec3 BlockPosition = And(Position, ChunkMask);
    Chunk->Blocks[BlockPosition.z][BlockPosition.y][BlockPosition.x] = Block;
    Chunk->MeshDirty = true;
}

inline block ChunkGroup_GetBlock(const chunk_group *ChunkGroup, ivec3 Position)
{
    ivec3 ChunkPosition = ShiftRight(Position, CHUNK_WIDTH_SHIFT);
    chunk *Chunk = ChunkGroup->Chunks[ChunkPosition.y + 1][ChunkPosition.x + 1];
    return Chunk_GetBlock(Chunk, Position);
}

u32 ChunkGroup_OpaqueMask(const chunk_group *ChunkGroup, ivec3 Position)
{
    u32 Mask = 0;
    for (i32 z = Position.z - 1; z <= Position.z + 1; ++z)
    for (i32 y = Position.y - 1; y <= Position.y + 1; ++y)
    for (i32 x = Position.x - 1; x <= Position.x + 1; ++x)
    {
        ivec3 BlockPosition = (ivec3){ x, y, z };
        block Block = ChunkGroup_GetBlock(ChunkGroup, BlockPosition);
        if ((Block.Id != BLOCK_ID_AIR) &&
            (Block.Id != BLOCK_ID_LEAVES))
        {
            Mask |= 1 << ((x+1-Position.x) + (y+1-Position.y)*3 + (z+1-Position.z)*9);
        }
    }
    return Mask;
}

u32 Chunk_BlockMask(chunk *Chunk, vec3 Position, u8 Block)
{
    u32 Mask = 0;
    ivec3 One = (ivec3) { 1, 1, 1 };
    ivec3 iPos = Vec3_FloorToIVec3(Position);
    ivec3 iMin = iVec3_Max(iVec3_Sub(iPos, One), (ivec3) {  0,  0,  0 });
    ivec3 iMax = iVec3_Min(iVec3_Add(iPos, One), (ivec3) { 15, 15, 15 });
    for (i32 z = iMin.z; z <= iMax.z; ++z)
    for (i32 y = iMin.y; y <= iMax.y; ++y)
    for (i32 x = iMin.x; x <= iMax.x; ++x)
    {
        u8 BlockId = Chunk->Blocks[z][y][x].Id;
        if (BlockId == Block)
        {
            Mask |= 1 << ((x+1-iPos.x) + (y+1-iPos.y)*3 + (z+1-iPos.z)*9);
        }
    }
    return Mask;
}

void Chunk_GenerateMesh(chunk_group ChunkGroup)
{
    chunk *Chunk = ChunkGroup.Chunks[1][1];
    Mesh_Clear(&Chunk->Mesh);

    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (i32 y = 0; y < CHUNK_WIDTH; ++y)
    for (i32 x = 0; x < CHUNK_WIDTH; ++x)
    {
        ivec3 iBlockPosition = (ivec3){ x, y, z };
        vec3 BlockPosition = iVec3_ToVec3(iBlockPosition);
        block Block = Chunk_GetBlock(Chunk, iBlockPosition);
        u32 OpaqueMask = ChunkGroup_OpaqueMask(&ChunkGroup, iBlockPosition);

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

    // place blocks
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

    Chunk->Mesh = Mesh_Create();

    // TODO: creation code
}

void Chunk_Delete(chunk *Chunk, i32 x, i32 y)
{
    Chunk->Allocated = false;
    assert(Chunk->x == x);
    assert(Chunk->y == y);

    Mesh_Delete(&Chunk->Mesh);

    // TODO: deletion code
}
