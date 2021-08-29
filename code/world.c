
enum
{
    BLOCK_ID_AIR = 0,
    BLOCK_ID_GRAS = 1,
} world_block_id;

#define WORLD_CHUNK_WIDTH 16
#define WORLD_CHUNK_HEIGHT 256
#define CHUNKS_AROUND_PLAYER 2
#define REGION_SIZE ((CHUNKS_AROUND_PLAYER*2)+1)

typedef struct world_quad
{
    vertex Vertices[4];
    f32 Distance;
}world_quad;

typedef struct world_block
{
    u8 Id;
} world_block;

typedef struct world_chunk
{
    world_block Blocks[WORLD_CHUNK_WIDTH][WORLD_CHUNK_WIDTH][WORLD_CHUNK_HEIGHT];
    f32 Distance;
    world_quad* ChunkQuads;
    i32 QuadCount;

} world_chunk;

typedef struct world_region
{
    struct world_chunk *Chunks[REGION_SIZE][REGION_SIZE];
    i32 OffsetX;
    i32 OffsetZ;
} world_region;

typedef struct entity
{
    f32 x, y, z;
} entity;

typedef struct world
{
    entity Entities[256];
    world_region Region;
} world;


world_chunk* Region_ChunkGetAtXZ(world_region* Region, i32 x, i32 z)
{
    i32 OffsetX = (((Region->OffsetX / WORLD_CHUNK_WIDTH) + x) % REGION_SIZE + REGION_SIZE) % REGION_SIZE;
    i32 OffsetZ = (((Region->OffsetZ / WORLD_CHUNK_WIDTH) + z) % REGION_SIZE + REGION_SIZE) % REGION_SIZE;
    return Region->Chunks[OffsetX][OffsetZ];
}

void Region_ChunkSetAtXZ(world_region* Region, i32 x, i32 z, world_chunk* Chunk)
{
    i32 OffsetX = (((Region->OffsetX / WORLD_CHUNK_WIDTH) + x) % REGION_SIZE + REGION_SIZE) % REGION_SIZE;
    i32 OffsetZ = (((Region->OffsetZ / WORLD_CHUNK_WIDTH) + z) % REGION_SIZE + REGION_SIZE) % REGION_SIZE;
    Region->Chunks[OffsetX][OffsetZ] = Chunk;
}

i32 Region_ChunkGetXOffset(world_region* Region, i32 x, i32 z)
{
    i32 OffsetX = x * 16 - ((REGION_SIZE / 2) * 16) + Region->OffsetX-8;
    return OffsetX;
}

i32 Region_ChunkGetZOffset(world_region* Region, i32 x, i32 z)
{
    i32 OffsetZ = z * 16 - ((REGION_SIZE / 2) * 16) + Region->OffsetZ-8;
    return OffsetZ;
}

void Chunk_GatherQuads(world_chunk *Chunk, i32 ChunkOffsetX, i32 ChunkOffsetZ)
{
    i32 i = 0;
    world_quad* ChunkQuads = malloc(sizeof(world_quad) * WORLD_CHUNK_WIDTH * WORLD_CHUNK_WIDTH * WORLD_CHUNK_HEIGHT * 3);
    
    for (i32 x = 0; x < WORLD_CHUNK_WIDTH; x++)
    {
        for (i32 z = 0; z < WORLD_CHUNK_WIDTH; z++)
        {
            for (i32 y = 0; y < WORLD_CHUNK_HEIGHT; y++)
            {
                world_block* CurrentBlock = &Chunk->Blocks[x][z][y];

                if (CurrentBlock->Id != BLOCK_ID_AIR)
                {
                    vec3 BlockCenter = (vec3){ (f32)x, (f32)y, (f32)z };

                    vec3 Corners[8] = {
                        {.x = -0.5f + BlockCenter.x+ChunkOffsetX, .y = -0.5f + BlockCenter.y, .z = -0.5f + BlockCenter.z+ChunkOffsetZ },
                        {.x = +0.5f + BlockCenter.x+ChunkOffsetX, .y = -0.5f + BlockCenter.y, .z = -0.5f + BlockCenter.z+ChunkOffsetZ },
                        {.x = -0.5f + BlockCenter.x+ChunkOffsetX, .y = +0.5f + BlockCenter.y, .z = -0.5f + BlockCenter.z+ChunkOffsetZ },
                        {.x = +0.5f + BlockCenter.x+ChunkOffsetX, .y = +0.5f + BlockCenter.y, .z = -0.5f + BlockCenter.z+ChunkOffsetZ },
                        {.x = -0.5f + BlockCenter.x+ChunkOffsetX, .y = -0.5f + BlockCenter.y, .z = +0.5f + BlockCenter.z+ChunkOffsetZ },
                        {.x = +0.5f + BlockCenter.x+ChunkOffsetX, .y = -0.5f + BlockCenter.y, .z = +0.5f + BlockCenter.z+ChunkOffsetZ },
                        {.x = -0.5f + BlockCenter.x+ChunkOffsetX, .y = +0.5f + BlockCenter.y, .z = +0.5f + BlockCenter.z+ChunkOffsetZ },
                        {.x = +0.5f + BlockCenter.x+ChunkOffsetX, .y = +0.5f + BlockCenter.y, .z = +0.5f + BlockCenter.z+ChunkOffsetZ },
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
                        ChunkQuads[i] = (world_quad){V0,V1,V2,V3};
                        i++;
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
                        ChunkQuads[i] = (world_quad){ V0,V1,V2,V3 };
                        i++;
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
                        ChunkQuads[i] = (world_quad){ V0,V1,V2,V3 };
                        i++;
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
                        ChunkQuads[i] = (world_quad){ V0,V1,V2,V3 };                  
                        i++;
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
                        ChunkQuads[i] = (world_quad){ V0,V1,V2,V3 };
                        i++;
                    }

                    // Top
                    if ((y == WORLD_CHUNK_HEIGHT - 1) || (Chunk->Blocks[x][z][y + 1].Id == BLOCK_ID_AIR))
                    {
                        vertex V0 = (vertex){ .TexCoord.u =  0, .TexCoord.v =  0 };
                        vertex V1 = (vertex){ .TexCoord.u = 16, .TexCoord.v =  0 };
                        vertex V2 = (vertex){ .TexCoord.u = 16, .TexCoord.v = 16 };
                        vertex V3 = (vertex){ .TexCoord.u =  0, .TexCoord.v = 16 };
                        V0.Position = Corners[2];
                        V1.Position = Corners[3];
                        V2.Position = Corners[7];
                        V3.Position = Corners[6];
                        ChunkQuads[i] = (world_quad){ V0,V1,V2,V3 };
                        i++;
                    }
                }
            }
        }
    }
    if (Chunk->ChunkQuads)
        free(Chunk->ChunkQuads);

    
    ChunkQuads = realloc(ChunkQuads, sizeof(world_quad) * i);

    Chunk->QuadCount = i;
    Chunk->ChunkQuads = ChunkQuads;
}


void Draw_QuadsChunk(const camera Camera,const bitmap Target, bitmap TerrainTexture, const world_chunk *Chunk)
{
    for (i32 i = 0; i < Chunk->QuadCount; i++) 
    {
        vertex V0 = Chunk->ChunkQuads[i].Vertices[0];
        vertex V1 = Chunk->ChunkQuads[i].Vertices[1];
        vertex V2 = Chunk->ChunkQuads[i].Vertices[2];
        vertex V3 = Chunk->ChunkQuads[i].Vertices[3];

        V0.Position = Camera_WorldToScreen(Camera, Target, V0.Position);
        V1.Position = Camera_WorldToScreen(Camera, Target, V1.Position);
        V2.Position = Camera_WorldToScreen(Camera, Target, V2.Position);
        V3.Position = Camera_WorldToScreen(Camera, Target, V3.Position);

        Draw_Quad(Target, TerrainTexture, V0, V1, V2, V3);
    }
}


void Draw_GrasBlock(const camera Camera, const bitmap Target, 
                    const bitmap Top, const bitmap Side, const bitmap Bottom, const vec3 BlockCenter)
{
    vec3 Corners[8] = {
        { .x = -0.5f + BlockCenter.x, .y = -0.5f + BlockCenter.y, .z = -0.5f + BlockCenter.z },
        { .x = +0.5f + BlockCenter.x, .y = -0.5f + BlockCenter.y, .z = -0.5f + BlockCenter.z },
        { .x = -0.5f + BlockCenter.x, .y = +0.5f + BlockCenter.y, .z = -0.5f + BlockCenter.z },
        { .x = +0.5f + BlockCenter.x, .y = +0.5f + BlockCenter.y, .z = -0.5f + BlockCenter.z },
        { .x = -0.5f + BlockCenter.x, .y = -0.5f + BlockCenter.y, .z = +0.5f + BlockCenter.z },
        { .x = +0.5f + BlockCenter.x, .y = -0.5f + BlockCenter.y, .z = +0.5f + BlockCenter.z },
        { .x = -0.5f + BlockCenter.x, .y = +0.5f + BlockCenter.y, .z = +0.5f + BlockCenter.z },
        { .x = +0.5f + BlockCenter.x, .y = +0.5f + BlockCenter.y, .z = +0.5f + BlockCenter.z },
    };

    for (u32 i = 0; i < 8; ++i)
        Corners[i] = Camera_WorldToScreen(Camera, Target, Corners[i]);


    vertex V0 = { .TexCoord.u =  0.0f, .TexCoord.v =  0.0f };
    vertex V1 = { .TexCoord.u = 16.0f, .TexCoord.v =  0.0f };
    vertex V2 = { .TexCoord.u = 16.0f, .TexCoord.v = 16.0f };
    vertex V3 = { .TexCoord.u =  0.0f, .TexCoord.v = 16.0f };
    
    V0.Position = Corners[2];
    V1.Position = Corners[3];
    V2.Position = Corners[7];
    V3.Position = Corners[6];
    Draw_Quad(Target, Top, V0, V1, V2, V3);

    V0.Position = Corners[0];
    V1.Position = Corners[1];
    V2.Position = Corners[3];
    V3.Position = Corners[2];
    Draw_Quad(Target, Side, V0, V1, V2, V3);

    V0.Position = Corners[1];
    V1.Position = Corners[5];
    V2.Position = Corners[7];
    V3.Position = Corners[3];
    Draw_Quad(Target, Side, V0, V1, V2, V3);

    V0.Position = Corners[5];
    V1.Position = Corners[4];
    V2.Position = Corners[6];
    V3.Position = Corners[7];
    Draw_Quad(Target, Side, V0, V1, V2, V3);

    V0.Position = Corners[4];
    V1.Position = Corners[0];
    V2.Position = Corners[2];
    V3.Position = Corners[6];
    Draw_Quad(Target, Side, V0, V1, V2, V3);

    V0.Position = Corners[4];
    V1.Position = Corners[5];
    V2.Position = Corners[1];
    V3.Position = Corners[0];
    Draw_Quad(Target, Bottom, V0, V1, V2, V3);
}

/*void DistanceSquaredQuad(const camera Camera, world_quad* Quads, const i32 QuadCount, f32* Distance)
{
    for (i32 i = 0; i < QuadCount; i++)
    {
        vec3 QuadMiddlePoint = { (Quads[i].Vertices[2].Position.x - 0.5f),
                                 (Quads[i].Vertices[2].Position.y - 0.5f),
                                 (Quads[i].Vertices[2].Position.z - 0.5f)};

        vec3 PlayerToQuad = { QuadMiddlePoint.x - Camera.Position.x,
                              QuadMiddlePoint.y - Camera.Position.y,
                              QuadMiddlePoint.z - Camera.Position.z };

        Distance[i] = (f32)(PlayerToQuad.x * PlayerToQuad.x +
                            PlayerToQuad.y * PlayerToQuad.y +
                            PlayerToQuad.z * PlayerToQuad.z);
    }
}*/

void Chunk_SortQuadsBubble(const camera Camera, world_quad *Quads, const i32 QuadCount)
{
    if (QuadCount == 0) return;

    for (i32 i = 0; i < QuadCount - 1; i++)
    {
        Quads[i].Distance = Camera_CalcZ(Camera, (vec3) {(Quads[i].Vertices[2].Position.x - 0.5f),
                                                         (Quads[i].Vertices[2].Position.y - 0.5f),
                                                         (Quads[i].Vertices[2].Position.z - 0.5f)});
    }

    for (i32 j = 0; j < QuadCount - 1; j++)
    {
        for (i32 i = 0; i < QuadCount - 1; i++)
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

void Chunk_SortQuadsInsertion(const camera Camera, world_quad *Quads, const i32 QuadCount)
{
    if (QuadCount == 0) return;

    for (i32 i = 0; i < QuadCount-1; i++)
    {
        Quads[i].Distance = Camera_CalcZ(Camera, (vec3) {(Quads[i].Vertices[2].Position.x - 0.5f),
                                                         (Quads[i].Vertices[2].Position.y - 0.5f),
                                                         (Quads[i].Vertices[2].Position.z - 0.5f)});
    }

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
}

void Chunk_Initionalize(world_chunk *Chunk,world_region *Region,camera Camera, i32 ChunkOffsetX, i32 ChunkOffsetZ)
{
    Chunk = malloc(sizeof(world_chunk));
    Region_ChunkSetAtXZ(Region, ChunkOffsetX, ChunkOffsetZ, Chunk);

    for (i32 x = 0; x < WORLD_CHUNK_WIDTH; x++)
    {
        for (i32 z = 0; z < WORLD_CHUNK_WIDTH; z++)
        {
            for (i32 y = 0; y < WORLD_CHUNK_HEIGHT; y++)
            {
                world_block* Current_Block = &Chunk->Blocks[x][z][y];

                if (y == 0 || (y == 1 && x==2 && z==1) || (z == 5 && x == 5&& y<20) )
                {
                    Current_Block->Id = BLOCK_ID_GRAS;
                }
                else
                {
                    Current_Block->Id = BLOCK_ID_AIR;
                }
            }
        }
    }
    Chunk_GatherQuads(Chunk,Region_ChunkGetXOffset(Region, ChunkOffsetX, ChunkOffsetZ), Region_ChunkGetZOffset(Region, ChunkOffsetX, ChunkOffsetZ));
    Chunk_SortQuadsBubble(Camera, &Chunk->ChunkQuads[0], Chunk->QuadCount);
}

void World_Update_Region_Offset(world_region *Region,const camera Camera) 
{
    if(Camera.Position.x - Region->OffsetX < -8)
    {
        Region->OffsetX -= WORLD_CHUNK_WIDTH;
        for(i32 z = 0; z < REGION_SIZE; z++)
        {
            world_chunk *OldChunk = Region_ChunkGetAtXZ(Region, 0 , z);
            free(OldChunk->ChunkQuads);
            free(OldChunk);
            world_chunk* NewChunk = { 0 };
            Chunk_Initionalize(NewChunk, Region, Camera, 0, z);
        }
    }
    else if (Camera.Position.x - Region->OffsetX > 8)
    {
        Region->OffsetX += WORLD_CHUNK_WIDTH;
        for (i32 z = 0; z < REGION_SIZE; z++)
        {
            world_chunk* OldChunk = Region_ChunkGetAtXZ(Region, REGION_SIZE-1, z);
            free(OldChunk->ChunkQuads);
            free(OldChunk);
            world_chunk* NewChunk = { 0 };
            Chunk_Initionalize(NewChunk, Region, Camera, REGION_SIZE - 1, z);
        }
    }
    else if (Camera.Position.z - Region->OffsetZ < -8)
    {
        Region->OffsetZ -= WORLD_CHUNK_WIDTH;
        for (i32 x = 0; x < REGION_SIZE; x++)
        {
            world_chunk* OldChunk = Region_ChunkGetAtXZ(Region, x, 0);
            free(OldChunk->ChunkQuads);
            free(OldChunk);
            world_chunk* NewChunk = { 0 };
            Chunk_Initionalize(NewChunk, Region, Camera, x, 0);
        }
    }
    else if (Camera.Position.z - Region->OffsetZ > 8)
    {
        Region->OffsetZ += WORLD_CHUNK_WIDTH;
        for (i32 x = 0; x < REGION_SIZE; x++)
        {
            world_chunk* OldChunk = Region_ChunkGetAtXZ(Region, x, REGION_SIZE - 1);
            free(OldChunk->ChunkQuads);
            free(OldChunk);
            world_chunk* NewChunk = { 0 };
            Chunk_Initionalize(NewChunk, Region, Camera, x, REGION_SIZE - 1);
        }
    }
}
