
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

typedef struct client
{
    /* RESOURCES */
    palette Palette;
    bitmap Terrain;
    bitmap Font;

    camera Camera;
    player Player;
    world World;
} client;

void Client_Init(client *Client)
{
    Client->Terrain = Win32_LoadBitmap("TERRAIN");
    Client->Font = Win32_LoadBitmap("FONT");
    Client->Palette = Win32_LoadPalette("PALETTE");
    Win32_SetPalette(&Client->Palette);

    Camera_SetPosition(&Client->Camera, (vec3){ 0 });
    Camera_SetRotation(&Client->Camera, 0.0f, 0.0f);

    World_Init(&Client->World);

    Client->Player = (player){
        .Position = (vec3) { 3.0f, 10.0f, 70.0f },
        .NoClip = true,
    };
}

void Client_Input(client *Client, const input Input, f32 DeltaTime)
{
    if (Input.NoClip) Client->Player.NoClip = !Client->Player.NoClip;

    Player_Input(&Client->Player, &Client->World, Input, DeltaTime);
}

void Client_Update(client *Client, const input Input, f32 DeltaTime)
{
    Player_Update(&Client->Player, &Client->World, DeltaTime);
    World_Update(&Client->World, Client->Camera);
}

void Client_Draw(client *Client, bitmap Buffer, f32 DeltaTime)
{
    Player_Draw(&Client->Player, &Client->World, &Client->Camera, DeltaTime);

    Raserizer_Clear(COLOR_SKYBLUE);
    World_Draw(&Client->World, Buffer, Client->Terrain, Client->Camera);
    Raserizer_Blit(Buffer);

    Draw_String(Buffer, Client->Font, COLOR_WHITE, (ivec2){8,8}, "ver. 0.001a");

    vec3 A = Camera_WorldToScreen(Client->Camera, Buffer, (vec3) { 0, 0,1});
    vec3 B = Camera_WorldToScreen(Client->Camera, Buffer, (vec3) {16, 0,1});
    vec3 C = Camera_WorldToScreen(Client->Camera, Buffer, (vec3) {16,16,1});
    vec3 D = Camera_WorldToScreen(Client->Camera, Buffer, (vec3) { 0,16,1});
    Draw_Line(Buffer, COLOR_WHITE, A, B);
    Draw_Line(Buffer, COLOR_WHITE, B, C);
    Draw_Line(Buffer, COLOR_WHITE, C, D);
    Draw_Line(Buffer, COLOR_WHITE, D, A);

    trace_result TraceResult;
    if (World_TraceRay(&Client->World, Client->Camera.Position, Camera_Direction(Client->Camera), 5.0f, &TraceResult) < 5.0f)
    {
        Block_HighlightFace(Buffer, Client->Camera, TraceResult.BlockPosition, TraceResult.BlockFace);
    }

    ivec2 Center = (ivec2) { Buffer.Width / 2, Buffer.Height / 2 };
    Draw_RectIVec2(Buffer, COLOR_WHITE, iVec2_Add(Center, (ivec2){-1,-5 }), (ivec2){ 2, 4 });
    Draw_RectIVec2(Buffer, COLOR_WHITE, iVec2_Add(Center, (ivec2){-1, 1 }), (ivec2){ 2, 4 });
    Draw_RectIVec2(Buffer, COLOR_WHITE, iVec2_Add(Center, (ivec2){-5,-1 }), (ivec2){ 4, 2 });
    Draw_RectIVec2(Buffer, COLOR_WHITE, iVec2_Add(Center, (ivec2){ 1,-1 }), (ivec2){ 4, 2 });
}
