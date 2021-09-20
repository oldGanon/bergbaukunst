
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

box Player_Box(player *Player)
{
    return (box){
        .Min = Vec3_Sub(Player->Position, (vec3){ 0.25f, 0.25f, 1.85f }),
        .Max = Vec3_Add(Player->Position, (vec3){ 0.25f, 0.25f, 0.15f }),
    };
}

vec3 Player_CheckMove(player *Player, world *World, vec3 Move)
{
    return World_CheckMoveBox(World, Player_Box(Player), Move);
}

void Player_Move(player *Player, world *World, vec3 Move)
{
    vec3 CheckMove = Player_CheckMove(Player, World, Move);
    if (Abs(CheckMove.x) < Abs(Move.x)) Player->Velocity.x = 0;
    if (Abs(CheckMove.y) < Abs(Move.y)) Player->Velocity.y = 0;
    if (Abs(CheckMove.z) < Abs(Move.z)) Player->Velocity.z = 0;
    if (CheckMove.z > Move.z) Player->OnGround = true;
    Player->Position = Vec3_Add(Player->Position, CheckMove);
}

void Player_Input(player *Player, world *World, input Input, f32 DeltaTime)
{
#define KEY_SENSITIVITY 0.05f
#define MOUSE_SENSITIVITY (1.0f / 3500.0f)

#define GRAVITY         25.0f
#define JUMP_SPEED       8.0f
#define NO_CLIP_SPEED   10.0f
#define GROUND_SPEED    10.0f
#define AIR_SPEED        2.5f
#define ACCELERATION    10.0f
#define GROUND_FRICTION 10.0f
#define AIR_FRICTION     0.0f

    // Look Input
    if (Input.LookUp)    Player->Pitch += KEY_SENSITIVITY;
    if (Input.LookDown)  Player->Pitch -= KEY_SENSITIVITY;
    if (Input.LookRight) Player->Yaw   += KEY_SENSITIVITY;    
    if (Input.LookLeft)  Player->Yaw   -= KEY_SENSITIVITY;
    Player->Yaw   += Input.Look.x * MOUSE_SENSITIVITY;
    Player->Pitch += Input.Look.y * MOUSE_SENSITIVITY;
    Player->Yaw = Modulo(Player->Yaw, 2*MATH_PI);
    Player->Pitch = Clamp(Player->Pitch, -0.5f*MATH_PI, 0.5f*MATH_PI);

    // Move Direction
    vec3 Forward = Player_Forward(Player);
    vec3 Right = Player_Right(Player);
    Player->MoveDir = Vec3_Zero();
    if (Input.MoveForward)
        Player->MoveDir = Vec3_Add(Player->MoveDir, Forward);
    if (Input.MoveBack)
        Player->MoveDir = Vec3_Sub(Player->MoveDir, Forward);
    if (Input.MoveRight)
        Player->MoveDir = Vec3_Add(Player->MoveDir, Right);
    if (Input.MoveLeft)
        Player->MoveDir = Vec3_Sub(Player->MoveDir, Right);
    Player->MoveDir = Vec3_Normalize(Player->MoveDir);
    Player->Jump = Input.Jump;
    Player->Crouch = Input.Crouch;

    Player->Cooldown = Max(0.0f, Player->Cooldown - DeltaTime);

    f32 Reach = 5.0f;

    if (Input.Punch && (Player->Cooldown == 0))
    {
        trace_result TraceResult;
        if (World_TraceRay(World, Player->Position, Player_ViewDirection(Player), Reach, &TraceResult) < Reach)
        {
            World_SetBlock(World, TraceResult.BlockPosition, (block) { 0 });
        }
        Player->Cooldown = 0.125f;
    }

    if (Input.Use && (Player->Cooldown == 0))
    {
        trace_result TraceResult;
        if (World_TraceRay(World, Player->Position, Player_ViewDirection(Player), Reach, &TraceResult) < Reach)
        {
            block Block = (block){ .Id = BLOCK_ID_GRAS };
            vec3 PlacePosition = TraceResult.FreePosition;
            box PlayerBox = Player_Box(Player);

            if (!Block_BoxIntersect(Block, PlacePosition, PlayerBox))
            {
                World_SetBlock(World, PlacePosition, Block);
            }

        }
        Player->Cooldown = 0.125f;
    }
}

void Player_Update(player *Player, world *World, f32 DeltaTime)
{
    if (Player->NoClip)
    {
        Player->Acceleration = Vec3_Zero();
        Player->Velocity = Vec3_Zero();
        vec3 Move = Vec3_Mul(Player->MoveDir, Vec3_Set1(NO_CLIP_SPEED));
        if (Player->Jump) Move.z += NO_CLIP_SPEED;
        if (Player->Crouch) Move.z -= NO_CLIP_SPEED;
        Player->Position = Vec3_Add(Player->Position, Vec3_Mul(Move, Vec3_Set1(DeltaTime)));
    }
    else
    {
        // Acceleration
        Player->Acceleration = Vec3_Zero();
        f32 WishSpeed = (Player->OnGround) ? GROUND_SPEED : AIR_SPEED;
        f32 CurrentSpeed = Vec3_Dot(Player->Velocity, Player->MoveDir);
        f32 AddSpeed = WishSpeed - CurrentSpeed;
        if (AddSpeed > 0)
        {
            AddSpeed *= ACCELERATION;
            Player->Acceleration = Vec3_Mul(Player->MoveDir, Vec3_Set1(AddSpeed));
        }

        // Gravity
        Player->Acceleration.z -= GRAVITY;

        if (Player->OnGround)
        {
            // Friction
            vec3 Friction = Vec3_Mul(Player->Velocity, Vec3_Set1(-GROUND_FRICTION));
            Player->Acceleration = Vec3_Add(Player->Acceleration, Friction);
            
            // Jump
            if (Player->Jump && Player->OnGround)
            {
                Player->Jump = false;
                if (Player->Velocity.z < JUMP_SPEED)
                    Player->Velocity.z = JUMP_SPEED;
            }
            Player->OnGround = false;
        }

        // Move
        vec3 AddVelocity = Vec3_Mul(Player->Acceleration, Vec3_Set1(DeltaTime));
        Player->Velocity = Vec3_Add(Player->Velocity, AddVelocity);
        vec3 AddPosition = Vec3_Mul(Player->Velocity, Vec3_Set1(DeltaTime));
        Player_Move(Player, World, AddPosition);
    }
}

void Player_Draw(player *Player, world *World, camera *Camera, f32 DeltaTime)
{
    vec3 AddVelocity = Vec3_Mul(Player->Acceleration, Vec3_Set1(DeltaTime));
    vec3 Velocity = Vec3_Add(Player->Velocity, AddVelocity);
    vec3 AddPosition = Vec3_Mul(Velocity, Vec3_Set1(DeltaTime));
    AddPosition = Player_CheckMove(Player, World, AddPosition);
    vec3 Position = Vec3_Add(Player->Position, AddPosition);

    Camera_SetPosition(Camera, Position);
    Camera_SetRotation(Camera, Player->Yaw, Player->Pitch);
}
