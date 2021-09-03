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

    bool NoClip;
    bool Interact;
    bool Jump;
    bool Crouch;
    bool Punch;
    bool Place;
} input;

#include "world.c"
#include "player.c"

typedef struct game
{
    u64 Frame;

    /* RESOURCES */
    palette Palette;
    bitmap Terrain;
    bitmap Font;

    camera Camera;
    player Player;
    world World;
} game;


void Game_Init(game *Game)
{
    Game->Terrain = Win32_LoadBitmap("TERRAIN");
    Game->Font = Win32_LoadBitmap("FONT");
    Game->Palette = Win32_LoadPalette("PALETTE");
    Win32_SetPalette(&Game->Palette);

    Camera_SetPosition(&Game->Camera, (vec3){ 0 });
    Camera_SetRotation(&Game->Camera, 0.0f, 0.0f);

    World_Create(&Game->World);

    Game->Player = (player){
        .Position = (vec3) { 3.0f, 10.0f, 10.0f },
        .NoClip = true,
    };
}

void Game_Update(game *Game, const input Input, f32 DeltaTime)
{
    camera *Camera = &Game->Camera;

    f32 Speed = 0.25f;

    if (Input.NoClip)
    {
        Game->Player.NoClip = !Game->Player.NoClip;
    }

    Player_Update(&Game->Player, Input, &Game->World, DeltaTime);

    Camera_SetPosition(Camera, Game->Player.Position);
    Camera_SetRotation(Camera, Game->Player.Yaw, Game->Player.Pitch);

    World_Update(&Game->World, Game->Camera);
}

void Game_Draw(game *Game, bitmap Buffer)
{
    Bitmap_Clear(Buffer, COLOR_SKYBLUE);

    World_Draw(&Game->World, Buffer, Game->Terrain, Game->Camera);

    Draw_String(Buffer, Game->Font, COLOR_WHITE, (ivec2){8,8}, "ver. 0.001a");

    vec3 A = Camera_WorldToScreen(Game->Camera, Buffer, (vec3) { 0, 0,1});
    vec3 B = Camera_WorldToScreen(Game->Camera, Buffer, (vec3) {16, 0,1});
    vec3 C = Camera_WorldToScreen(Game->Camera, Buffer, (vec3) {16,16,1});
    vec3 D = Camera_WorldToScreen(Game->Camera, Buffer, (vec3) { 0,16,1});
    Draw_Line(Buffer, COLOR_WHITE, A, B);
    Draw_Line(Buffer, COLOR_WHITE, B, C);
    Draw_Line(Buffer, COLOR_WHITE, C, D);
    Draw_Line(Buffer, COLOR_WHITE, D, A);

    trace_result TraceResult;
    if (World_TraceRay(&Game->World, Game->Camera.Position, Camera_Direction(Game->Camera), 5.0f, &TraceResult) < 5.0f)
    {
        Block_Highlight(Buffer, Game->Camera, TraceResult);
    }

    ivec2 Center = (ivec2) { Buffer.Width / 2, Buffer.Height / 2 };
    Draw_RectIVec2(Buffer, COLOR_WHITE, iVec2_Add(Center, (ivec2){-1,-5 }), (ivec2){ 2, 4 });
    Draw_RectIVec2(Buffer, COLOR_WHITE, iVec2_Add(Center, (ivec2){-1, 1 }), (ivec2){ 2, 4 });
    Draw_RectIVec2(Buffer, COLOR_WHITE, iVec2_Add(Center, (ivec2){-5,-1 }), (ivec2){ 4, 2 });
    Draw_RectIVec2(Buffer, COLOR_WHITE, iVec2_Add(Center, (ivec2){ 1,-1 }), (ivec2){ 4, 2 });
    
    // Draw_RectIVec2(Buffer, COLOR_BLACK, (ivec2) { Buffer.Width / 2 - 1, Buffer.Height / 2 - 1}, (ivec2) {1, 1});
    //Draw_RectInt(Buffer, COLOR_BLACK, Buffer.Width/2 - 3, Buffer.Height / 2 - 3, 5, 5 );
}
