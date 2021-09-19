
typedef struct player
{
    vec3 Position;
    vec3 Velocity;
    vec3 Acceleration;
    bool OnGround;
    bool Jump;
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

void Player_Move(player *Player, world *World, vec3 Move)
{
    {
        box PlayerBox = Player_Box(Player);
        box MoveBox = PlayerBox;
        if (Move.x < 0) { MoveBox.Max.x = MoveBox.Min.x; MoveBox.Min.x += Move.x;}
        if (Move.x > 0) { MoveBox.Min.x = MoveBox.Max.x; MoveBox.Max.x += Move.x;}
        box CollisionBox = World_BoxIntersection(World, MoveBox);
        if (!Box_Empty(CollisionBox))
        {
            if (Move.x < 0) Move.x = CollisionBox.Max.x - PlayerBox.Min.x;
            if (Move.x > 0) Move.x = CollisionBox.Min.x - PlayerBox.Max.x;
            Player->Velocity.x = 0;
        }
        Player->Position.x += Move.x;
    }

    {
        box PlayerBox = Player_Box(Player);
        box MoveBox = PlayerBox;
        if (Move.y < 0) { MoveBox.Max.y = MoveBox.Min.y; MoveBox.Min.y += Move.y; }
        if (Move.y > 0) { MoveBox.Min.y = MoveBox.Max.y; MoveBox.Max.y += Move.y; }
        box CollisionBox = World_BoxIntersection(World, MoveBox);
        if (!Box_Empty(CollisionBox))
        {
            if (Move.y < 0) Move.y = CollisionBox.Max.y - PlayerBox.Min.y;
            if (Move.y > 0) Move.y = CollisionBox.Min.y - PlayerBox.Max.y;
            Player->Velocity.y = 0;
        }
        Player->Position.y += Move.y;
    }

    {
        box PlayerBox = Player_Box(Player);
        box MoveBox = PlayerBox;
        if (Move.z < 0) { MoveBox.Max.z = MoveBox.Min.z; MoveBox.Min.z += Move.z; }
        if (Move.z > 0) { MoveBox.Min.z = MoveBox.Max.z; MoveBox.Max.z += Move.z; }
        box CollisionBox = World_BoxIntersection(World, MoveBox);
        if (!Box_Empty(CollisionBox))
        {
            if (Move.z < 0)
            {
                Player->OnGround = true;
                Move.z = CollisionBox.Max.z - PlayerBox.Min.z;
            }
            if (Move.z > 0) Move.z = CollisionBox.Min.z - PlayerBox.Max.z;
            Player->Velocity.z = 0;
        }
        Player->Position.z += Move.z;
    }
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
    vec3 WishDir = { 0 };
    if (Input.MoveForward)
        WishDir = Vec3_Add(WishDir, Forward);
    if (Input.MoveBack)
        WishDir = Vec3_Sub(WishDir, Forward);
    if (Input.MoveRight)
        WishDir = Vec3_Add(WishDir, Right);
    if (Input.MoveLeft)
        WishDir = Vec3_Sub(WishDir, Right);
    WishDir = Vec3_Normalize(WishDir);

    if (Player->NoClip)
    {
        Player->Acceleration = Vec3_Zero();
        Player->Velocity = Vec3_Mul(WishDir, Vec3_Set1(NO_CLIP_SPEED));
        if (Input.Jump) Player->Velocity.z += NO_CLIP_SPEED;
        if (Input.Crouch) Player->Velocity.z -= NO_CLIP_SPEED;
    }
    else
    {
        // Accelerate
        Player->Acceleration = Vec3_Zero();
        f32 WishSpeed = (Player->OnGround) ? GROUND_SPEED : AIR_SPEED;
        f32 CurrentSpeed = Vec3_Dot(Player->Velocity, WishDir);
        f32 AddSpeed = WishSpeed - CurrentSpeed;
        if (AddSpeed > 0)
        {
            AddSpeed *= ACCELERATION;
            Player->Acceleration = Vec3_Add(Player->Acceleration, Vec3_Mul(WishDir, Vec3_Set1(AddSpeed)));
        }

        // Friction
        f32 Friction  = (Player->OnGround) ? GROUND_FRICTION : AIR_FRICTION;
        Player->Acceleration = Vec3_Add(Player->Acceleration, Vec3_Mul(Player->Velocity, Vec3_Set1(-Friction)));
        
        // Gravity
        Player->Acceleration.z -= GRAVITY;

        // Jump
        Player->Jump = Input.Jump;
    }

    Player->Cooldown = Max(0.0f, Player->Cooldown - DeltaTime);

    if (Input.Punch && (Player->Cooldown == 0))
    {
        trace_result TraceResult;
        if (World_TraceRay(World, Player->Position, Player_ViewDirection(Player), 5.0f, &TraceResult) < 5.0f)
        {
            World_SetBlock(World, TraceResult.BlockPosition, (block) { 0 });
        }
        Player->Cooldown = 0.125f;
    }

    if (Input.Place && (Player->Cooldown == 0))
    {
        trace_result TraceResult;
        if (World_TraceRay(World, Player->Position, Player_ViewDirection(Player), 5.0f, &TraceResult) < 5.0f)
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
        Player->Position = Vec3_Add(Player->Position, Vec3_Mul(Player->Velocity, Vec3_Set1(DeltaTime)));
    }
    else
    {
        // Jump
        if (Player->Jump && Player->OnGround)
        {
            Player->Jump = false;
            if (Player->Velocity.z < JUMP_SPEED)
                Player->Velocity.z = JUMP_SPEED;
        }
        Player->OnGround = false;

        // Move
        vec3 AddVelocity = Vec3_Mul(Player->Acceleration, Vec3_Set1(DeltaTime));
        Player->Velocity = Vec3_Add(Player->Velocity, AddVelocity);
        vec3 AddPosition = Vec3_Mul(Player->Velocity, Vec3_Set1(DeltaTime));
        Player_Move(Player, World, AddPosition);
    }
}

void Player_Draw(player *Player, world *World, camera *Camera, f32 DeltaTime)
{
    vec3 Position = Vec3_Add(Player->Position, Vec3_Mul(Player->Velocity, Vec3_Set1(DeltaTime)));

    Camera_SetPosition(Camera, Position);
    Camera_SetRotation(Camera, Player->Yaw, Player->Pitch);
}
