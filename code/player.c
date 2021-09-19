
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

void Player_Move(player *Player, world *World, vec3 Move)
{
    {
        box PlayerBox = (box){
            .Min = Vec3_Sub(Player->Position, (vec3){ 0.25f, 0.25f, 1.85f }),
            .Max = Vec3_Add(Player->Position, (vec3){ 0.25f, 0.25f, 0.15f }),
        };
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
        box PlayerBox = (box){
            .Min = Vec3_Sub(Player->Position, (vec3){ 0.25f, 0.25f, 1.85f }),
            .Max = Vec3_Add(Player->Position, (vec3){ 0.25f, 0.25f, 0.15f }),
        };
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
        box PlayerBox = (box){
            .Min = Vec3_Sub(Player->Position, (vec3){ 0.25f, 0.25f, 1.85f }),
            .Max = Vec3_Add(Player->Position, (vec3){ 0.25f, 0.25f, 0.15f }),
        };
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

    if (Player->NoClip)
    {
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

        f32 Dist = 10 * DeltaTime;

        Player->Position = Vec3_Add(Player->Position, Vec3_Mul(WishDir, Vec3_Set1(Dist)));
        if (Input.Jump) Player->Position.z += Dist;
        if (Input.Crouch) Player->Position.z -= Dist;
    }
    else
    {
        // Move Input
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
                if (Player->Velocity.z < JUMP_SPEED)
                    Player->Velocity.z = JUMP_SPEED;
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
        Player->OnGround = false;
        Player->Velocity.z -= GRAVITY * DeltaTime;

        // Horizontal Move
        Player_Move(Player, World, Vec3_Mul(Player->Velocity, Vec3_Set1(DeltaTime)));
    }

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

            box PlayerBox = {
            .Min = Vec3_Sub(Player->Position, (vec3) { 0.25f, 0.25f, 1.85f }),
            .Max = Vec3_Add(Player->Position, (vec3) { 0.25f, 0.25f, 0.15f }),
            };

            box BlockBox  = {
                .Min = { PlacePosition.x, PlacePosition.y, PlacePosition.z + 0.01 },
                .Max = { PlacePosition.x + 1, PlacePosition.y + 1, PlacePosition.z + 1 - 0.01 },
            };

            if (!Box_Intersect(PlayerBox, BlockBox))
            {
                World_SetBlock(World, PlacePosition, (block) { .Id = BLOCK_ID_GRAS });
            }

        }
    }
}
