
#define CHUNK_WIDTH_SHIFT 4
#define CHUNK_WIDTH (1 << CHUNK_WIDTH_SHIFT)
#define CHUNK_WIDTH_MASK (CHUNK_WIDTH - 1)
#define CHUNK_HEIGHT_SHIFT 7
#define CHUNK_HEIGHT (1 << CHUNK_HEIGHT_SHIFT)
#define CHUNK_HEIGHT_MASK (CHUNK_HEIGHT - 1)

typedef struct chunk
{
    i32 x, z;
    bool Allocated;

    quad_mesh Mesh;

    world_block Blocks[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_HEIGHT];
} chunk;




quad Quad_MovePosition(quad Quad, vec3 Offset)
{
    Quad.Verts[0].Position = Vec3_Add(Quad.Verts[0].Position, Offset);
    Quad.Verts[1].Position = Vec3_Add(Quad.Verts[1].Position, Offset);
    Quad.Verts[2].Position = Vec3_Add(Quad.Verts[2].Position, Offset);
    Quad.Verts[3].Position = Vec3_Add(Quad.Verts[3].Position, Offset);
    return Quad;
}

quad Quad_MoveTexCoord(quad Quad, vec2 Offset)
{
    Quad.Verts[0].TexCoord = Vec2_Add(Quad.Verts[0].TexCoord, Offset);
    Quad.Verts[1].TexCoord = Vec2_Add(Quad.Verts[1].TexCoord, Offset);
    Quad.Verts[2].TexCoord = Vec2_Add(Quad.Verts[2].TexCoord, Offset);
    Quad.Verts[3].TexCoord = Vec2_Add(Quad.Verts[3].TexCoord, Offset);
    return Quad;
}

quad Quad_RotateVerts(quad Quad)
{
    vertex T = Quad.Verts[0];
    Quad.Verts[0] = Quad.Verts[1];
    Quad.Verts[1] = Quad.Verts[2];
    Quad.Verts[2] = Quad.Verts[3];
    Quad.Verts[3] = T;
    return Quad;
}

quad Block_LeftFace(u32 Index, vec3 Position, u32 OpaqueMask)
{
    f32 DefaultShadow = 0.25f;
    quad Quad = {
        .Verts[0] = { .Position = { 0,0,1 }, .TexCoord = {  0,  0 }, .Shadow = DefaultShadow },
        .Verts[1] = { .Position = { 0,0,0 }, .TexCoord = { 16,  0 }, .Shadow = DefaultShadow },
        .Verts[2] = { .Position = { 0,1,0 }, .TexCoord = { 16, 16 }, .Shadow = DefaultShadow },
        .Verts[3] = { .Position = { 0,1,1 }, .TexCoord = {  0, 16 }, .Shadow = DefaultShadow },
    };
    Quad = Quad_MovePosition(Quad, Position);
    
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    Quad = Quad_MoveTexCoord(Quad, UV);
    
    if (OpaqueMask & (1 <<  3)) { Quad.Verts[0].Shadow += 0.25f; Quad.Verts[1].Shadow += 0.25f; }
    if (OpaqueMask & (1 <<  9)) { Quad.Verts[1].Shadow += 0.25f; Quad.Verts[2].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 21)) { Quad.Verts[2].Shadow += 0.25f; Quad.Verts[3].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 15)) { Quad.Verts[3].Shadow += 0.25f; Quad.Verts[0].Shadow += 0.25f; }

    if ((OpaqueMask & (1 <<  6)) && (Quad.Verts[0].Shadow == DefaultShadow)) Quad.Verts[0].Shadow += 0.25f;
    if ((OpaqueMask & (1 <<  0)) && (Quad.Verts[1].Shadow == DefaultShadow)) Quad.Verts[1].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 18)) && (Quad.Verts[2].Shadow == DefaultShadow)) Quad.Verts[2].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 24)) && (Quad.Verts[3].Shadow == DefaultShadow)) Quad.Verts[3].Shadow += 0.25f;
    
    if (OpaqueMask & ((1 << 6) | (1 << 18)))
        return Quad_RotateVerts(Quad);
    return Quad;
}

quad Block_RightFace(u32 Index, vec3 Position, u32 OpaqueMask)
{
    f32 DefaultShadow = 0.25f;
    quad Quad = {
        .Verts[0] = { .Position = { 1,0,0 }, .TexCoord = {  0,  0 }, .Shadow = DefaultShadow },
        .Verts[1] = { .Position = { 1,0,1 }, .TexCoord = { 16,  0 }, .Shadow = DefaultShadow },
        .Verts[2] = { .Position = { 1,1,1 }, .TexCoord = { 16, 16 }, .Shadow = DefaultShadow },
        .Verts[3] = { .Position = { 1,1,0 }, .TexCoord = {  0, 16 }, .Shadow = DefaultShadow },
    };
    Quad = Quad_MovePosition(Quad, Position);
    
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    Quad = Quad_MoveTexCoord(Quad, UV);
    
    if (OpaqueMask & (1 <<  5)) { Quad.Verts[0].Shadow += 0.25f; Quad.Verts[1].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 17)) { Quad.Verts[1].Shadow += 0.25f; Quad.Verts[2].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 23)) { Quad.Verts[2].Shadow += 0.25f; Quad.Verts[3].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 11)) { Quad.Verts[3].Shadow += 0.25f; Quad.Verts[0].Shadow += 0.25f; }

    if ((OpaqueMask & (1 <<  2)) && (Quad.Verts[0].Shadow == DefaultShadow)) Quad.Verts[0].Shadow += 0.25f;
    if ((OpaqueMask & (1 <<  8)) && (Quad.Verts[1].Shadow == DefaultShadow)) Quad.Verts[1].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 26)) && (Quad.Verts[2].Shadow == DefaultShadow)) Quad.Verts[2].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 20)) && (Quad.Verts[3].Shadow == DefaultShadow)) Quad.Verts[3].Shadow += 0.25f;
    
    if (OpaqueMask & ((1 << 2) | (1 << 26)))
        return Quad_RotateVerts(Quad);
    return Quad;
}

quad Block_FrontFace(u32 Index, vec3 Position, u32 OpaqueMask)
{
    f32 DefaultShadow = 0.125f;
    quad Quad = {
        .Verts[0] = { .Position = { 0,0,0 }, .TexCoord = {  0,  0 }, .Shadow = DefaultShadow },
        .Verts[1] = { .Position = { 1,0,0 }, .TexCoord = { 16,  0 }, .Shadow = DefaultShadow },
        .Verts[2] = { .Position = { 1,1,0 }, .TexCoord = { 16, 16 }, .Shadow = DefaultShadow },
        .Verts[3] = { .Position = { 0,1,0 }, .TexCoord = {  0, 16 }, .Shadow = DefaultShadow },
    };
    Quad = Quad_MovePosition(Quad, Position);
    
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    Quad = Quad_MoveTexCoord(Quad, UV);
    
    if (OpaqueMask & (1 <<  1)) { Quad.Verts[0].Shadow += 0.25f; Quad.Verts[1].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 11)) { Quad.Verts[1].Shadow += 0.25f; Quad.Verts[2].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 19)) { Quad.Verts[2].Shadow += 0.25f; Quad.Verts[3].Shadow += 0.25f; }
    if (OpaqueMask & (1 <<  9)) { Quad.Verts[3].Shadow += 0.25f; Quad.Verts[0].Shadow += 0.25f; }

    if ((OpaqueMask & (1 <<  0)) && (Quad.Verts[0].Shadow == DefaultShadow)) Quad.Verts[0].Shadow += 0.25f;
    if ((OpaqueMask & (1 <<  2)) && (Quad.Verts[1].Shadow == DefaultShadow)) Quad.Verts[1].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 20)) && (Quad.Verts[2].Shadow == DefaultShadow)) Quad.Verts[2].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 18)) && (Quad.Verts[3].Shadow == DefaultShadow)) Quad.Verts[3].Shadow += 0.25f;
    
    if (OpaqueMask & ((1 << 0) | (1 << 20)))
        return Quad_RotateVerts(Quad);
    return Quad;
}

quad Block_BackFace(u32 Index, vec3 Position, u32 OpaqueMask)
{
    f32 DefaultShadow = 0.125f;
    quad Quad = {
        .Verts[0] = { .Position = { 1,0,1 }, .TexCoord = {  0,  0 }, .Shadow = DefaultShadow },
        .Verts[1] = { .Position = { 0,0,1 }, .TexCoord = { 16,  0 }, .Shadow = DefaultShadow },
        .Verts[2] = { .Position = { 0,1,1 }, .TexCoord = { 16, 16 }, .Shadow = DefaultShadow },
        .Verts[3] = { .Position = { 1,1,1 }, .TexCoord = {  0, 16 }, .Shadow = DefaultShadow },
    };
    Quad = Quad_MovePosition(Quad, Position);
    
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    Quad = Quad_MoveTexCoord(Quad, UV);
    
    if (OpaqueMask & (1 <<  7)) { Quad.Verts[0].Shadow += 0.25f; Quad.Verts[1].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 15)) { Quad.Verts[1].Shadow += 0.25f; Quad.Verts[2].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 25)) { Quad.Verts[2].Shadow += 0.25f; Quad.Verts[3].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 17)) { Quad.Verts[3].Shadow += 0.25f; Quad.Verts[0].Shadow += 0.25f; }

    if ((OpaqueMask & (1 <<  8)) && (Quad.Verts[0].Shadow == DefaultShadow)) Quad.Verts[0].Shadow += 0.25f;
    if ((OpaqueMask & (1 <<  6)) && (Quad.Verts[1].Shadow == DefaultShadow)) Quad.Verts[1].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 24)) && (Quad.Verts[2].Shadow == DefaultShadow)) Quad.Verts[2].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 26)) && (Quad.Verts[3].Shadow == DefaultShadow)) Quad.Verts[3].Shadow += 0.25f;
    
    if (OpaqueMask & ((1 << 6) | (1 << 26)))
        return Quad_RotateVerts(Quad);
    return Quad;
}

quad Block_BottomFace(u32 Index, vec3 Position, u32 OpaqueMask)
{
    f32 DefaultShadow = 0.375f;
    quad Quad = {
        .Verts[0] = { .Position = { 0,0,1 }, .TexCoord = {  0,  0 }, .Shadow = DefaultShadow },
        .Verts[1] = { .Position = { 1,0,1 }, .TexCoord = { 16,  0 }, .Shadow = DefaultShadow },
        .Verts[2] = { .Position = { 1,0,0 }, .TexCoord = { 16, 16 }, .Shadow = DefaultShadow },
        .Verts[3] = { .Position = { 0,0,0 }, .TexCoord = {  0, 16 }, .Shadow = DefaultShadow },
    };
    Quad = Quad_MovePosition(Quad, Position);
    
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    Quad = Quad_MoveTexCoord(Quad, UV);
    
    if (OpaqueMask & (1 << 7)) { Quad.Verts[0].Shadow += 0.25f; Quad.Verts[1].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 5)) { Quad.Verts[1].Shadow += 0.25f; Quad.Verts[2].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 1)) { Quad.Verts[2].Shadow += 0.25f; Quad.Verts[3].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 3)) { Quad.Verts[3].Shadow += 0.25f; Quad.Verts[0].Shadow += 0.25f; }

    if ((OpaqueMask & (1 << 6)) && (Quad.Verts[0].Shadow == DefaultShadow)) Quad.Verts[0].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 8)) && (Quad.Verts[1].Shadow == DefaultShadow)) Quad.Verts[1].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 2)) && (Quad.Verts[2].Shadow == DefaultShadow)) Quad.Verts[2].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 0)) && (Quad.Verts[3].Shadow == DefaultShadow)) Quad.Verts[3].Shadow += 0.25f;
    
    if (OpaqueMask & ((1 << 6) | (1 << 2)))
        return Quad_RotateVerts(Quad);
    return Quad;
}

quad Block_TopFace(u32 Index, vec3 Position, u32 OpaqueMask)
{
    f32 DefaultShadow = 0.0f;
    quad Quad = {
        .Verts[0] = { .Position = { 0,1,0 }, .TexCoord = {  0,  0 }, .Shadow = DefaultShadow },
        .Verts[1] = { .Position = { 1,1,0 }, .TexCoord = { 16,  0 }, .Shadow = DefaultShadow },
        .Verts[2] = { .Position = { 1,1,1 }, .TexCoord = { 16, 16 }, .Shadow = DefaultShadow },
        .Verts[3] = { .Position = { 0,1,1 }, .TexCoord = {  0, 16 }, .Shadow = DefaultShadow },
    };
    Quad = Quad_MovePosition(Quad, Position);
    
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    Quad = Quad_MoveTexCoord(Quad, UV);
    
    if (OpaqueMask & (1 << 19)) { Quad.Verts[0].Shadow += 0.25f; Quad.Verts[1].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 23)) { Quad.Verts[1].Shadow += 0.25f; Quad.Verts[2].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 25)) { Quad.Verts[2].Shadow += 0.25f; Quad.Verts[3].Shadow += 0.25f; }
    if (OpaqueMask & (1 << 21)) { Quad.Verts[3].Shadow += 0.25f; Quad.Verts[0].Shadow += 0.25f; }

    if ((OpaqueMask & (1 << 18)) && (Quad.Verts[0].Shadow == DefaultShadow)) Quad.Verts[0].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 20)) && (Quad.Verts[1].Shadow == DefaultShadow)) Quad.Verts[1].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 26)) && (Quad.Verts[2].Shadow == DefaultShadow)) Quad.Verts[2].Shadow += 0.25f;
    if ((OpaqueMask & (1 << 24)) && (Quad.Verts[3].Shadow == DefaultShadow)) Quad.Verts[3].Shadow += 0.25f;
    
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

u32 Chunk_OpaqueMask(chunk *Chunk, vec3 Position)
{
    u32 Mask = 0;
    ivec3 One = (ivec3) { 1, 1, 1 };
    ivec3 iPos = Vec3_FloorToIVec3(Position);
    ivec3 iMin = iVec3_Max(iVec3_Sub(iPos, One), (ivec3) {  0,  0,  0 });
    ivec3 iMax = iVec3_Min(iVec3_Add(iPos, One), (ivec3) { 15, 15, 15 });
    for (i32 y = iMin.y; y <= iMax.y; ++y)
    for (i32 z = iMin.z; z <= iMax.z; ++z)
    for (i32 x = iMin.x; x <= iMax.x; ++x)
    {
        u8 BlockId = Chunk->Blocks[x][z][y].Id;
        if ((BlockId != BLOCK_ID_AIR) &&
            (BlockId != BLOCK_ID_LEAVES))
        {
            Mask |= 1 << ((x+1-iPos.x) + (z+1-iPos.z)*3 + (y+1-iPos.y)*9);
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
    for (i32 y = iMin.y; y <= iMax.y; ++y)
    for (i32 z = iMin.z; z <= iMax.z; ++z)
    for (i32 x = iMin.x; x <= iMax.x; ++x)
    {
        u8 BlockId = Chunk->Blocks[x][z][y].Id;
        if (BlockId == Block)
        {
            Mask |= 1 << ((x+1-iPos.x) + (z+1-iPos.z)*3 + (y+1-iPos.y)*9);
        }
    }
    return Mask;
}

void Chunk_GenerateMesh(chunk *Chunk)
{
    Mesh_Clear(&Chunk->Mesh);

    for (i32 x = 0; x < CHUNK_WIDTH; ++x)
    for (i32 z = 0; z < CHUNK_WIDTH; ++z)
    for (i32 y = 0; y < CHUNK_HEIGHT; ++y)
    {
        world_block *CurrentBlock = &Chunk->Blocks[x][z][y];
        
        vec3 BlockPosition = (vec3){ (f32)x, (f32)y, (f32)z };

        switch (CurrentBlock->Id)
        {
            case BLOCK_ID_GRAS:
            {
                u32 OpaqueMask = Chunk_OpaqueMask(Chunk, BlockPosition);
                Chunk_AddBlockQuads(&Chunk->Mesh, BlockPosition, OpaqueMask, 1, 1, 1, 1, 2, 0);
            } break;

            case BLOCK_ID_WOOD:
            {
                u32 OpaqueMask = Chunk_OpaqueMask(Chunk, BlockPosition);
                Chunk_AddBlockQuads(&Chunk->Mesh, BlockPosition, OpaqueMask, 7, 7, 7, 7, 15, 15);
            } break;

            case BLOCK_ID_LEAVES:
            {
                u32 OpaqueMask = Chunk_OpaqueMask(Chunk, BlockPosition);
                // u32 LeaveMask = Chunk_BlockMask(Chunk, BlockPosition, BLOCK_ID_LEAVES);
                Chunk_AddBlockQuads(&Chunk->Mesh, BlockPosition, OpaqueMask, 3, 3, 3, 3, 3, 3);
            } break;
        }
    }
}

void Chunk_Draw(const camera Camera, const bitmap Target, bitmap TerrainTexture, chunk *Chunk)
{
    vec3 ChunkDim = (vec3) { CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH };
    vec3 ChunkMin = Vec3_Mul(ChunkDim, (vec3) { (f32)Chunk->x, 0, (f32)Chunk->z});
    vec3 ChunkMax = Vec3_Add(ChunkDim, ChunkMin);
    if (!Camera_BoxVisible(Camera, Target, ChunkMin, ChunkMax)) return;
    Mesh_Draw(Target, Camera, TerrainTexture, ChunkMin, &Chunk->Mesh);
}

void Chunk_Create(chunk *Chunk, i32 x, i32 z)
{
    Chunk->Allocated = true;
    Chunk->x = x;
    Chunk->z = z;

    for (i32 xx = 0; xx < CHUNK_WIDTH; xx++)
    for (i32 zz = 0; zz < CHUNK_WIDTH; zz++)
    for (i32 yy = 0; yy < CHUNK_HEIGHT; yy++)
    {
        world_block* Current_Block = &Chunk->Blocks[xx][zz][yy];

        if ((yy == 0) || 
            (zz == 1 && xx == 2 && yy == 1) ||  
            (zz == 2 && xx == 3 && yy == 1) ||
            (zz == 9 && xx == 1 && yy == 1) ||
            (zz == 8 && xx == 2 && yy == 1) ||
            (zz == 8 && xx == 2 && yy == 6))
        {
            Current_Block->Id = BLOCK_ID_GRAS;
        }
        else if (zz == 5 && xx == 5 && yy <= 5)
        {
            Current_Block->Id = BLOCK_ID_WOOD;
        }
        else if (((zz >= 3) && (zz <= 7) && (xx >= 4) && (xx <= 6) && (yy >= 3) && (yy <= 5)) ||
                 ((zz >= 4) && (zz <= 6) && (xx >= 3) && (xx <= 7) && (yy >= 3) && (yy <= 5)) ||
                 ((zz == 5) && (xx >= 4) && (xx <= 6) && (yy == 6)) ||
                 ((zz >= 4) && (zz <= 6) && (xx == 5) && (yy == 6)))
        {
            Current_Block->Id = BLOCK_ID_LEAVES;
        }
        else
        {
            Current_Block->Id = BLOCK_ID_AIR;
        }
    }

    Chunk->Mesh = Mesh_Create();
    Chunk_GenerateMesh(Chunk);

    // TODO: creation code
}

void Chunk_Delete(chunk *Chunk, i32 x, i32 z)
{
    Chunk->Allocated = false;
    assert(Chunk->x == x);
    assert(Chunk->z == z);

    Mesh_Delete(&Chunk->Mesh);

    // TODO: deletion code
}
