
enum
{
    BLOCK_ID_AIR = 0,
    BLOCK_ID_GRAS = 1,
} world_block_id;

#define WORLD_CHUNK_WIDTH 16
#define WORLD_CHUNK_HEIGHT 256

typedef struct world_quad
{
    vertex Vertices[4];
}world_quad;

typedef struct world_block
{
    u8 Id;
} world_block;

typedef struct world_chunk
{
    world_block Blocks[WORLD_CHUNK_WIDTH][WORLD_CHUNK_WIDTH][WORLD_CHUNK_HEIGHT];
    world_quad* ChunkQuads;
    i32 QuadCount;

} world_chunk;

typedef struct world_region
{
    struct world_chunk Chunks[16][16];
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

void DistanceSquaredQuad(const camera Camera, world_quad *Quads, const i32 QuadCount, f32 *Distance);



void Chunk_GatherQuads(world_chunk *Chunk)
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
                        {.x = -0.5f + BlockCenter.x, .y = -0.5f + BlockCenter.y, .z = -0.5f + BlockCenter.z },
                        {.x = +0.5f + BlockCenter.x, .y = -0.5f + BlockCenter.y, .z = -0.5f + BlockCenter.z },
                        {.x = -0.5f + BlockCenter.x, .y = +0.5f + BlockCenter.y, .z = -0.5f + BlockCenter.z },
                        {.x = +0.5f + BlockCenter.x, .y = +0.5f + BlockCenter.y, .z = -0.5f + BlockCenter.z },
                        {.x = -0.5f + BlockCenter.x, .y = -0.5f + BlockCenter.y, .z = +0.5f + BlockCenter.z },
                        {.x = +0.5f + BlockCenter.x, .y = -0.5f + BlockCenter.y, .z = +0.5f + BlockCenter.z },
                        {.x = -0.5f + BlockCenter.x, .y = +0.5f + BlockCenter.y, .z = +0.5f + BlockCenter.z },
                        {.x = +0.5f + BlockCenter.x, .y = +0.5f + BlockCenter.y, .z = +0.5f + BlockCenter.z },
                    };
                    //Left, Right, Front, Back, Bottom, Top

                    if (x == 0 || (Chunk->Blocks[x - 1][z][y].Id == BLOCK_ID_AIR))
                    {
                        vertex V0 = (vertex){ .TexCoord.u = 0.0f + 16.0f,.TexCoord.v = 0.0f };
                        vertex V1 = (vertex){ .TexCoord.u = 16.0f + 16.0f,.TexCoord.v = 0.0f };
                        vertex V2 = (vertex){ .TexCoord.u = 16.0f + 16.0f,.TexCoord.v = 16.0f };
                        vertex V3 = (vertex){ .TexCoord.u = 0.0f + 16.0f,.TexCoord.v = 16.0f};
                        V0.Position = Corners[4];
                        V1.Position = Corners[0];
                        V2.Position = Corners[2];
                        V3.Position = Corners[6];
                        ChunkQuads[i] = (world_quad){V0,V1,V2,V3};
                        i++;
                    }

                    if ((x == WORLD_CHUNK_WIDTH - 1) || (Chunk->Blocks[x + 1][z][y].Id == BLOCK_ID_AIR))
                    {
                        vertex V0 = (vertex){ .TexCoord.u = 0.0f+16.0f,.TexCoord.v = 0.0f };
                        vertex V1 = (vertex){ .TexCoord.u = 16.0f+ 16.0f,.TexCoord.v = 0.0f };
                        vertex V2 = (vertex){ .TexCoord.u = 16.0f+ 16.0f,.TexCoord.v = 16.0f };
                        vertex V3 = (vertex){ .TexCoord.u = 0.0f+ 16.0f,.TexCoord.v = 16.0f };
                        V0.Position = Corners[1];
                        V1.Position = Corners[5];
                        V2.Position = Corners[7];
                        V3.Position = Corners[3];
                        ChunkQuads[i] = (world_quad){ V0,V1,V2,V3 };
                        i++;
                    }

                    if (z == 0 || (Chunk->Blocks[x][z - 1][y].Id == BLOCK_ID_AIR))
                    {
                        vertex V0 = (vertex){ .TexCoord.u = 0.0f + 16.0f,.TexCoord.v = 0.0f };
                        vertex V1 = (vertex){ .TexCoord.u = 16.0f + 16.0f,.TexCoord.v = 0.0f };
                        vertex V2 = (vertex){ .TexCoord.u = 16.0f + 16.0f,.TexCoord.v = 16.0f };
                        vertex V3 = (vertex){ .TexCoord.u = 0.0f + 16.0f,.TexCoord.v = 16.0f };
                        V0.Position = Corners[0];
                        V1.Position = Corners[1];
                        V2.Position = Corners[3];
                        V3.Position = Corners[2];
                        ChunkQuads[i] = (world_quad){ V0,V1,V2,V3 };
                        i++;
                    }

                    if ((z == WORLD_CHUNK_WIDTH - 1) || (Chunk->Blocks[x][z + 1][y].Id == BLOCK_ID_AIR))
                    {
                        vertex V0 = (vertex){ .TexCoord.u = 0.0f + 16.0f,.TexCoord.v = 0.0f };
                        vertex V1 = (vertex){ .TexCoord.u = 16.0f + 16.0f,.TexCoord.v = 0.0f };
                        vertex V2 = (vertex){ .TexCoord.u = 16.0f + 16.0f,.TexCoord.v = 16.0f };
                        vertex V3 = (vertex){ .TexCoord.u = 0.0f + 16.0f,.TexCoord.v = 16.0f };
                        V0.Position = Corners[5];
                        V1.Position = Corners[4];
                        V2.Position = Corners[6];
                        V3.Position = Corners[7];
                        ChunkQuads[i] = (world_quad){ V0,V1,V2,V3 };                  
                        i++;
                    }

                    if (y == 0 || (Chunk->Blocks[x][z][y - 1].Id == BLOCK_ID_AIR))
                    {
                        vertex V0 = (vertex){ .TexCoord.u = 0.0f+32.0f,.TexCoord.v = 0.0f };
                        vertex V1 = (vertex){ .TexCoord.u = 16.0f + 32.0f,.TexCoord.v = 0.0f };
                        vertex V2 = (vertex){ .TexCoord.u = 16.0f + 32.0f,.TexCoord.v = 16.0f };
                        vertex V3 = (vertex){ .TexCoord.u = 0.0f + 32.0f,.TexCoord.v = 16.0f };
                        V0.Position = Corners[4];
                        V1.Position = Corners[5];
                        V2.Position = Corners[1];
                        V3.Position = Corners[0];
                        ChunkQuads[i] = (world_quad){ V0,V1,V2,V3 };
                        i++;
                    }

                    if ((y == WORLD_CHUNK_HEIGHT - 1) || (Chunk->Blocks[x][z][y + 1].Id == BLOCK_ID_AIR))
                    {
                        vertex V0 = (vertex){ .TexCoord.u = 0.0f,.TexCoord.v = 0.0f };
                        vertex V1 = (vertex){ .TexCoord.u = 16.0f,.TexCoord.v = 0.0f };
                        vertex V2 = (vertex){ .TexCoord.u = 16.0f,.TexCoord.v = 16.0f };
                        vertex V3 = (vertex){ .TexCoord.u = 0.0f,.TexCoord.v = 16.0f };
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

        V0.Position = CameraToScreen(Target, WorldToCamera(Camera, V0.Position));
        V1.Position = CameraToScreen(Target, WorldToCamera(Camera, V1.Position));
        V2.Position = CameraToScreen(Target, WorldToCamera(Camera, V2.Position));
        V3.Position = CameraToScreen(Target, WorldToCamera(Camera, V3.Position));

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
        Corners[i] = CameraToScreen(Target, WorldToCamera(Camera, Corners[i]));


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


void Chunk_SortQuadsBubble(const camera Camera, world_quad *Quads, const i32 QuadCount)
{
    if (QuadCount == 0) return;

    f32 *DistancesSquared = malloc(sizeof(f32) * QuadCount);

    DistanceSquaredQuad(Camera, Quads, QuadCount, DistancesSquared);

    for (i32 j = 0; j < QuadCount - 1; j++)
    {
        for (i32 i = 0; i < QuadCount - 1; i++)
        {
            f32 Distance = DistancesSquared[i];
            if (DistancesSquared[i] < DistancesSquared[i+1])
            {
                f32 TempDistance = DistancesSquared[i];
                DistancesSquared[i] = DistancesSquared[i+1];
                DistancesSquared[i+1] = TempDistance;

                world_quad TempQuad = Quads[i];
                Quads[i] = Quads[i+1];
                Quads[i+1] = TempQuad;
            }
        }
    }
    free(DistancesSquared);
}

void Chunk_SortQuadsInsertion(const camera Camera, world_quad *Quads, const i32 QuadCount)
{
    if (QuadCount == 0) return;
    f32* DistancesSquared = malloc(sizeof(f32) * QuadCount);

    DistanceSquaredQuad(Camera, Quads, QuadCount, DistancesSquared);

    for (i32 i = QuadCount-1; i >= 0; i--)
    {
        f32 Value = DistancesSquared[i];
        world_quad QuadValue = Quads[i];
        i32 j = i;
        while ((j < QuadCount) && (DistancesSquared[j + 1] > Value))
        {
            DistancesSquared[j] = DistancesSquared[j + 1];
            Quads[j] = Quads[j + 1];
            j = j + 1;
        }
        DistancesSquared[j] = Value;
        Quads[j] = QuadValue;

    }
    free(DistancesSquared);
}

void DistanceSquaredQuad(const camera Camera, world_quad *Quads, const i32 QuadCount, f32 *Distance)
{
    for (i32 i = 0; i < QuadCount; i++)
    {
        vec3 QuadMiddlePoint = { (Quads[i].Vertices[2].Position.x - Quads[i].Vertices[0].Position.x) / 2 + Quads[i].Vertices[0].Position.x,
                                 (Quads[i].Vertices[2].Position.y - Quads[i].Vertices[0].Position.y) / 2 + Quads[i].Vertices[0].Position.y,
                                 (Quads[i].Vertices[2].Position.z - Quads[i].Vertices[0].Position.z) / 2 + Quads[i].Vertices[0].Position.z };

        vec3 PlayerToQuad = { QuadMiddlePoint.x - Camera.Position.x,
                              QuadMiddlePoint.y - Camera.Position.y,
                              QuadMiddlePoint.z - Camera.Position.z };

        Distance[i] = (f32)(PlayerToQuad.x * PlayerToQuad.x +
                            PlayerToQuad.y * PlayerToQuad.y +
                            PlayerToQuad.z * PlayerToQuad.z);
    }
}

