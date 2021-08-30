
#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 256

typedef struct chunk
{
    i32 x, z;
    bool Allocated;

    world_quad* Quads;
    u32 QuadCount;

    world_block Blocks[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_HEIGHT];
} chunk;



world_quad Block_TopFace(u8 Index, vec3 Position, u32 OpaqueMask)
{
    vertex V0 = { .Position = { 0,1,0 }, .TexCoord = {  0,  0 }, };
    vertex V1 = { .Position = { 1,1,0 }, .TexCoord = { 16,  0 }, };
    vertex V2 = { .Position = { 1,1,1 }, .TexCoord = { 16, 16 }, };
    vertex V3 = { .Position = { 0,1,1 }, .TexCoord = {  0, 16 }, };
    
    V0.Position = Vec3_Add(V0.Position, Position);
    V1.Position = Vec3_Add(V1.Position, Position);
    V2.Position = Vec3_Add(V2.Position, Position);
    V3.Position = Vec3_Add(V3.Position, Position);
    
    vec2 UV = { (Index & 15) * 16.0f, (Index >> 4) * 16.0f };
    V0.TexCoord = Vec2_Add(V0.TexCoord, UV);
    V1.TexCoord = Vec2_Add(V1.TexCoord, UV);
    V2.TexCoord = Vec2_Add(V2.TexCoord, UV);
    V3.TexCoord = Vec2_Add(V3.TexCoord, UV);
    
    if (OpaqueMask & (1 << 19)) { V0.Shadow += 0.25f; V1.Shadow += 0.25f; }
    if (OpaqueMask & (1 << 21)) { V0.Shadow += 0.25f; V3.Shadow += 0.25f; }
    if (OpaqueMask & (1 << 23)) { V1.Shadow += 0.25f; V2.Shadow += 0.25f; }
    if (OpaqueMask & (1 << 25)) { V2.Shadow += 0.25f; V3.Shadow += 0.25f; }

    if ((OpaqueMask & (1 << 18)) && (V0.Shadow == 0.0f)) V0.Shadow += 0.25f;
    if ((OpaqueMask & (1 << 20)) && (V1.Shadow == 0.0f)) V1.Shadow += 0.25f;
    if ((OpaqueMask & (1 << 26)) && (V2.Shadow == 0.0f)) V2.Shadow += 0.25f;
    if ((OpaqueMask & (1 << 24)) && (V3.Shadow == 0.0f)) V3.Shadow += 0.25f;
    
    if (OpaqueMask & (1 << 18) || OpaqueMask & (1 << 26))
        return (world_quad){ V3, V0, V1, V2 };
    return (world_quad){ V0, V1, V2, V3 };
}

u32 Chunk_BlockMask(chunk *Chunk, vec3 Position)
{
    u32 Mask = 0;
    ivec3 One = (ivec3) { 1, 1, 1 };
    ivec3 iPos = Vec3_FloorToIVec3(Position);
    ivec3 iMin = iVec3_Max(iVec3_Sub(iPos, One), (ivec3) {  0,  0,  0 });
    ivec3 iMax = iVec3_Min(iVec3_Add(iPos, One), (ivec3) { 16, 16, 16 });
    for (i32 x = iMin.x; x <= iMax.x; ++x)
    for (i32 z = iMin.z; z <= iMax.z; ++z)
    for (i32 y = iMin.y; y <= iMax.y; ++y)
    {
        if (Chunk->Blocks[x][z][y].Id != BLOCK_ID_AIR)
        {
            Mask |= 1 << ((x+1-iPos.x) + (z+1-iPos.z)*3 + (y+1-iPos.y)*9);
        }
    }
    return Mask;
}

void Chunk_GatherQuads(chunk *Chunk)
{
    if (Chunk->Quads)
        free(Chunk->Quads);

    Chunk->QuadCount = 0;
    Chunk->Quads = malloc(sizeof(world_quad) * WORLD_CHUNK_WIDTH * WORLD_CHUNK_WIDTH * WORLD_CHUNK_HEIGHT * 3);
    
    for (i32 x = 0; x < CHUNK_WIDTH; ++x)
    for (i32 z = 0; z < CHUNK_WIDTH; ++z)
    for (i32 y = 0; y < CHUNK_HEIGHT; ++y)
    {
        world_block* CurrentBlock = &Chunk->Blocks[x][z][y];

        if (CurrentBlock->Id != BLOCK_ID_AIR)
        {
            vec3 BlockPosition = (vec3){ (f32)x, (f32)y, (f32)z };

            u32 OpaqueMask = Chunk_BlockMask(Chunk, BlockPosition);

            vec3 Corners[8] = {
                {.x = -0.0f + BlockPosition.x, .y = -0.0f + BlockPosition.y, .z = -0.0f + BlockPosition.z },
                {.x = +1.0f + BlockPosition.x, .y = -0.0f + BlockPosition.y, .z = -0.0f + BlockPosition.z },
                {.x = -0.0f + BlockPosition.x, .y = +1.0f + BlockPosition.y, .z = -0.0f + BlockPosition.z },
                {.x = +1.0f + BlockPosition.x, .y = +1.0f + BlockPosition.y, .z = -0.0f + BlockPosition.z },
                {.x = -0.0f + BlockPosition.x, .y = -0.0f + BlockPosition.y, .z = +1.0f + BlockPosition.z },
                {.x = +1.0f + BlockPosition.x, .y = -0.0f + BlockPosition.y, .z = +1.0f + BlockPosition.z },
                {.x = -0.0f + BlockPosition.x, .y = +1.0f + BlockPosition.y, .z = +1.0f + BlockPosition.z },
                {.x = +1.0f + BlockPosition.x, .y = +1.0f + BlockPosition.y, .z = +1.0f + BlockPosition.z },
            };
            //Left, Right, Front, Back, Bottom, Top

            // Left
            if (x == 0 || (Chunk->Blocks[x - 1][z][y].Id == BLOCK_ID_AIR))
            {
                vertex V0 = (vertex){ .TexCoord.u =  0 + 16, .TexCoord.v =  0, .Shadow = 0.5f };
                vertex V1 = (vertex){ .TexCoord.u = 16 + 16, .TexCoord.v =  0, .Shadow = 0.5f };
                vertex V2 = (vertex){ .TexCoord.u = 16 + 16, .TexCoord.v = 16, .Shadow = 0.0f };
                vertex V3 = (vertex){ .TexCoord.u =  0 + 16, .TexCoord.v = 16, .Shadow = 0.0f };
                V0.Position = Corners[4];
                V1.Position = Corners[0];
                V2.Position = Corners[2];
                V3.Position = Corners[6];
                Chunk->Quads[Chunk->QuadCount++] = (world_quad){V0,V1,V2,V3};
            }

            // Right
            if ((x == WORLD_CHUNK_WIDTH - 1) || (Chunk->Blocks[x + 1][z][y].Id == BLOCK_ID_AIR))
            {
                vertex V0 = (vertex){ .TexCoord.u =  0 + 16, .TexCoord.v =  0, .Shadow = 0.5f };
                vertex V1 = (vertex){ .TexCoord.u = 16 + 16, .TexCoord.v =  0, .Shadow = 0.5f };
                vertex V2 = (vertex){ .TexCoord.u = 16 + 16, .TexCoord.v = 16, .Shadow = 0.0f };
                vertex V3 = (vertex){ .TexCoord.u =  0 + 16, .TexCoord.v = 16, .Shadow = 0.0f };
                V0.Position = Corners[1];
                V1.Position = Corners[5];
                V2.Position = Corners[7];
                V3.Position = Corners[3];
                Chunk->Quads[Chunk->QuadCount++] = (world_quad){ V0,V1,V2,V3 };
            }

            // Front
            if (z == 0 || (Chunk->Blocks[x][z - 1][y].Id == BLOCK_ID_AIR))
            {
                vertex V0 = (vertex){ .TexCoord.u =  0 + 16, .TexCoord.v =  0, .Shadow = 0.5f };
                vertex V1 = (vertex){ .TexCoord.u = 16 + 16, .TexCoord.v =  0, .Shadow = 0.5f };
                vertex V2 = (vertex){ .TexCoord.u = 16 + 16, .TexCoord.v = 16, .Shadow = 0.0f };
                vertex V3 = (vertex){ .TexCoord.u =  0 + 16, .TexCoord.v = 16, .Shadow = 0.0f };
                V0.Position = Corners[0];
                V1.Position = Corners[1];
                V2.Position = Corners[3];
                V3.Position = Corners[2];
                Chunk->Quads[Chunk->QuadCount++] = (world_quad){ V0,V1,V2,V3 };
            }

            // Back
            if ((z == WORLD_CHUNK_WIDTH - 1) || (Chunk->Blocks[x][z + 1][y].Id == BLOCK_ID_AIR))
            {
                vertex V0 = (vertex){ .TexCoord.u =  0 + 16, .TexCoord.v =  0, .Shadow = 0.5f };
                vertex V1 = (vertex){ .TexCoord.u = 16 + 16, .TexCoord.v =  0, .Shadow = 0.5f };
                vertex V2 = (vertex){ .TexCoord.u = 16 + 16, .TexCoord.v = 16, .Shadow = 0.0f };
                vertex V3 = (vertex){ .TexCoord.u =  0 + 16, .TexCoord.v = 16, .Shadow = 0.0f };
                V0.Position = Corners[5];
                V1.Position = Corners[4];
                V2.Position = Corners[6];
                V3.Position = Corners[7];
                Chunk->Quads[Chunk->QuadCount++] = (world_quad){ V0,V1,V2,V3 };
            }

            // Bottom
            if (y == 0 || (Chunk->Blocks[x][z][y - 1].Id == BLOCK_ID_AIR))
            {
                vertex V0 = (vertex){ .TexCoord.u =  0 + 32, .TexCoord.v =  0, .Shadow = 0.5f };
                vertex V1 = (vertex){ .TexCoord.u = 16 + 32, .TexCoord.v =  0, .Shadow = 0.5f };
                vertex V2 = (vertex){ .TexCoord.u = 16 + 32, .TexCoord.v = 16, .Shadow = 0.5f };
                vertex V3 = (vertex){ .TexCoord.u =  0 + 32, .TexCoord.v = 16, .Shadow = 0.5f };
                V0.Position = Corners[4];
                V1.Position = Corners[5];
                V2.Position = Corners[1];
                V3.Position = Corners[0];
                Chunk->Quads[Chunk->QuadCount++] = (world_quad){ V0,V1,V2,V3 };
            }

            // Top
            if ((y == WORLD_CHUNK_HEIGHT - 1) || (Chunk->Blocks[x][z][y + 1].Id == BLOCK_ID_AIR))
            {
                Chunk->Quads[Chunk->QuadCount++] = Block_TopFace(0, BlockPosition, OpaqueMask);
                // vertex V0 = (vertex){ .TexCoord.u =  0, .TexCoord.v =  0 };
                // vertex V1 = (vertex){ .TexCoord.u = 16, .TexCoord.v =  0 };
                // vertex V2 = (vertex){ .TexCoord.u = 16, .TexCoord.v = 16 };
                // vertex V3 = (vertex){ .TexCoord.u =  0, .TexCoord.v = 16 };
                // V0.Position = Corners[2];
                // V1.Position = Corners[3];
                // V2.Position = Corners[7];
                // V3.Position = Corners[6];
                // Chunk->Quads[Chunk->QuadCount++] = (world_quad){ V0,V1,V2,V3 };
            }
        }
    }

    Chunk->Quads = realloc(Chunk->Quads, sizeof(world_quad) * Chunk->QuadCount);
}

void Chunk_SortQuadsBubble(const camera Camera, chunk *Chunk)
{
    vec3 ChunkOffset = { (f32)Chunk->x * CHUNK_WIDTH, 0, (f32)Chunk->z * CHUNK_WIDTH };

    if (Chunk->QuadCount <= 1) return;

    world_quad *Quads = Chunk->Quads;
    i32 QuadCount = (i32)Chunk->QuadCount;

    for (u32 i = 0; i < Chunk->QuadCount; i++)
    {
        vertex *Vertices = Quads[i].Vertices;
        vec3 Center = Vec3_Add(Vec3_Add(Vertices[0].Position, Vertices[1].Position),
                               Vec3_Add(Vertices[2].Position, Vertices[3].Position));
        Center = Vec3_Mul(Center, (vec3){ 0.25f, 0.25f, 0.25f });
        Center = Vec3_Add(Center, ChunkOffset);
        Quads[i].Distance = Camera_CalcDist(Camera, Center);
    }

    for (u32 j = 0; j < Chunk->QuadCount - 1; j++)
    {
        for (u32 i = 0; i < Chunk->QuadCount - 1; i++)
        {
            if (Quads[i].Distance < Quads[i+1].Distance)
            {
                world_quad TempQuad = Quads[i];
                Quads[i] = Quads[i+1];
                Quads[i+1] = TempQuad;
            }
        }
    }
}

void Chunk_SortQuadsInsertion(const camera Camera, chunk *Chunk)
{
    vec3 ChunkOffset = { (f32)Chunk->x * CHUNK_WIDTH, 0, (f32)Chunk->z * CHUNK_WIDTH };

    world_quad *Quads = Chunk->Quads;
    i32 QuadCount = (i32)Chunk->QuadCount;

    if (QuadCount <= 1) return;

    for (i32 i = 0; i < QuadCount; i++)
    {
        vec3 Center = Vec3_Add(Vec3_Add(Quads[i].Vertices[0].Position, Quads[i].Vertices[1].Position),
                               Vec3_Add(Quads[i].Vertices[2].Position, Quads[i].Vertices[3].Position));
        Center = Vec3_Mul(Center, (vec3){ 0.25f, 0.25f, 0.25f });
        Center = Vec3_Add(Center, ChunkOffset);
        Quads[i].Distance = Camera_CalcDist(Camera, Center);
    }
#if 0
    for (i32 i = QuadCount-2; i >= 0; i--)
    {
        world_quad Value = Quads[i];
        i32 j = i;
        while ((j < QuadCount-1) && (Quads[j + 1].Distance > Value.Distance))
        {
            Quads[j] = Quads[j + 1];
            j = j + 1;
        }
        Quads[j] = Value;

    }
#else
    i32 End = QuadCount - 1;
    i32 Cur = 0;
    while (Cur < End)
    {
        i32 B = Cur++;
        i32 A = Cur;
        while (Quads[A].Distance > Quads[B].Distance)
        {
            world_quad T = Quads[B];
            Quads[B] = Quads[A];
            Quads[A] = T;
            if (B == 0) break;
            A = B--;
        }
    }
#endif
}

void Draw_ChunkQuads(const camera Camera, const bitmap Target, bitmap TerrainTexture, const chunk *Chunk)
{
    vec3 ChunkOffset = { (f32)Chunk->x * CHUNK_WIDTH, 0, (f32)Chunk->z * CHUNK_WIDTH };

    for (u32 i = 0; i < Chunk->QuadCount; i++) 
    {
        vertex V0 = Chunk->Quads[i].Vertices[0];
        vertex V1 = Chunk->Quads[i].Vertices[1];
        vertex V2 = Chunk->Quads[i].Vertices[2];
        vertex V3 = Chunk->Quads[i].Vertices[3];

        V0.Position = Camera_WorldToScreen(Camera, Target, Vec3_Add(V0.Position, ChunkOffset));
        V1.Position = Camera_WorldToScreen(Camera, Target, Vec3_Add(V1.Position, ChunkOffset));
        V2.Position = Camera_WorldToScreen(Camera, Target, Vec3_Add(V2.Position, ChunkOffset));
        V3.Position = Camera_WorldToScreen(Camera, Target, Vec3_Add(V3.Position, ChunkOffset));

        Draw_Quad(Target, TerrainTexture, V0, V1, V2, V3);
    }
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
            (zz == 5 && xx == 5 && yy < 20))
        {
            Current_Block->Id = BLOCK_ID_GRAS;
        }
        else
        {
            Current_Block->Id = BLOCK_ID_AIR;
        }
    }

    Chunk_GatherQuads(Chunk);

    // TODO: creation code
}

void Chunk_Delete(chunk *Chunk, i32 x, i32 z)
{
    Chunk->Allocated = false;
    assert(Chunk->x == x);
    assert(Chunk->z == z);

    free(Chunk->Quads);

    // TODO: deletion code
}
