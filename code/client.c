
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
} input;

typedef struct player
{
    // physics
    vec3 Position;
    vec3 Velocity;
    vec3 Acceleration;

    // input
    vec3 MoveDir;
    bool Jump;
    bool Crouch;

    // state
    bool OnGround;
    f32 Cooldown;
    f32 Yaw, Pitch;
} player;

typedef struct client
{
    /* NETWORK */
    network_client Client;

    /* RESOURCES */
    palette Palette;
    palette PaletteWater;
    bitmap Terrain;
    bitmap Font;

    /* STATE */
    camera Camera;
    player Player;
    view View;

    /* DEBUG */
    f32 Fps;
    bool NoClip;
    bool Hitboxes;
} client;

#include "player.c"

void Client_Init(client *Client, const char *Ip)
{
    /* NETWORK */
    while (!Network_ClientInit(&Client->Client, Ip, "4510"));

    /* RESOURCES */
    Client->Terrain = Win32_LoadBitmap("TERRAIN");
    Client->Font = Win32_LoadBitmap("FONT");
    Client->Palette = Win32_LoadPalette("PALETTE");
    Client->PaletteWater = Win32_LoadPalette("PALETTE_WATER");
    Win32_SetPalette(&Client->Palette);

    /* STATE */
    Camera_SetPosition(&Client->Camera, (vec3){ 0 });
    Camera_SetRotation(&Client->Camera, 0.0f, 0.0f);

    View_Init(&Client->View);

    Client->Player = (player){
        .Position = (vec3) { 3.0f, 10.0f, 70.0f },
    };

    /* DEBUG */
    Client->Fps = 0;
    Client->NoClip = true;
    Client->Hitboxes = false;
}

void Client_ProcessMessages(client *Client)
{
    msg Message;
    while (Network_ClientGetMessage(&Client->Client, &Message))
    {
        switch (Message.Header.Type)
        {
            case MSG_DISCONNECT: GlobalRunning = false; return;
            case MSG_PLAYER_STATE: break;

            case MSG_VIEW_POSITION: View_SetPosition(&Client->View, Message.ViewPosition.Position); break;
            case MSG_CHUNK_DATA:    View_SetChunk(&Client->View, &Message.ChunkData); break;
            case MSG_SET_BLOCK:     View_SetBlock(&Client->View, Message.SetBlock.Position, Message.SetBlock.Block); break;
            case MSG_SET_ENTITY:    View_SetEntity(&Client->View, &Message.SetEntity); break;
            default: break;
        }
    }
}

void Client_Input(client *Client, const input Input, f32 DeltaTime)
{
    Client_ProcessMessages(Client);

    Player_Input(&Client->Player, Client, Input, DeltaTime);

    f32 Fps = (1.0f / DeltaTime);
    Client->Fps = Lerp(Client->Fps, Fps, DeltaTime);
}

void Client_Update(client *Client, const input Input, f32 DeltaTime)
{
    Player_Update(&Client->Player, Client, DeltaTime);
}

void Client_Draw(client *Client, bitmap Target, f32 DeltaTime)
{
    block CameraBlock = View_GetBlock(&Client->View, Vec3_FloorToIVec3(Client->Camera.Position));
    if (CameraBlock.Id == BLOCK_ID_AIR)
    {
        Rasterizer_Clear(COLOR_SKYBLUE);
        Win32_SetPalette(&Client->Palette);
    }
    else if (CameraBlock.Id == BLOCK_ID_WATER)
    {
        Rasterizer_Clear(COLOR_BLUE);
        Win32_SetPalette(&Client->PaletteWater);
    }
    else
    {
        Rasterizer_Clear(COLOR_BLACK);
    }
    Player_Draw(&Client->Player, Client, &Client->Camera, DeltaTime);
    View_Draw(&Client->View, Target, Client->Terrain, Client->Camera);
    Rasterizer_Rasterize();
    Rasterizer_Blit(Target);    

    if (Client->Hitboxes)
        View_DrawEntityBoxes(&Client->View, Target, Client->Camera);

    // Draw_String(Target, Client->Font, COLOR_WHITE, (ivec2){8,8}, "Ver: 0.001a");
    ivec2 Position = Draw_String(Target, Client->Font, COLOR_WHITE, (ivec2){8,8}, "Fps: ");
    Draw_Number(Target, Client->Font, COLOR_WHITE, Position, Client->Fps);

    View_DrawLineBox(Target, Client->Camera, (box){{0},{CHUNK_WIDTH,CHUNK_WIDTH,CHUNK_HEIGHT}});

    trace_result TraceResult;
    if (View_TraceRay(&Client->View, Client->Camera.Position, Camera_Direction(Client->Camera), 5.0f, &TraceResult) < 5.0f)
    {
        Block_HighlightFace(Target, Client->Camera, TraceResult.BlockPosition, TraceResult.BlockFace);
    }

    ivec2 Center = (ivec2) { Target.Width / 2, Target.Height / 2 };
    i32 Gap = 4;
    i32 Thickness = 4;
    i32 Length = 8;
    Draw_RectIVec2(Target, COLOR_WHITE, iVec2_Add(Center, (ivec2){-(Gap + Length),-(Thickness / 2) }), (ivec2){ Length, Thickness });
    Draw_RectIVec2(Target, COLOR_WHITE, iVec2_Add(Center, (ivec2){  Gap,          -(Thickness / 2) }), (ivec2){ Length, Thickness });
    Draw_RectIVec2(Target, COLOR_WHITE, iVec2_Add(Center, (ivec2){-(Thickness / 2),-(Gap + Length) }), (ivec2){ Thickness, Length });
    Draw_RectIVec2(Target, COLOR_WHITE, iVec2_Add(Center, (ivec2){-(Thickness / 2),  Gap,          }), (ivec2){ Thickness, Length });
}
