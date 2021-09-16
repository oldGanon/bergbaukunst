
typedef struct player
{
    vec3 Position;
    vec3 Velocity;
    bool OnGround;

    f32 Yaw, Pitch;
    bool NoClip;
} player;

vec3 Player_Forward(player *Player)
{
    return (vec3) {
        .x = Sin(Player->Yaw),
        .y = Cos(Player->Yaw)
    };
}

vec3 Player_Right(player *Player)
{
    return (vec3){
        .x = Cos(Player->Yaw),
        .y = -Sin(Player->Yaw)
    };
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
#define KEY_SENSITIVITY 0.05f
#define MOUSE_SENSITIVITY (1.0f / 3500.0f)

#define GRAVITY 25.0f
#define JUMP_SPEED 8.0f
#define STOP_SPEED 4.0f
#define FRICTION 5.0f
#define GROUND_ACCEL 10.0f
#define AIR_ACCEL 3.0f
#define GROUND_SPEED 7.5f
#define AIR_SPEED 2.0f

    // Look Input
    if (Input.LookUp) {
        Player->Pitch += KEY_SENSITIVITY;
    }
    if (Input.LookDown) {
        Player->Pitch -= KEY_SENSITIVITY;
    }
    if (Input.LookRight) {
        Player->Yaw += KEY_SENSITIVITY;
    }    
    if (Input.LookLeft) {
        Player->Yaw -= KEY_SENSITIVITY;
    }
    Player->Yaw += Input.Look.x * MOUSE_SENSITIVITY;
    Player->Pitch += Input.Look.y * MOUSE_SENSITIVITY;
    Player->Yaw = Modulo(Player->Yaw, 2*MATH_PI);
    Player->Pitch = Clamp(Player->Pitch, -0.5f*MATH_PI, 0.5f*MATH_PI);

#if 0
    // Move Input
    vec3 Forward = Player_Forward(Player);
    vec3 Right = Player_Right(Player);
    vec3 WishDir = { 0 };
    if (Input.MoveForward)
        WishDir = Vec3_Add(WishDir, Forward);
    if (Input.MoveRight)
        WishDir = Vec3_Add(WishDir, Right);
    WishDir = Vec3_Normalize(WishDir);

    // Ground Move
    if (Player->OnGround)
    {
        // Friction
        f32 Speed = Vec3_Length(Player->Velocity);
        if (Speed < 1) Player->Velocity = (vec3){ 0 };
        else
        {
            f32 Control = Max(Speed, STOP_SPEED);
            f32 Friction = FRICTION;
            f32 Drop = Control * Friction * DeltaTime;
            f32 NewSpeed = Speed - Drop;
            if (NewSpeed < 0)
                Player->Velocity = (vec3){ 0 };
            else
                Player->Velocity = Vec3_Mul(Player->Velocity, Vec3_Set1(NewSpeed / Speed));
        }

        // Jump
        if (Input.Jump)
        {
            if (Player->Velocity.y < JUMP_SPEED)
                Player->Velocity.y = JUMP_SPEED;
            Player->OnGround = false;
        }
    }

    // Accelerate
    f32 Accel = (Player->OnGround) ? GROUND_ACCEL : AIR_ACCEL;
    f32 CurrentSpeed = Vec3_Dot(Player->Velocity, WishDir);
    f32 WishSpeed = (Player->OnGround) ? GROUND_SPEED : AIR_SPEED;
    f32 AddSpeed = WishSpeed - CurrentSpeed;
    if (AddSpeed > 0)
    {
        f32 AccelSpeed = Accel * GROUND_SPEED * DeltaTime;
        if (AccelSpeed > AddSpeed)
            AccelSpeed = AddSpeed;
        Player->Velocity = Vec3_Add(Player->Velocity, Vec3_Mul(WishDir, Vec3_Set1(AccelSpeed)));
    }

    // Gravity
    if (!Player->OnGround)
        Player->Velocity.y -= GRAVITY * DeltaTime;

    if ()
#else
    vec3 Forward = Player_Forward(Player);
    vec3 Right = Player_Right(Player);
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
#endif



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
