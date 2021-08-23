
typedef struct world_chunk
{
    u8 Blocks[16][16][256];
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
                    const bitmap Top, const bitmap Side, const bitmap Bottom)
{
    vec3 Corners[8] = {
        { .x = -0.5f, .y = -0.5f, .z = -0.5f },
        { .x = +0.5f, .y = -0.5f, .z = -0.5f },
        { .x = -0.5f, .y = +0.5f, .z = -0.5f },
        { .x = +0.5f, .y = +0.5f, .z = -0.5f },
        { .x = -0.5f, .y = -0.5f, .z = +0.5f },
        { .x = +0.5f, .y = -0.5f, .z = +0.5f },
        { .x = -0.5f, .y = +0.5f, .z = +0.5f },
        { .x = +0.5f, .y = +0.5f, .z = +0.5f },
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
