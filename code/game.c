
#include "world.c"

typedef struct game
{
    u64 Frame;

    /* RESOURCES */
    bitmap Terrain;
    bitmap Font;

    camera Camera;
    world World;
} game;

typedef struct input
{
    vec2 Look;
    ivec2 Mouse;

    bool MoveForward;
    bool MoveBack;
    bool MoveLeft;
    bool MoveRight;

    bool LookUp;
    bool LookDown;
    bool LookLeft;
    bool LookRight;

    bool Interact;
    bool Jump;
    bool Crouch;
    bool Punch;
    bool Place;
} input;

void Game_Init(game *Game)
{
/*
    chunk_map ChunkMap = ChunkMap_Create();

    for (i32 x = -16; x < 16; ++x)
    for (i32 z = -16; z < 16; ++z)
    {
        ChunkMap_AllocateChunk(&ChunkMap, x, z);
    }

    for (i32 x = -16; x < 16; ++x)
    for (i32 z = -16; z < 16; ++z)
    {
        chunk *Chunk = ChunkMap_GetChunk(&ChunkMap, x, z);
        assert(Chunk->x == x);
        assert(Chunk->z == z);
    }

    for (i32 x = -16; x < 16; ++x)
    for (i32 z = -16; z < 16; ++z)
    {
        ChunkMap_DeleteChunk(&ChunkMap, x, z);
    }

    ChunkMap_Delete(&ChunkMap);
*/

    Game->Terrain = Win32_LoadBitmap("TERRAIN");
    Game->Font = Win32_LoadBitmap("FONT");

    Camera_SetPosition(&Game->Camera, (vec3) { 3.0f, 2.0f, 2.0f });
    //Camera_SetRotation(&Game->Camera, 0.0f, -0.78539816339f * 0.75f);
    Camera_SetRotation(&Game->Camera, 0.0f, 0.0f);


    World_Create(&Game->World);
}

void Game_Update(game *Game, const input Input)
{
    camera* Camera = &Game->Camera;

    vec3 Forward = Camera_Direction(*Camera);
     
    vec3 Right = Camera_Right(*Camera);
    vec3 NewCameraPosition = Camera->Position;
    f32 NewYaw = Camera->Yaw;
    f32 NewPitch = Camera->Pitch;

    f32 Speed = 0.5f;
    f32 TurnSpeed = 0.05f;
    f32 Sensitivity = 1.0f / 3500.0f;

    if (Input.MoveForward) {
        NewCameraPosition.x += Forward.x * Speed;
        NewCameraPosition.y += Forward.y * Speed;
        NewCameraPosition.z += Forward.z * Speed;
    }
    if (Input.MoveBack) {
        NewCameraPosition.x -= Forward.x * Speed;
        NewCameraPosition.y -= Forward.y * Speed;
        NewCameraPosition.z -= Forward.z * Speed;
    }
    if (Input.MoveRight) {
        NewCameraPosition.x += Right.x * Speed;
        NewCameraPosition.z += Right.z * Speed;
    }
    if (Input.MoveLeft) {
        NewCameraPosition.x -= Right.x * Speed;
        NewCameraPosition.z -= Right.z * Speed;
    }
    if (Input.LookUp) {
        NewPitch += TurnSpeed;
    }
    if (Input.LookDown) {
        NewPitch -= TurnSpeed;
    }
    if (Input.LookRight) {
        NewYaw += TurnSpeed;
    }
    
    if (Input.LookLeft) {
        NewYaw -= TurnSpeed;
    }

    NewYaw += Input.Look.x * Sensitivity;
    NewPitch += Input.Look.y * Sensitivity;
    
    if (Input.Punch)
    {
        Block_PlayerLookingAt(&Game->World, Game->Camera);
    }
    
    Camera_SetPosition(Camera, NewCameraPosition);
    Camera_SetRotation(Camera, NewYaw, NewPitch);

    World_Update(&Game->World, Game->Camera);

}

void Game_Draw(game *Game, bitmap Buffer)
{
    Bitmap_Clear(Buffer, COLOR_SKYBLUE);

    World_Draw(&Game->World, Buffer, Game->Terrain, Game->Camera);

    Draw_String(Buffer, Game->Font, COLOR_WHITE, (ivec2){32,32}, "ASFIDJH\nasdasd");

    vec3 A = Camera_WorldToScreen(Game->Camera, Buffer, (vec3) {0,1,0});
    vec3 B = Camera_WorldToScreen(Game->Camera, Buffer, (vec3) {16,1,0});
    vec3 C = Camera_WorldToScreen(Game->Camera, Buffer, (vec3) {16,1,16});
    vec3 D = Camera_WorldToScreen(Game->Camera, Buffer, (vec3) {0,1,16});
    Draw_Line(Buffer, COLOR_WHITE, A, B);
    Draw_Line(Buffer, COLOR_WHITE, B, C);
    Draw_Line(Buffer, COLOR_WHITE, C, D);
    Draw_Line(Buffer, COLOR_WHITE, D, A);

    Draw_RectIVec2(Buffer, COLOR_BLACK, (ivec2) { Buffer.Width / 2 - 1, Buffer.Height / 2 - 1}, (ivec2) {1, 1});
    //Draw_RectInt(Buffer, COLOR_BLACK, Buffer.Width/2 - 3, Buffer.Height / 2 - 3, 5, 5 );

    /*
    //bitmap GrasTop = Bitmap_Section(Game->Image, 0, 0, 16, 16);
    //bitmap GrasSide = Bitmap_Section(Game->Image, 16, 0, 16, 16);
    //bitmap GrasBottom = Bitmap_Section(Game->Image, 32, 0, 16, 16);

    
    vec3 Offsets[256] = { 0 };
    for (i32 x = 0; x < 16; x++)
    {
        for (i32 z = 0; z < 16; z++)
        {
            Offsets[16 * x + z] = Game->World.Region.Chunks[0][0].Blocks[x][z][0].Position;
        }
    }

    SortBlockDistances(Game->Camera, Offsets, 256);

    for (i32 i = 0; i < 256; i++)
    {
        Draw_GrasBlock(Game->Camera, Buffer, GrasTop, GrasSide, GrasBottom, Offsets[i]);
    }

    //Draw_GrasBlock(Game->Camera, Buffer, GrasTop, GrasSide, GrasBottom, (vec3) { 1, 1, 1 });

    //Draw_EntireChunk(Game->Camera, Buffer, GrasTop, GrasSide, GrasBottom, &Game->World.Region.Chunks[0][0]);
    
    */
/*
    Bitmap_Clear(Buffer, COLOR_HEX(0xC0FFEE));
    Bitmap_SetPixel(Buffer, COLOR_HEX(0xDEAD), 10, 10);
    Bitmap_SetPixel(Buffer, COLOR_HEX(0x134156), 20, 20);
    bitmap Small = Bitmap_Section(Game->Image, 0, 0, 16, 16);
    Draw_Bitmap(Buffer, Small, 20, (Game->Frame % 200 - 20));
    Draw_String(Buffer, Game->Font, COLOR_HEX(0xFF00AA), 80, 80, "ASFIDJH\nasdasd");
    Draw_Rect(Buffer, COLOR_HEX(0x0000FF), (Game->Frame % 200 - 20), (Game->Frame % 200 - 20), 16, 16);

    // Draw_Line(Buffer, 0 + linex, liney, 100 + linex, 100 + liney, COLOR_HEX(0x0000FF));
    // Draw_Line(Buffer, 1 + linex, liney, 101 + linex, 100 + liney, COLOR_HEX(0xFF00FF));
    // Draw_Line(Buffer, 2 + linex, liney, 102 + linex, 100 + liney, COLOR_HEX(0x00FFFF));
    // Draw_Line(Buffer, 3 + linex, liney, 103 + linex, 100 + liney, COLOR_HEX(0xFFFFFF));
    // Draw_Line(Buffer, 4 + linex, liney, 104 + linex, 100 + liney, COLOR_HEX(0x0000FF));

    Draw_Line(Buffer, COLOR_HEX(0x0000FF), 100, 100, 120, 110);
    Draw_Line(Buffer, COLOR_HEX(0x0000FF), 100, 100, 120,  90);
    Draw_Line(Buffer, COLOR_HEX(0x0000FF), 100, 100,  80,  90);
    Draw_Line(Buffer, COLOR_HEX(0x0000FF), 100, 100,  80, 110);
    Draw_Line(Buffer, COLOR_HEX(0x0000FF), 100, 100, 110, 120);
    Draw_Line(Buffer, COLOR_HEX(0x0000FF), 100, 100,  90, 120);
    Draw_Line(Buffer, COLOR_HEX(0x0000FF), 100, 100,  90,  80);
    Draw_Line(Buffer, COLOR_HEX(0x0000FF), 100, 100, 110,  80);



    point P0 = { .x = 250.5f + linex, .y =  50.5f + liney };
    point P1 = { .x = 200.5f + linex, .y = 100.5f + liney };
    point P2 = { .x = 225.5f + linex, .y = 150.5f + liney };
    point P3 = { .x = 210.5f + linex, .y = 175.5f + liney };

    triangle Triangle = { .a = P0, .b = P1, .c = P2 };
    Draw_TriangleStruct(Buffer, COLOR_HEX(0x00FFFF), Triangle);

    triangle Triangle2 = (triangle) { .a = P1, .b = P2, .c = P3 };
    Draw_TriangleStruct(Buffer, COLOR_HEX(0xFF00FF), Triangle2);
    
    // if ((Game->Frame / 60) & 1) return;
    line Line = { .a = P1, .b = P2 };
    Draw_Line(Buffer, COLOR_HEX(0x4000FF), Line);
    line Line2 = { .a = P2,  .b = P3 };
    Draw_Line(Buffer, COLOR_HEX(0x4000FF), Line2);
    line Line3 = { .a = P3, .b = P1 };
    Draw_Line(Buffer, COLOR_HEX(0x4000FF), Line3);

    Draw_Point(Buffer, COLOR_HEX(0x000000), P1);
    Draw_Point(Buffer, COLOR_HEX(0x000000), P2);
    Draw_Point(Buffer, COLOR_HEX(0x000000), P3);



    // point P4 = { .x =  0.0f + linex, .y =  0 };
    // point P5 = { .x =  0.0f + linex, .y = 10 };
    // point P6 = { .x = 10.0f + linex, .y = 10 };
    // point P7 = { .x = 10.0f + linex, .y =  0 };

    point P4 = { .x =  0.0f + linex, .y =  0 };
    point P5 = { .x =  0.0f + linex, .y = 10 };
    point P6 = { .x = 10.0f + linex, .y = 10 };
    point P7 = { .x = 10.0f + linex, .y =  0 };

    triangle Triangle3 = { .a = P4, .b = P5, .c = P7 };
    Draw_TriangleStruct(Buffer, COLOR_HEX(0x000000), Triangle3);
    // if ((Game->Frame / 10) & 1) return;
    triangle Triangle4 = { .a = P7, .b = P5, .c = P6 };
    Draw_TriangleStruct(Buffer, COLOR_HEX(0xFFFFFF), Triangle4);

    

    point A = { .x = 10.0f + linex, .y = 100 };
    point B = { .x = 15.0f + linex, .y = 105 };
    point C = { .x = 10.99f + linex, .y = 100 };
    point D = { .x = 15.99f + linex, .y = 105 };

    line LineAB = { .a = A, .b = B };
    Draw_Line(Buffer, COLOR_HEX(0xFFFFFF), LineAB);
    line LineCD = { .a = C, .b = D };
    Draw_Line(Buffer, COLOR_HEX(0xFFFFFF), LineCD);





    point P10 = { .x = 100, .y = 100 };
    point P11 = { .x = 164, .y = 196 };
    point P12 = { .x = 100, .y = 164 };
    point P13 = { .x =   0, .y =   0 };
    point P14 = { .x =  16, .y =  32 };
    point P15 = { .x =   0, .y =  32 };

    triangle Triangle6 = { .a = P10, .b = P11, .c = P12 };
    triangle Triangle7 = { .a = P13, .b = P14, .c = P15 };
    // Draw_TriangleTextured(Buffer, Game->Font, Triangle6, Triangle7);


    vertex V0 = { .x =  10, .y =  10, .z = 1, .u =  1, .v =  1 };
    vertex V1 = { .x = 150, .y = 150, .z = 2, .u = 32, .v = 32 };
    vertex V2 = { .x =  10, .y = 100, .z = 2, .u =  1, .v = 32 };
    vertex V3 = { .x = 150, .y =  60, .z = 1, .u = 32, .v =  1 };

#if 1
    Draw_TriangleTextured3D(Buffer, Game->Image, V0, V1, V2);
    Draw_TriangleTextured3D(Buffer, Game->Image, V0, V1, V3);
#else
    // Draw_QuadTexturedPerspectiveCorrect(Buffer, Game->Image, V0, V3, V1, V2);
    Draw_T(Buffer, Game->Image, V0, V2, V0, V3);
    Draw_T(Buffer, Game->Image, V0, V2, V3, V1);
    Draw_T(Buffer, Game->Image, V2, V1, V3, V1);
#endif
*/
}
