
struct world_chunk
{
    u8 Blocks[16][16][256];
};

struct world_region
{
    struct world_chunk Chunks[16][16];
};

struct entity
{
    f32 x, y, z;
};

struct world
{
    struct entity Entities[256];

    struct world_region Region;
};

void Draw_GrasBlock(const struct camera Camera, const struct bitmap Target, 
                    const struct bitmap Top, const struct bitmap Side, const struct bitmap Bottom)
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

    struct vertex V0 = { .u =  0.0f, .v =  0.0f };
    struct vertex V1 = { .u = 16.0f, .v =  0.0f };
    struct vertex V2 = { .u = 16.0f, .v = 16.0f };
    struct vertex V3 = { .u =  0.0f, .v = 16.0f };

    V0.Position = Corners[2];
    V1.Position = Corners[3];
    V2.Position = Corners[7];
    V3.Position = Corners[6];
    Draw_QuadTextured3D(Target, Top, V0, V1, V2, V3);

    V0.Position = Corners[0];
    V1.Position = Corners[1];
    V2.Position = Corners[3];
    V3.Position = Corners[2];
    Draw_QuadTextured3D(Target, Side, V0, V1, V2, V3);

    V0.Position = Corners[1];
    V1.Position = Corners[5];
    V2.Position = Corners[7];
    V3.Position = Corners[3];
    Draw_QuadTextured3D(Target, Side, V0, V1, V2, V3);

    V0.Position = Corners[5];
    V1.Position = Corners[4];
    V2.Position = Corners[6];
    V3.Position = Corners[7];
    Draw_QuadTextured3D(Target, Side, V0, V1, V2, V3);

    V0.Position = Corners[4];
    V1.Position = Corners[0];
    V2.Position = Corners[2];
    V3.Position = Corners[6];
    Draw_QuadTextured3D(Target, Side, V0, V1, V2, V3);

    V0.Position = Corners[4];
    V1.Position = Corners[5];
    V2.Position = Corners[1];
    V3.Position = Corners[0];
    Draw_QuadTextured3D(Target, Bottom, V0, V1, V2, V3);
}
