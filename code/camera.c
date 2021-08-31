
typedef struct camera
{
    vec3 Position;
    f32 Yaw, Pitch;
    f32 SinYaw, CosYaw;
    f32 SinPitch, CosPitch;
} camera;

void Camera_SetPosition(camera *Camera, vec3 Position)
{
    Camera->Position = Position;
}

void Camera_SetRotation(camera *Camera, f32 Yaw, f32 Pitch)
{
    Camera->Yaw = Modulo(Yaw, 2*MATH_PI);
    Camera->Pitch = Clamp(Pitch, -0.5f*MATH_PI, 0.5f*MATH_PI);
    Camera->SinYaw = Sin(Camera->Yaw);
    Camera->CosYaw = Cos(Camera->Yaw);
    Camera->SinPitch = Sin(Camera->Pitch);
    Camera->CosPitch = Cos(Camera->Pitch);
}

vec3 Camera_Direction(const camera Camera)
{
    return (vec3) {
        .x = Camera.SinYaw * Camera.CosPitch,
        .y = Camera.SinPitch,
        .z = Camera.CosPitch * Camera.CosYaw,
    };
}

vec3 Camera_Forward(const camera Camera)
{
    return (vec3) {
        .x = Camera.SinYaw,
        .z = Camera.CosYaw,
    };
}

vec3 Camera_Right(const camera Camera)
{
    return (vec3) {
        .x = Camera.CosYaw,
        .z = -Camera.SinYaw,
    };
}

vec3 Camera_ToScreen(const bitmap Screen, vec3 Position)
{
    f32 HalfWidth = Screen.Width * 0.5f;
    f32 HalfHeight = Screen.Height * 0.5f;
    f32 MinDim = Min(HalfWidth, HalfHeight);
    Position.x = Position.x * MinDim + HalfWidth;
    Position.y = Position.y * MinDim + HalfHeight;

    return Position;
}

vec3 Camera_FromScreen(const bitmap Screen, vec3 Position)
{
    f32 HalfWidth = Screen.Width * 0.5f;
    f32 HalfHeight = Screen.Height * 0.5f;
    f32 MinDim = Min(HalfWidth, HalfHeight);
    Position.x = Position.x - HalfWidth / MinDim;
    Position.y = Position.y - HalfHeight / MinDim;

    return Position;
}

vec3 Camera_FromWorldDir(const camera Camera, vec3 Direction)
{
    {
        f32 Newx = Camera.CosYaw * Direction.x - Camera.SinYaw * Direction.z;
        f32 Newz = Camera.CosYaw * Direction.z + Camera.SinYaw * Direction.x;
        Direction.x = Newx;
        Direction.z = Newz;
    }
    {
        f32 Newy = Camera.CosPitch * Direction.y - Camera.SinPitch * Direction.z;
        f32 Newz = Camera.CosPitch * Direction.z + Camera.SinPitch * Direction.y;
        Direction.y = Newy;
        Direction.z = Newz;
    }

    return Direction;
}

vec3 Camera_ToWorldDir(const camera Camera, vec3 Direction)
{
    {
        f32 Newy = Camera.CosPitch * Direction.y + Camera.SinPitch * Direction.z;
        f32 Newz = Camera.CosPitch * Direction.z - Camera.SinPitch * Direction.y;
        Direction.y = Newy;
        Direction.z = Newz;
    }
    {
        f32 Newx = Camera.CosYaw * Direction.x + Camera.SinYaw * Direction.z;
        f32 Newz = Camera.CosYaw * Direction.z - Camera.SinYaw * Direction.x;
        Direction.x = Newx;
        Direction.z = Newz;
    }

    return Direction;
}

vec3 Camera_FromWorld(const camera Camera, vec3 Position)
{
    Position = Vec3_Sub(Position, Camera.Position);
    Position = Camera_FromWorldDir(Camera, Position);
    return Position;
}

vec3 Camera_ToWorld(const camera Camera, vec3 Position)
{
    Position = Camera_ToWorldDir(Camera, Position);
    Position = Vec3_Add(Position, Camera.Position);
    return Position;
}

vec3 Camera_WorldToScreen(const camera Camera, const bitmap Screen, vec3 Position)
{
    return Camera_ToScreen(Screen, Camera_FromWorld(Camera, Position));
}

vec3 Camera_ScreenToWorld(const camera Camera, const bitmap Screen, vec3 Position)
{
    return Camera_ToWorld(Camera, Camera_FromScreen(Screen, Position));
}

f32 Camera_CalcZ(const camera Camera, vec3 Position)
{
    Position = Vec3_Sub(Position, Camera.Position);
    Position.z = Camera.CosYaw * Position.z + Camera.SinYaw * Position.x;
    Position.z = Camera.CosPitch * Position.z + Camera.SinPitch * Position.y;
    return Position.z;
}

f32 Camera_CalcDist(const camera Camera, vec3 Position)
{
    return Vec3_LengthSq(Vec3_Sub(Position, Camera.Position));
}

bool Camera_PointVisible(const camera Camera, const bitmap Screen, vec3 Position)
{
    Position = Camera_FromWorld(Camera, Position);
    f32 MinDim = 1.0f / Min(Screen.Width, Screen.Height);
    f32 X = Screen.Width * MinDim;
    f32 Y = Screen.Height * MinDim;

    vec3 A = {-X,-Y, 1 };
    vec3 B = { X,-Y, 1 };
    vec3 C = { X, Y, 1 };
    vec3 D = {-X, Y, 1 };

    if (Vec3_Dot(Position, Vec3_Cross(A, B)) < 0) return false;
    if (Vec3_Dot(Position, Vec3_Cross(B, C)) < 0) return false;
    if (Vec3_Dot(Position, Vec3_Cross(C, D)) < 0) return false;
    if (Vec3_Dot(Position, Vec3_Cross(D, A)) < 0) return false;
    return true;
}

bool Camera_BoxVisible(const camera Camera, const bitmap Screen, vec3 P0, vec3 P1)
{
    vec3 Min = Vec3_Min(P0, P1);
    vec3 Max = Vec3_Max(P0, P1);

    vec3 Half = (vec3){ 0.5f, 0.5f, 0.5f };
    vec3 HalfDim = Vec3_Mul(Vec3_Sub(Max, Min), Half);
    vec3 Center = Vec3_Sub(Vec3_Add(Min, HalfDim), Camera.Position);

    f32 MinDim = 1.0f / Min(Screen.Width, Screen.Height);
    f32 X = Screen.Width * MinDim;
    f32 Y = Screen.Height * MinDim;

    vec3 A = {-X,-Y, 1 };
    vec3 B = { X,-Y, 1 };
    vec3 C = { X, Y, 1 };
    vec3 D = {-X, Y, 1 };

    vec3 AB = Camera_ToWorldDir(Camera, Vec3_Cross(A, B));
    vec3 BC = Camera_ToWorldDir(Camera, Vec3_Cross(B, C));
    vec3 CD = Camera_ToWorldDir(Camera, Vec3_Cross(C, D));
    vec3 DA = Camera_ToWorldDir(Camera, Vec3_Cross(D, A));

    if (Vec3_Dot(AB, Vec3_Add(Vec3_Mul(Vec3_Sign(AB), HalfDim), Center)) < 0) return false;
    if (Vec3_Dot(BC, Vec3_Add(Vec3_Mul(Vec3_Sign(BC), HalfDim), Center)) < 0) return false;
    if (Vec3_Dot(CD, Vec3_Add(Vec3_Mul(Vec3_Sign(CD), HalfDim), Center)) < 0) return false;
    if (Vec3_Dot(DA, Vec3_Add(Vec3_Mul(Vec3_Sign(DA), HalfDim), Center)) < 0) return false;
    return true;
}
