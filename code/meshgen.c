
quad Block_FaceQuad(vec3 Position, vec3 Right, vec3 Up, vec2 UV, vec2 U, vec2 V, f32 *Shadow)
{
    return (quad){
        .Verts = {
            [0] = {
                .Position = Position,
                .TexCoord = UV,
                .Shadow = Shadow[0],
            },
            [1] = {
                .Position = Vec3_Add(Position, Right),
                .TexCoord = Vec2_Add(UV, U),
                .Shadow = Shadow[1],
            },
            [2] = {
                .Position = Vec3_Add(Position, Vec3_Add(Right, Up)),
                .TexCoord = Vec2_Add(UV, Vec2_Add(U, V)),
                .Shadow = Shadow[2],
            },
            [3] = {
                .Position = Vec3_Add(Position, Up),
                .TexCoord = Vec2_Add(UV, V),
                .Shadow = Shadow[3],
            },
        },
    };
}

quad Block_LeftFace(vec3 Position, u32 Index, const block_group *BlockGroup)
{
    f32 ShadowValues[3][3] = {
        { BlockGroup->Blocks[0][2][0].Shadow, BlockGroup->Blocks[0][1][0].Shadow, BlockGroup->Blocks[0][0][0].Shadow, },
        { BlockGroup->Blocks[1][2][0].Shadow, BlockGroup->Blocks[1][1][0].Shadow, BlockGroup->Blocks[1][0][0].Shadow, },
        { BlockGroup->Blocks[2][2][0].Shadow, BlockGroup->Blocks[2][1][0].Shadow, BlockGroup->Blocks[2][0][0].Shadow, },
    };
    f32 Shadow[4] = {
        (ShadowValues[0][0] + ShadowValues[0][1] + ShadowValues[1][0] + ShadowValues[1][1]) * (0.25f / 15.0f),
        (ShadowValues[0][1] + ShadowValues[0][2] + ShadowValues[1][1] + ShadowValues[1][2]) * (0.25f / 15.0f),
        (ShadowValues[1][1] + ShadowValues[1][2] + ShadowValues[2][1] + ShadowValues[2][2]) * (0.25f / 15.0f),
        (ShadowValues[1][0] + ShadowValues[1][1] + ShadowValues[2][0] + ShadowValues[2][1]) * (0.25f / 15.0f),
    };
    vec3 Pos = Vec3_Add(Position, (vec3){0,1,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){0,-1,0}, (vec3){0,0,1}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    if ((ShadowValues[0][0] + ShadowValues[2][2]) > (ShadowValues[0][2] + ShadowValues[2][0]))
        Face = Quad_RotateVerts(Face);
    return Face;
}

quad Block_RightFace(vec3 Position, u32 Index, const block_group *BlockGroup)
{
    f32 ShadowValues[3][3] = {
        { BlockGroup->Blocks[0][0][2].Shadow, BlockGroup->Blocks[0][1][2].Shadow, BlockGroup->Blocks[0][2][2].Shadow, },
        { BlockGroup->Blocks[1][0][2].Shadow, BlockGroup->Blocks[1][1][2].Shadow, BlockGroup->Blocks[1][2][2].Shadow, },
        { BlockGroup->Blocks[2][0][2].Shadow, BlockGroup->Blocks[2][1][2].Shadow, BlockGroup->Blocks[2][2][2].Shadow, },
    };
    f32 Shadow[4] = {
        (ShadowValues[0][0] + ShadowValues[0][1] + ShadowValues[1][0] + ShadowValues[1][1]) * (0.25f / 15.0f),
        (ShadowValues[0][1] + ShadowValues[0][2] + ShadowValues[1][1] + ShadowValues[1][2]) * (0.25f / 15.0f),
        (ShadowValues[1][1] + ShadowValues[1][2] + ShadowValues[2][1] + ShadowValues[2][2]) * (0.25f / 15.0f),
        (ShadowValues[1][0] + ShadowValues[1][1] + ShadowValues[2][0] + ShadowValues[2][1]) * (0.25f / 15.0f),
    };
    vec3 Pos = Vec3_Add(Position, (vec3){1,0,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){0,1,0}, (vec3){0,0,1}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    if ((ShadowValues[0][0] + ShadowValues[2][2]) > (ShadowValues[0][2] + ShadowValues[2][0]))
        Face = Quad_RotateVerts(Face);
    return Face;
}

quad Block_FrontFace(vec3 Position, u32 Index, const block_group *BlockGroup)
{
    f32 ShadowValues[3][3] = {
        { BlockGroup->Blocks[0][0][0].Shadow, BlockGroup->Blocks[0][0][1].Shadow, BlockGroup->Blocks[0][0][2].Shadow, },
        { BlockGroup->Blocks[1][0][0].Shadow, BlockGroup->Blocks[1][0][1].Shadow, BlockGroup->Blocks[1][0][2].Shadow, },
        { BlockGroup->Blocks[2][0][0].Shadow, BlockGroup->Blocks[2][0][1].Shadow, BlockGroup->Blocks[2][0][2].Shadow, },
    };
    f32 Shadow[4] = {
        (ShadowValues[0][0] + ShadowValues[0][1] + ShadowValues[1][0] + ShadowValues[1][1]) * (0.25f / 15.0f),
        (ShadowValues[0][1] + ShadowValues[0][2] + ShadowValues[1][1] + ShadowValues[1][2]) * (0.25f / 15.0f),
        (ShadowValues[1][1] + ShadowValues[1][2] + ShadowValues[2][1] + ShadowValues[2][2]) * (0.25f / 15.0f),
        (ShadowValues[1][0] + ShadowValues[1][1] + ShadowValues[2][0] + ShadowValues[2][1]) * (0.25f / 15.0f),
    };
    vec3 Pos = Vec3_Add(Position, (vec3){0,0,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){1,0,0}, (vec3){0,0,1}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    if ((ShadowValues[0][0] + ShadowValues[2][2]) > (ShadowValues[0][2] + ShadowValues[2][0]))
        Face = Quad_RotateVerts(Face);
    return Face;
}

quad Block_BackFace(vec3 Position, u32 Index, const block_group *BlockGroup)
{
    f32 ShadowValues[3][3] = {
        { BlockGroup->Blocks[0][2][2].Shadow, BlockGroup->Blocks[0][2][1].Shadow, BlockGroup->Blocks[0][2][0].Shadow, },
        { BlockGroup->Blocks[1][2][2].Shadow, BlockGroup->Blocks[1][2][1].Shadow, BlockGroup->Blocks[1][2][0].Shadow, },
        { BlockGroup->Blocks[2][2][2].Shadow, BlockGroup->Blocks[2][2][1].Shadow, BlockGroup->Blocks[2][2][0].Shadow, },
    };
    f32 Shadow[4] = {
        (ShadowValues[0][0] + ShadowValues[0][1] + ShadowValues[1][0] + ShadowValues[1][1]) * (0.25f / 15.0f),
        (ShadowValues[0][1] + ShadowValues[0][2] + ShadowValues[1][1] + ShadowValues[1][2]) * (0.25f / 15.0f),
        (ShadowValues[1][1] + ShadowValues[1][2] + ShadowValues[2][1] + ShadowValues[2][2]) * (0.25f / 15.0f),
        (ShadowValues[1][0] + ShadowValues[1][1] + ShadowValues[2][0] + ShadowValues[2][1]) * (0.25f / 15.0f),
    };
    vec3 Pos = Vec3_Add(Position, (vec3){1,1,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){-1,0,0}, (vec3){0,0,1}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    if ((ShadowValues[0][0] + ShadowValues[2][2]) > (ShadowValues[0][2] + ShadowValues[2][0]))
        Face = Quad_RotateVerts(Face);
    return Face;
}

quad Block_BottomFace(vec3 Position, u32 Index, const block_group *BlockGroup)
{
    f32 ShadowValues[3][3] = {
        { BlockGroup->Blocks[0][2][0].Shadow, BlockGroup->Blocks[0][2][1].Shadow, BlockGroup->Blocks[0][2][2].Shadow, },
        { BlockGroup->Blocks[0][1][0].Shadow, BlockGroup->Blocks[0][1][1].Shadow, BlockGroup->Blocks[0][1][2].Shadow, },
        { BlockGroup->Blocks[0][0][0].Shadow, BlockGroup->Blocks[0][0][1].Shadow, BlockGroup->Blocks[0][0][2].Shadow, },
    };
    f32 Shadow[4] = {
        (ShadowValues[0][0] + ShadowValues[0][1] + ShadowValues[1][0] + ShadowValues[1][1]) * (0.25f / 15.0f),
        (ShadowValues[0][1] + ShadowValues[0][2] + ShadowValues[1][1] + ShadowValues[1][2]) * (0.25f / 15.0f),
        (ShadowValues[1][1] + ShadowValues[1][2] + ShadowValues[2][1] + ShadowValues[2][2]) * (0.25f / 15.0f),
        (ShadowValues[1][0] + ShadowValues[1][1] + ShadowValues[2][0] + ShadowValues[2][1]) * (0.25f / 15.0f),
    };
    vec3 Pos = Vec3_Add(Position, (vec3){0,1,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){1,0,0}, (vec3){0,-1,0}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    if ((ShadowValues[0][0] + ShadowValues[2][2]) > (ShadowValues[0][2] + ShadowValues[2][0]))
        Face = Quad_RotateVerts(Face);
    return Face;
}

quad Block_TopFace(vec3 Position, u32 Index, const block_group *BlockGroup)
{
    f32 ShadowValues[3][3] = {
        { BlockGroup->Blocks[2][0][0].Shadow, BlockGroup->Blocks[2][0][1].Shadow, BlockGroup->Blocks[2][0][2].Shadow, },
        { BlockGroup->Blocks[2][1][0].Shadow, BlockGroup->Blocks[2][1][1].Shadow, BlockGroup->Blocks[2][1][2].Shadow, },
        { BlockGroup->Blocks[2][2][0].Shadow, BlockGroup->Blocks[2][2][1].Shadow, BlockGroup->Blocks[2][2][2].Shadow, },
    };
    f32 Shadow[4] = {
        (ShadowValues[0][0] + ShadowValues[0][1] + ShadowValues[1][0] + ShadowValues[1][1]) * (0.25f / 15.0f),
        (ShadowValues[0][1] + ShadowValues[0][2] + ShadowValues[1][1] + ShadowValues[1][2]) * (0.25f / 15.0f),
        (ShadowValues[1][1] + ShadowValues[1][2] + ShadowValues[2][1] + ShadowValues[2][2]) * (0.25f / 15.0f),
        (ShadowValues[1][0] + ShadowValues[1][1] + ShadowValues[2][0] + ShadowValues[2][1]) * (0.25f / 15.0f),
    };
    vec3 Pos = Vec3_Add(Position, (vec3){0,0,1});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){1,0,0}, (vec3){0,1,0}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    if ((ShadowValues[0][0] + ShadowValues[2][2]) > (ShadowValues[0][2] + ShadowValues[2][0]))
        Face = Quad_RotateVerts(Face);
    return Face;
}

void Block_AddQuadsToMesh(quad_mesh *Mesh, vec3 Position, const block_group *BlockGroup, 
                          u32 Left, u32 Right, u32 Front, u32 Back, u32 Bottom, u32 Top)
{
    // Left
    if (!Block_Opaque[BlockGroup->Blocks[1][1][0].Id])
    {
        Mesh_AddQuad(Mesh, Block_LeftFace(Position, Left, BlockGroup));
    }

    // Right
    if (!Block_Opaque[BlockGroup->Blocks[1][1][2].Id])
    {
        Mesh_AddQuad(Mesh, Block_RightFace(Position, Right, BlockGroup));
    }

    // Front
    if (!Block_Opaque[BlockGroup->Blocks[1][0][1].Id])
    {
        Mesh_AddQuad(Mesh, Block_FrontFace(Position, Front, BlockGroup));
    }

    // Back
    if (!Block_Opaque[BlockGroup->Blocks[1][2][1].Id])
    {
        Mesh_AddQuad(Mesh, Block_BackFace(Position, Back, BlockGroup));
    }

    // Bottom
    if (!Block_Opaque[BlockGroup->Blocks[0][1][1].Id])
    {
        Mesh_AddQuad(Mesh, Block_BottomFace(Position, Bottom, BlockGroup));
    }

    // Top
    if (!Block_Opaque[BlockGroup->Blocks[2][1][1].Id])
    {
        Mesh_AddQuad(Mesh, Block_TopFace(Position, Top, BlockGroup));
    }
}

typedef struct padded_chunk
{
    block Blocks[CHUNK_HEIGHT+2][CHUNK_WIDTH+2][CHUNK_WIDTH+2];
} padded_chunk;

void View_PaddedChunk(view *View, ivec2 ChunkPosition, padded_chunk *PaddedChunk)
{
    for (i32 y = 0; y < CHUNK_WIDTH+2; ++y)
    for (i32 x = 0; x < CHUNK_WIDTH+2; ++x)
    {
        PaddedChunk->Blocks[0][y][x] = DEFAULT_HELL_BLOCK;
        PaddedChunk->Blocks[CHUNK_HEIGHT+1][y][x] = DEFAULT_SKY_BLOCK;
    }

    view_chunk *Chunk = View_GetChunk(View, ChunkPosition);
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (i32 y = 0; y < CHUNK_WIDTH; ++y)
    for (i32 x = 0; x < CHUNK_WIDTH; ++x)
    {
        PaddedChunk->Blocks[z+1][y+1][x+1] = Chunk->Chunk.Blocks[z][y][x];
    }

    Chunk = View_GetChunk(View, iVec2_Add(ChunkPosition, (ivec2){-1,-1}));
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    {
        PaddedChunk->Blocks[z+1][0][0] = Chunk->Chunk.Blocks[z][CHUNK_WIDTH-1][CHUNK_WIDTH-1];
    }

    Chunk = View_GetChunk(View, iVec2_Add(ChunkPosition, (ivec2){1,-1}));
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    {
        PaddedChunk->Blocks[z+1][0][CHUNK_WIDTH+1] = Chunk->Chunk.Blocks[z][CHUNK_WIDTH-1][0];
    }

    Chunk = View_GetChunk(View, iVec2_Add(ChunkPosition, (ivec2){-1,1}));
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    {
        PaddedChunk->Blocks[z+1][CHUNK_WIDTH+1][0] = Chunk->Chunk.Blocks[z][0][CHUNK_WIDTH-1];
    }

    Chunk = View_GetChunk(View, iVec2_Add(ChunkPosition, (ivec2){1,1}));
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    {
        PaddedChunk->Blocks[z+1][CHUNK_WIDTH+1][CHUNK_WIDTH+1] = Chunk->Chunk.Blocks[z][0][0];
    }

    Chunk = View_GetChunk(View, iVec2_Add(ChunkPosition, (ivec2){0,-1}));
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (i32 x = 0; x < CHUNK_WIDTH; ++x)
    {
        PaddedChunk->Blocks[z+1][0][x+1] = Chunk->Chunk.Blocks[z][CHUNK_WIDTH-1][x];
    }

    Chunk = View_GetChunk(View, iVec2_Add(ChunkPosition, (ivec2){-1,0}));
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (i32 y = 0; y < CHUNK_WIDTH; ++y)
    {
        PaddedChunk->Blocks[z+1][y+1][0] = Chunk->Chunk.Blocks[z][y][CHUNK_WIDTH-1];
    }

    Chunk = View_GetChunk(View, iVec2_Add(ChunkPosition, (ivec2){0,1}));
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (i32 x = 0; x < CHUNK_WIDTH; ++x)
    {
        PaddedChunk->Blocks[z+1][CHUNK_WIDTH+1][x+1] = Chunk->Chunk.Blocks[z][0][x];
    }

    Chunk = View_GetChunk(View, iVec2_Add(ChunkPosition, (ivec2){1,0}));
    if (Chunk)
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (i32 y = 0; y < CHUNK_WIDTH; ++y)
    {
        PaddedChunk->Blocks[z+1][y+1][CHUNK_WIDTH+1] = Chunk->Chunk.Blocks[z][y][0];
    }
}

block_group PaddedChunk_GetBlockGroup(padded_chunk *Chunk, ivec3 WorldPosition)
{
    block_group BlockGroup;
    for (i32 z = 0; z < 3; ++z)
    for (i32 y = 0; y < 3; ++y)
    for (i32 x = 0; x < 3; ++x)
    {
        ivec3 BlockPosition = (ivec3){ x, y, z };
        BlockPosition = iVec3_Add(BlockPosition, WorldPosition);
        BlockGroup.Blocks[z][y][x] = Chunk->Blocks[BlockPosition.z][BlockPosition.y][BlockPosition.x];
    }
    return BlockGroup;
}

block PaddedChunk_GetBlock(padded_chunk *Chunk, ivec3 WorldPosition)
{
    ivec3 BlockPosition = World_ToBlockPosition(WorldPosition);
    BlockPosition = iVec3_Add(WorldPosition, iVec3_Set1(1));
    return Chunk->Blocks[BlockPosition.z][BlockPosition.y][BlockPosition.x];
}

void View_GenerateChunkMesh(view *View, ivec2 ChunkPosition)
{
    view_chunk *Chunk = View_GetChunk(View, ChunkPosition);

    Mesh_Clear(&Chunk->Mesh);

    padded_chunk PaddedChunk = { 0 };
    View_PaddedChunk(View, ChunkPosition, &PaddedChunk);

    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (i32 y = 0; y < CHUNK_WIDTH; ++y)
    for (i32 x = 0; x < CHUNK_WIDTH; ++x)
    {
        ivec3 iBlockPosition = (ivec3){ x, y, z };
        block Block = PaddedChunk_GetBlock(&PaddedChunk, iBlockPosition);
        if (Block.Id == BLOCK_ID_AIR) continue;

        block_group BlockGroup = PaddedChunk_GetBlockGroup(&PaddedChunk, iBlockPosition);

        vec3 BlockPosition = iVec3_toVec3(iBlockPosition);
        switch (Block.Id)
        {
            case BLOCK_ID_DIRT:
            {
                Block_AddQuadsToMesh(&Chunk->Mesh, BlockPosition, &BlockGroup, 2, 2, 2, 2, 2, 2);
            } break;

            case BLOCK_ID_GRAS:
            {
                Block_AddQuadsToMesh(&Chunk->Mesh, BlockPosition, &BlockGroup, 1, 1, 1, 1, 2, 0);
            } break;

            case BLOCK_ID_STONE:
            {
                Block_AddQuadsToMesh(&Chunk->Mesh, BlockPosition, &BlockGroup, 5, 5, 5, 5, 5, 5);
            } break;

            case BLOCK_ID_COBBLE:
            {
                Block_AddQuadsToMesh(&Chunk->Mesh, BlockPosition, &BlockGroup, 4, 4, 4, 4, 4, 4);
            } break;

            case BLOCK_ID_WOOD:
            {
                Block_AddQuadsToMesh(&Chunk->Mesh, BlockPosition, &BlockGroup, 7, 7, 7, 7, 15, 15);
            } break;

            case BLOCK_ID_LEAVES:
            {
                Block_AddQuadsToMesh(&Chunk->Mesh, BlockPosition, &BlockGroup, 3, 3, 3, 3, 3, 3);
            } break;

            case BLOCK_ID_SAND:
            {
                Block_AddQuadsToMesh(&Chunk->Mesh, BlockPosition, &BlockGroup, 6, 6, 6, 6, 6, 6);
            } break;
        }
    }

    Chunk->MeshDirty = false;
}

void Mob_AddBlockMesh(quad_mesh* Mesh, vec3 Position, f32 Height, f32 Width)
{
    vec2 UV = { 32.0f, 0.0f };
    f32 Shadow[4] = { 0, 0, 0, 0 };
    vec3 Pos;
    quad Face;
    
    //Left
    Pos = Vec3_Add(Position, (vec3) { -Width / 2, Width / 2, -Height / 2 });
    Face = Block_FaceQuad(Pos, (vec3) { 0, -Width, 0 }, (vec3) { 0, 0, Height }, UV, (vec2) { 16, 0 }, (vec2) { 0, 16 }, Shadow);
    Mesh_AddQuad(Mesh, Face);
    
    //Right
    Pos = Vec3_Add(Position, (vec3) { Width / 2, Width / 2, -Height / 2 });
    Face = Block_FaceQuad(Pos, (vec3) { 0, 0, Height }, (vec3){ 0, -Width, 0 }, UV, (vec2) { 16, 0 }, (vec2) { 0, 16 }, Shadow);
    Mesh_AddQuad(Mesh, Face);
    //Front
    Pos = Vec3_Add(Position, (vec3) { -Width / 2, -Width / 2, -Height / 2 });
    Face = Block_FaceQuad(Pos, (vec3) { Width, 0, 0 }, (vec3) { 0, 0, Height }, UV, (vec2) { 16, 0 }, (vec2) { 0, 16 }, Shadow);
    Mesh_AddQuad(Mesh, Face);
    //Back
    Pos = Vec3_Add(Position, (vec3) { -Width / 2, Width / 2, -Height / 2 });
    Face = Block_FaceQuad(Pos, (vec3) { 0, 0, Height }, (vec3) { Width, 0, 0 }, UV, (vec2) { 16, 0 }, (vec2) { 0, 16 }, Shadow);
    Mesh_AddQuad(Mesh, Face);
    //Bottom
    Pos = Pos = Vec3_Add(Position, (vec3) { -Width / 2, -Width / 2, -Height / 2 });
    Face = Block_FaceQuad(Pos, (vec3) { 0, Width, 0 }, (vec3) { Width, 0, 0 }, UV, (vec2) { 16, 0 }, (vec2) { 0, 16 }, Shadow);
    Mesh_AddQuad(Mesh, Face);
    //Top
    Pos = Vec3_Add(Position, (vec3) { -Width / 2, -Width / 2, Height / 2 });
    Face = Block_FaceQuad(Pos, (vec3) { Width, 0, 0 }, (vec3) { 0, Width, 0 }, UV, (vec2) { 16, 0 }, (vec2) { 0, 16 }, Shadow);
    Mesh_AddQuad(Mesh, Face);
    
}

void Mob_AddMesh(quad_mesh* Mesh, vec3 Position)
{
    f32 Height = 1.0f;
    f32 Width = 2.0f;
    Mob_AddBlockMesh(Mesh, Position, Height, Width);
}

void View_GenerateMobMesh(view_entity *Entity)
{
    if (ENTITY_NONE == Entity->Entity.Type) return;

    Entity->Model = Mesh_Create();
    switch (Entity->Entity.Type)
    {
    case(ENTITY_MOB):
        Mob_AddMesh(&Entity->Model, Entity->Entity.Position); break;
    }

}
