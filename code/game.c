
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

    bool Punch;
    bool Use;
    bool Jump;
    bool Crouch;
    
    bool NoClip;
} input;

#include "world.c"
#include "player.c"

typedef struct game
{
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

    World_Init(&Game->World);

    Game->Player = (player){
        .Position = (vec3) { 3.0f, 10.0f, 70.0f },
        .NoClip = true,
    };
}

void Game_Input(game *Game, const input Input, f32 DeltaTime)
{
    if (Input.NoClip) Game->Player.NoClip = !Game->Player.NoClip;

    Player_Input(&Game->Player, &Game->World, Input, DeltaTime);
}

void Game_Update(game *Game, const input Input, f32 DeltaTime)
{
    Player_Update(&Game->Player, &Game->World, DeltaTime);
    World_Update(&Game->World, Game->Camera);
}

void Game_Draw(game *Game, bitmap Buffer, f32 DeltaTime)
{
    Player_Draw(&Game->Player, &Game->World, &Game->Camera, DeltaTime);

    Raserizer_Clear(COLOR_SKYBLUE);
    World_Draw(&Game->World, Buffer, Game->Terrain, Game->Camera);
    Raserizer_Blit(Buffer);

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
        Block_HighlightFace(Buffer, Game->Camera, TraceResult.BlockPosition, TraceResult.BlockFace);
    }

    ivec2 Center = (ivec2) { Buffer.Width / 2, Buffer.Height / 2 };
    Draw_RectIVec2(Buffer, COLOR_WHITE, iVec2_Add(Center, (ivec2){-1,-5 }), (ivec2){ 2, 4 });
    Draw_RectIVec2(Buffer, COLOR_WHITE, iVec2_Add(Center, (ivec2){-1, 1 }), (ivec2){ 2, 4 });
    Draw_RectIVec2(Buffer, COLOR_WHITE, iVec2_Add(Center, (ivec2){-5,-1 }), (ivec2){ 4, 2 });
    Draw_RectIVec2(Buffer, COLOR_WHITE, iVec2_Add(Center, (ivec2){ 1,-1 }), (ivec2){ 4, 2 });
}
