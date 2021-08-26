
typedef struct Block
{
    vec3 Position;
    i8 Translucent;
}Block;

typedef struct world_chunk
{
    Block Blocks[16][16][256];
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

void Draw_EntireChunk(const camera Camera, const bitmap Target,
                    const bitmap Top, const bitmap Side, const bitmap Bottom,world_chunk* Chunk) 
{
    typedef struct SidesToBeDrawn
    {
        i8 Top;
        i8 Front;
        i8 Right;
        i8 Back;
        i8 Left;
        i8 Bottom;
    }SidesToBeDrawn;

    SidesToBeDrawn Empty = { 0 };
    SidesToBeDrawn Sides = { 0 };

    for (i32 x = 0; x < 16; x++)
    {
        for (i32 z = 0; z < 16; z++)
        {
            for (i32 y = 0; y < 256; y++)
            {
                Sides = Empty;
                Block CurrentBlock = Chunk->Blocks[x][z][y];
                
                if (CurrentBlock.Translucent == 0) {

                    vec3 BlockCenter = CurrentBlock.Position;

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

                    for (u32 i = 0; i < 8; ++i)
                        Corners[i] = CameraToScreen(Target, WorldToCamera(Camera, Corners[i]));

                    vertex V0 = { .TexCoord.u = 0.0f, .TexCoord.v = 0.0f };
                    vertex V1 = { .TexCoord.u = 16.0f, .TexCoord.v = 0.0f };
                    vertex V2 = { .TexCoord.u = 16.0f, .TexCoord.v = 16.0f };
                    vertex V3 = { .TexCoord.u = 0.0f, .TexCoord.v = 16.0f };
                    
                    i32 Block_x = (i32)CurrentBlock.Position.x%16;
                    i32 Block_y = (i32)CurrentBlock.Position.y;
                    i32 Block_z = (i32)CurrentBlock.Position.z%16;


                    Block* Block_Top = &Chunk->Blocks[Block_x][Block_z][Block_y + 1];
                    Block* Block_Front = &Chunk->Blocks[Block_x][Block_z - 1][Block_y];
                    Block* Block_Right = &Chunk->Blocks[Block_x + 1][Block_z][Block_y];
                    Block* Block_Back = &Chunk->Blocks[Block_x][Block_z + 1][Block_y];
                    Block* Block_Left = &Chunk->Blocks[Block_x - 1][Block_z][Block_y];
                    Block* Block_Bottom = &Chunk->Blocks[Block_x][Block_z][Block_y - 1];

                    if (x > 0)
                    {
                        if (Block_Left->Translucent == 1)
                        {
                            Sides.Left = 1;
                        }
                    }
                    else
                    {
                        Sides.Left = 1;
                    }
                    if (x < 15)
                    {
                        if (Block_Right->Translucent == 1) 
                        {
                            Sides.Right = 1;
                        }
                    }
                    else
                    {
                        Sides.Right = 1;
                    }
                    if (z > 0)
                    {
                        if (Block_Front->Translucent == 1)
                        {
                            Sides.Front = 1;
                        }
                    }
                    else
                    {
                        Sides.Front = 1;
                    }
                    if(z <15)
                    {
                        if(Block_Back->Translucent == 1)
                        {
                            Sides.Back = 1;
                        }
                    }
                    else
                    {
                        Sides.Back = 1;
                    }
                    if (y > 0)
                    {
                        if(Block_Bottom->Translucent == 1)
                        {
                            Sides.Bottom = 1;
                        }
                    }
                    else
                    {
                        Sides.Bottom = 1;
                    }
                    if(y < 255)
                    {
                        if (Block_Top->Translucent == 1) 
                        {
                            Sides.Top = 1;
                        }
                    }
                    else
                    {
                        Sides.Top = 1;
                    }

                    if(Sides.Left == 1)
                    {
                        V0.Position = Corners[4];
                        V1.Position = Corners[0];
                        V2.Position = Corners[2];
                        V3.Position = Corners[6];
                        Draw_Quad(Target, Side, V0, V1, V2, V3);
                    }
                    if (Sides.Right == 1) 
                    {
                        V0.Position = Corners[1];
                        V1.Position = Corners[5];
                        V2.Position = Corners[7];
                        V3.Position = Corners[3];
                        Draw_Quad(Target, Side, V0, V1, V2, V3);
                    }
                    if (Sides.Front == 1) 
                    {
                        V0.Position = Corners[0];
                        V1.Position = Corners[1];
                        V2.Position = Corners[3];
                        V3.Position = Corners[2];
                        Draw_Quad(Target, Side, V0, V1, V2, V3);
                    }
                    if (Sides.Back == 1) 
                    {
                        V0.Position = Corners[5];
                        V1.Position = Corners[4];
                        V2.Position = Corners[6];
                        V3.Position = Corners[7];
                        Draw_Quad(Target, Side, V0, V1, V2, V3);
                    }
                    if(Sides.Bottom == 1)
                    {
                        V0.Position = Corners[4];
                        V1.Position = Corners[5];
                        V2.Position = Corners[1];
                        V3.Position = Corners[0];
                        Draw_Quad(Target, Bottom, V0, V1, V2, V3);
                    }
                    if(Sides.Top == 1)
                    {
                        V0.Position = Corners[2];
                        V1.Position = Corners[3];
                        V2.Position = Corners[7];
                        V3.Position = Corners[6];
                        Draw_Quad(Target, Top, V0, V1, V2, V3);
                    }
                    
                }

            }
        }
    }
}

void SortBlockDistances(camera Cammera, vec3 *Offsets, i32 Length) 
{
    f32 *DistancesSquared = malloc(sizeof(f32) * Length);

    for (i32 i = 0; i < Length; i++)
    {
        vec3 PlayerToBlock = {
            Offsets[i].x - Cammera.Position.x,
            Offsets[i].y - Cammera.Position.y,
            Offsets[i].z - Cammera.Position.z
        };
        DistancesSquared[i] = PlayerToBlock.x * PlayerToBlock.x + 
                              PlayerToBlock.y * PlayerToBlock.y +
                              PlayerToBlock.z * PlayerToBlock.z;
    }

    for (i32 j = 0; j <= Length - 1; j++)
    {
        for (i32 i = 0; i <= Length - 1; i++)
        {
            f32 Distance = DistancesSquared[i];
            if (DistancesSquared[i] < DistancesSquared[i+1])
            {
                f32 TempDistance = DistancesSquared[i];
                DistancesSquared[i] = DistancesSquared[i+1];
                DistancesSquared[i+1] = TempDistance;

                vec3 TempOffset = Offsets[i];
                Offsets[i] = Offsets[i+1];
                Offsets[i+1] = TempOffset;
            }
        }
    }

    free(DistancesSquared);
}
