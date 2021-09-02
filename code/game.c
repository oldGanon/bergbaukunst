
#include "world.c"

typedef struct game
{
    u64 Frame;

    /* RESOURCES */
    palette Palette;
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
    Game->Terrain = Win32_LoadBitmap("TERRAIN");
    Game->Font = Win32_LoadBitmap("FONT");
    Game->Palette = Win32_LoadPalette("PALETTE");
    Win32_SetPalette(&Game->Palette);

    Camera_SetPosition(&Game->Camera, (vec3) { 3.0f, 2.0f, 2.0f });
    Camera_SetRotation(&Game->Camera, 0.0f, 0.0f);

    World_Create(&Game->World);
}

void Game_Update(game *Game, const input Input)
{
    camera *Camera = &Game->Camera;

    vec3 Forward = Camera_Forward(*Camera);
    vec3 Right = Camera_Right(*Camera);
    vec3 NewCameraPosition = Camera->Position;
    f32 NewYaw = Camera->Yaw;
    f32 NewPitch = Camera->Pitch;

    f32 Speed = 0.25f;
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
    if (Input.Jump)
    {
        NewCameraPosition.y += Speed;
    }
    if (Input.Crouch)
    {
        NewCameraPosition.y -= Speed;
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
        trace_result TraceResult;
        if (World_TraceCameraRay(&Game->World, Game->Camera, 5.0f, &TraceResult))
        {
            World_SetBlock(&Game->World, TraceResult.BlockPosition, (block) { 0 });
        }
    }
    else if (Input.Place)
    {
        trace_result TraceResult;
        if (World_TraceCameraRay(&Game->World, Game->Camera, 5.0f, &TraceResult))
        {
            Block_PlaceOnSide(&Game->World, TraceResult);
        }
    }
    
    Camera_SetPosition(Camera, NewCameraPosition);
    Camera_SetRotation(Camera, NewYaw, NewPitch);

    World_Update(&Game->World, Game->Camera);

}

void Game_Draw(game *Game, bitmap Buffer)
{
    Bitmap_Clear(Buffer, COLOR_SKYBLUE);

    World_Draw(&Game->World, Buffer, Game->Terrain, Game->Camera);

    Draw_String(Buffer, Game->Font, COLOR_WHITE, (ivec2){8,8}, "ver. 0.001a");

    vec3 A = Camera_WorldToScreen(Game->Camera, Buffer, (vec3) {0,1,0});
    vec3 B = Camera_WorldToScreen(Game->Camera, Buffer, (vec3) {16,1,0});
    vec3 C = Camera_WorldToScreen(Game->Camera, Buffer, (vec3) {16,1,16});
    vec3 D = Camera_WorldToScreen(Game->Camera, Buffer, (vec3) {0,1,16});
    Draw_Line(Buffer, COLOR_WHITE, A, B);
    Draw_Line(Buffer, COLOR_WHITE, B, C);
    Draw_Line(Buffer, COLOR_WHITE, C, D);
    Draw_Line(Buffer, COLOR_WHITE, D, A);

    trace_result TraceResult;
    if (World_TraceCameraRay(&Game->World, Game->Camera, 5.0f, &TraceResult))
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
