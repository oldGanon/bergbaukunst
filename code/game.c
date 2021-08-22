
#include "world.c"

struct game
{
    u64 Frame;

    /* RESOURCES */
    struct bitmap Image;
    struct bitmap Font;

    struct camera Camera;
    struct world World;
};

struct input
{
    u8 MoveUp;
    u8 MoveDown;
    u8 MoveLeft;
    u8 MoveRight;

    u8 LookUp;
    u8 LookDown;
    u8 LookLeft;
    u8 LookRight;
};

void Game_Init(struct game *Game)
{
    Game->Image = Win32_LoadBitmap("IMAGE");
    Game->Font = Win32_LoadBitmap("FONT");

    Camera_SetPosition(&Game->Camera, (vec3) { 0,0,-1.0f });
    Camera_SetRotation(&Game->Camera, 0.0f, -0.78539816339f * 0.75f);
}

void Game_Update(struct game *Game, const struct input Input)
{
    Camera_SetRotation(&Game->Camera, Game->Camera.Yaw + 0.025f, Game->Camera.Pitch);
    vec3 Forward = Camera_Forward(Game->Camera);
    Forward.x = -1.5f * Forward.x;
    Forward.y = -1.5f * Forward.y;
    Forward.z = -1.5f * Forward.z;
    Camera_SetPosition(&Game->Camera, Forward);
}

void Game_Draw(const struct game *Game, struct bitmap Buffer)
{
    Bitmap_Clear(Buffer, COLOR_BLUE);

    struct bitmap GrasTop = Bitmap_Section(Game->Image, 0, 0, 16, 16);
    struct bitmap GrasSide = Bitmap_Section(Game->Image, 16, 0, 16, 16);
    struct bitmap GrasBottom = Bitmap_Section(Game->Image, 32, 0, 16, 16);
    Draw_GrasBlock(Game->Camera, Buffer, GrasTop, GrasSide, GrasBottom);

    Draw_String(Buffer, Game->Font, COLOR_WHITE, 32, 32, "ASFIDJH\nasdasd");
    
/*
    Bitmap_Clear(Buffer, COLOR_HEX(0xC0FFEE));
    Bitmap_SetPixel(Buffer, COLOR_HEX(0xDEAD), 10, 10);
    Bitmap_SetPixel(Buffer, COLOR_HEX(0x134156), 20, 20);
    struct bitmap Small = Bitmap_Section(Game->Image, 0, 0, 16, 16);
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



    struct point P0 = { .x = 250.5f + linex, .y =  50.5f + liney };
    struct point P1 = { .x = 200.5f + linex, .y = 100.5f + liney };
    struct point P2 = { .x = 225.5f + linex, .y = 150.5f + liney };
    struct point P3 = { .x = 210.5f + linex, .y = 175.5f + liney };

    struct triangle Triangle = { .a = P0, .b = P1, .c = P2 };
    Draw_TriangleStruct(Buffer, COLOR_HEX(0x00FFFF), Triangle);

    struct triangle Triangle2 = (struct triangle) { .a = P1, .b = P2, .c = P3 };
    Draw_TriangleStruct(Buffer, COLOR_HEX(0xFF00FF), Triangle2);
    
    // if ((Game->Frame / 60) & 1) return;
    struct line Line = { .a = P1, .b = P2 };
    Draw_Line(Buffer, COLOR_HEX(0x4000FF), Line);
    struct line Line2 = { .a = P2,  .b = P3 };
    Draw_Line(Buffer, COLOR_HEX(0x4000FF), Line2);
    struct line Line3 = { .a = P3, .b = P1 };
    Draw_Line(Buffer, COLOR_HEX(0x4000FF), Line3);

    Draw_Point(Buffer, COLOR_HEX(0x000000), P1);
    Draw_Point(Buffer, COLOR_HEX(0x000000), P2);
    Draw_Point(Buffer, COLOR_HEX(0x000000), P3);



    // struct point P4 = { .x =  0.0f + linex, .y =  0 };
    // struct point P5 = { .x =  0.0f + linex, .y = 10 };
    // struct point P6 = { .x = 10.0f + linex, .y = 10 };
    // struct point P7 = { .x = 10.0f + linex, .y =  0 };

    struct point P4 = { .x =  0.0f + linex, .y =  0 };
    struct point P5 = { .x =  0.0f + linex, .y = 10 };
    struct point P6 = { .x = 10.0f + linex, .y = 10 };
    struct point P7 = { .x = 10.0f + linex, .y =  0 };

    struct triangle Triangle3 = { .a = P4, .b = P5, .c = P7 };
    Draw_TriangleStruct(Buffer, COLOR_HEX(0x000000), Triangle3);
    // if ((Game->Frame / 10) & 1) return;
    struct triangle Triangle4 = { .a = P7, .b = P5, .c = P6 };
    Draw_TriangleStruct(Buffer, COLOR_HEX(0xFFFFFF), Triangle4);

    

    struct point A = { .x = 10.0f + linex, .y = 100 };
    struct point B = { .x = 15.0f + linex, .y = 105 };
    struct point C = { .x = 10.99f + linex, .y = 100 };
    struct point D = { .x = 15.99f + linex, .y = 105 };

    struct line LineAB = { .a = A, .b = B };
    Draw_Line(Buffer, COLOR_HEX(0xFFFFFF), LineAB);
    struct line LineCD = { .a = C, .b = D };
    Draw_Line(Buffer, COLOR_HEX(0xFFFFFF), LineCD);





    struct point P10 = { .x = 100, .y = 100 };
    struct point P11 = { .x = 164, .y = 196 };
    struct point P12 = { .x = 100, .y = 164 };
    struct point P13 = { .x =   0, .y =   0 };
    struct point P14 = { .x =  16, .y =  32 };
    struct point P15 = { .x =   0, .y =  32 };

    struct triangle Triangle6 = { .a = P10, .b = P11, .c = P12 };
    struct triangle Triangle7 = { .a = P13, .b = P14, .c = P15 };
    // Draw_TriangleTextured(Buffer, Game->Font, Triangle6, Triangle7);


    struct vertex V0 = { .x =  10, .y =  10, .z = 1, .u =  1, .v =  1 };
    struct vertex V1 = { .x = 150, .y = 150, .z = 2, .u = 32, .v = 32 };
    struct vertex V2 = { .x =  10, .y = 100, .z = 2, .u =  1, .v = 32 };
    struct vertex V3 = { .x = 150, .y =  60, .z = 1, .u = 32, .v =  1 };

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
