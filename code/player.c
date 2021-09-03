
typedef struct player
{
    vec3 Position;
    vec3 Velocity;
    f32 Yaw, Pitch;
    bool NoClip;
} player;

vec3 Player_Forward(player *Player)
{
    return (vec3) { .x = Sin(Player->Yaw),  .y = Cos(Player->Yaw) };
}

vec3 Player_Right(player *Player)
{
    return (vec3){ .x = Cos(Player->Yaw), .y = -Sin(Player->Yaw) };
}

vec3 Player_ViewDirection(player *Player)
{
    return (vec3) {
        .x = Sin(Player->Yaw) * Cos(Player->Pitch),
        .y = Cos(Player->Pitch) * Cos(Player->Yaw),
        .z = Sin(Player->Pitch),
    };
}

void Player_Update(player *Player, input Input, world *World,  f32 DeltaTime)
{
    vec3 Forward = Player_Forward(Player);
    vec3 Right = Player_Right(Player);

    f32 TurnSpeed = 0.05f;
    f32 Sensitivity = 1.0f / 3500.0f;

    if(!Player->NoClip)
    {
        f32 Speed = 0.05f;
        f32 JumpHeight = 0.20f;

        vec3 Acceleration = { 0 };

        if (Input.MoveForward) {
            Acceleration.x += Forward.x * Speed;
            Acceleration.y += Forward.y * Speed;
            Acceleration.z += Forward.z * Speed;
        }
        if (Input.MoveBack) {
            Acceleration.x -= Forward.x * Speed;
            Acceleration.y -= Forward.y * Speed;
            Acceleration.z -= Forward.z * Speed;
        }
        if (Input.MoveRight) {
            Acceleration.x += Right.x * Speed;
            Acceleration.y += Right.y * Speed;
        }
        if (Input.MoveLeft) {
            Acceleration.x -= Right.x * Speed;
            Acceleration.y -= Right.y * Speed;
        }
        if (Input.Jump && Player->Velocity.z == 0)
        {
            Acceleration.z += JumpHeight;
        }




        Acceleration.z -= 0.01f;

        Acceleration.x = Acceleration.x - Player->Velocity.x * 0.5f;
        Acceleration.y = Acceleration.y - Player->Velocity.y * 0.5f;
        Acceleration.z = Acceleration.z - Player->Velocity.z * 0.01f;

        Player->Velocity = Vec3_Add(Player->Velocity, Acceleration);

        if (World_GetBlock(World, (vec3) { Player->Position.x + Player->Velocity.x, Player->Position.y, Player->Position.z     }).Id != 0 ||
            World_GetBlock(World, (vec3) { Player->Position.x + Player->Velocity.x, Player->Position.y, Player->Position.z - 1 }).Id != 0)
        {
            Player->Velocity.x = 0;
        }
        if (World_GetBlock(World, (vec3) { Player->Position.x, Player->Position.y + Player->Velocity.y, Player->Position.z     }).Id != 0  ||
            World_GetBlock(World, (vec3) { Player->Position.x, Player->Position.y + Player->Velocity.y, Player->Position.z - 1 }).Id != 0)
        {
            Player->Velocity.y = 0;
        }
        if (World_GetBlock(World, (vec3) { Player->Position.x, Player->Position.y, Player->Position.z + Player->Velocity.z     }).Id != 0 ||
            World_GetBlock(World, (vec3) { Player->Position.x, Player->Position.y, Player->Position.z + Player->Velocity.z - 1 }).Id != 0)
        {
            Player->Velocity.z = 0;
        }

        Player->Position = Vec3_Add(Player->Position, Player->Velocity);
    }
    else
    {
        f32 Speed = 0.2f;

        if (Input.MoveForward) {
            Player->Position.x += Forward.x * Speed;
            Player->Position.y += Forward.y * Speed;
            Player->Position.z += Forward.z * Speed;
        }
        if (Input.MoveBack) {
            Player->Position.x -= Forward.x * Speed;
            Player->Position.y -= Forward.y * Speed;
            Player->Position.z -= Forward.z * Speed;
        }
        if (Input.MoveRight) {
            Player->Position.x += Right.x * Speed;
            Player->Position.y += Right.y * Speed;
        }
        if (Input.MoveLeft) {
            Player->Position.x -= Right.x * Speed;
            Player->Position.y -= Right.y * Speed;
        }
        if (Input.Jump)
        {
            Player->Position.z += Speed;
        }
        if (Input.Crouch)
        {
            Player->Position.z -= Speed;
        }
    }



    if (Input.LookUp) {
        Player->Pitch += TurnSpeed;
    }
    if (Input.LookDown) {
        Player->Pitch -= TurnSpeed;
    }
    if (Input.LookRight) {
        Player->Yaw += TurnSpeed;
    }    
    if (Input.LookLeft) {
        Player->Yaw -= TurnSpeed;
    }
    Player->Yaw += Input.Look.x * Sensitivity;
    Player->Pitch += Input.Look.y * Sensitivity;



    if (Input.Punch)
    {
        trace_result TraceResult;
        if (World_TraceRay(World, Player->Position, Player_ViewDirection(Player), 5.0f, &TraceResult) < 5.0f)
        {
            World_SetBlock(World, TraceResult.BlockPosition, (block) { 0 });
        }
    }
    else if (Input.Place)
    {
        trace_result TraceResult;
        if (World_TraceRay(World, Player->Position, Player_ViewDirection(Player), 5.0f, &TraceResult) < 5.0f)
        {
            vec3 PlacePosition = TraceResult.FreePosition;

            vec3 PlayerBlock = Vec3_Floor(Player->Position);
            if (!(PlayerBlock.x == PlacePosition.x &&
                PlayerBlock.y == PlacePosition.y &&
               (PlayerBlock.z == PlacePosition.z ||
                PlayerBlock.z - 1 == PlacePosition.z)))
            {
                World_SetBlock(World, PlacePosition, (block){ .Id = BLOCK_ID_GRAS });
            }
        }
    }
}
