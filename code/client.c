
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

#include "player.c"

typedef struct client
{
    /* NETWORK */
    network_client Client;

    /* RESOURCES */
    palette Palette;
    bitmap Terrain;
    bitmap Font;

    /* STATE */
    camera Camera;
    player Player;
    view View;
} client;

void Client_Init(client *Client)
{
    /* NETWORK */
    while (!Network_Client(&Client->Client, "localhost", "4510"));

    /* RESOURCES */
    Client->Terrain = Win32_LoadBitmap("TERRAIN");
    Client->Font = Win32_LoadBitmap("FONT");
    Client->Palette = Win32_LoadPalette("PALETTE");
    Win32_SetPalette(&Client->Palette);

    /* STATE */
    Camera_SetPosition(&Client->Camera, (vec3){ 0 });
    Camera_SetRotation(&Client->Camera, 0.0f, 0.0f);

    View_Init(&Client->View);

    Client->Player = (player){
        .Position = (vec3) { 3.0f, 10.0f, 70.0f },
        .NoClip = true,
    };
}

void Client_ProcessMessages(client *Client)
{
    msg Message;
    while (Network_ClientGetMessage(&Client->Client, &Message))
    {
        switch (Message.Header.Type)
        {
            case MSG_DISCONNECT: break;
            case MSG_PLACE_BLOCK: break;
            case MSG_BREAK_BLOCK: break;
            case MSG_PLAYER_POSITION: break;
            case MSG_VIEW_POSITION: View_SetPosition(&Client->View, Message.ViewPosition.Position); break;
            case MSG_CHUNK_DATA: View_SetChunk(&Client->View, &Message.ChunkData); break;
        }
    }
}

void Client_Input(client *Client, const input Input, f32 DeltaTime)
{
    Client_ProcessMessages(Client);

    if (Input.NoClip) Client->Player.NoClip = !Client->Player.NoClip;

    Player_Input(&Client->Player, &Client->View, Input, DeltaTime);
}

void Client_Update(client *Client, const input Input, f32 DeltaTime)
{
    Player_Update(&Client->Player, &Client->View, DeltaTime);
}

void Client_Draw(client *Client, bitmap Buffer, f32 DeltaTime)
{
    Player_Draw(&Client->Player, &Client->View, &Client->Camera, DeltaTime);

    Raserizer_Clear(COLOR_SKYBLUE);
    View_Draw(&Client->View, Buffer, Client->Terrain, Client->Camera);
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
    if (View_TraceRay(&Client->View, Client->Camera.Position, Camera_Direction(Client->Camera), 5.0f, &TraceResult) < 5.0f)
    {
        Block_HighlightFace(Buffer, Client->Camera, TraceResult.BlockPosition, TraceResult.BlockFace);
    }

    ivec2 Center = (ivec2) { Buffer.Width / 2, Buffer.Height / 2 };
    Draw_RectIVec2(Buffer, COLOR_WHITE, iVec2_Add(Center, (ivec2){-1,-5 }), (ivec2){ 2, 4 });
    Draw_RectIVec2(Buffer, COLOR_WHITE, iVec2_Add(Center, (ivec2){-1, 1 }), (ivec2){ 2, 4 });
    Draw_RectIVec2(Buffer, COLOR_WHITE, iVec2_Add(Center, (ivec2){-5,-1 }), (ivec2){ 4, 2 });
    Draw_RectIVec2(Buffer, COLOR_WHITE, iVec2_Add(Center, (ivec2){ 1,-1 }), (ivec2){ 4, 2 });
}
