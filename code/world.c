
typedef struct Block
{
    vec3 Position;
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
