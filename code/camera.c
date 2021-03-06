
typedef struct camera
{
    vec3 Position;
    f32 Yaw, Pitch;
    f32 SinYaw, CosYaw;
    f32 SinPitch, CosPitch;
} camera;

void Camera_SetPosition(camera *Camera, vec3 Position);
void Camera_SetRotation(camera *Camera, f32 Yaw, f32 Pitch);
vec3 Camera_Direction(const camera Camera);
vec3 Camera_Forward(const camera Camera);
vec3 Camera_Right(const camera Camera);
vec3 Camera_ToScreen(const bitmap Screen, vec3 Position);
vec3 Camera_FromScreen(const bitmap Screen, vec3 Position);
vec3 Camera_FromWorldDir(const camera Camera, vec3 Direction);
vec3 Camera_ToWorldDir(const camera Camera, vec3 Direction);
vec3 Camera_FromWorld(const camera Camera, vec3 Position);
vec3 Camera_ToWorld(const camera Camera, vec3 Position);
vec3 Camera_WorldToScreen(const camera Camera, const bitmap Screen, vec3 Position);
vec3 Camera_ScreenToWorld(const camera Camera, const bitmap Screen, vec3 Position);
f32 Camera_CalcZ(const camera Camera, vec3 Position);
f32 Camera_CalcDist(const camera Camera, vec3 Position);
bool Camera_PointVisible(const camera Camera, const bitmap Screen, vec3 Position);
bool Camera_BoxVisible(const camera Camera, const bitmap Screen, box Box);

/******************/
/* IMPLEMENTATION */
/******************/

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
        .y = Camera.CosPitch * Camera.CosYaw,
        .z = Camera.SinPitch,
    };
}

vec3 Camera_Forward(const camera Camera)
{
    return (vec3) {
        .x = Camera.SinYaw,
        .y = Camera.CosYaw,
    };
}

vec3 Camera_Right(const camera Camera)
{
    return (vec3) {
        .x = Camera.CosYaw,
        .y = -Camera.SinYaw,
    };
}

vec3 Camera_ToScreen(const bitmap Screen, vec3 Position)
{
    f32 HalfWidth = Screen.Width * 0.5f;
    f32 HalfHeight = Screen.Height * 0.5f;
    f32 MinDim = Min(HalfWidth, HalfHeight);
    return (vec3) {
        .x = Position.x * MinDim + HalfWidth,
        .y = Position.z * MinDim + HalfHeight,
        .z = Position.y,
    };
}

vec3 Camera_FromScreen(const bitmap Screen, vec3 Position)
{
    f32 HalfWidth = Screen.Width * 0.5f;
    f32 HalfHeight = Screen.Height * 0.5f;
    f32 MinDim = Min(HalfWidth, HalfHeight);
    return (vec3) {
        .x = Position.x - HalfWidth / MinDim,
        .y = Position.z,
        .z = Position.y - HalfHeight / MinDim,
    };
}

vec3 Camera_FromWorldDir(const camera Camera, vec3 Direction)
{
    {
        f32 Newx = Camera.CosYaw * Direction.x - Camera.SinYaw * Direction.y;
        f32 Newy = Camera.CosYaw * Direction.y + Camera.SinYaw * Direction.x;
        Direction.x = Newx;
        Direction.y = Newy;
    }
    {
        f32 Newz = Camera.CosPitch * Direction.z - Camera.SinPitch * Direction.y;
        f32 Newy = Camera.CosPitch * Direction.y + Camera.SinPitch * Direction.z;
        Direction.z = Newz;
        Direction.y = Newy;
    }

    return Direction;
}

vec3 Camera_ToWorldDir(const camera Camera, vec3 Direction)
{
    {
        f32 Newz = Camera.CosPitch * Direction.z + Camera.SinPitch * Direction.y;
        f32 Newy = Camera.CosPitch * Direction.y - Camera.SinPitch * Direction.z;
        Direction.z = Newz;
        Direction.y = Newy;
    }
    {
        f32 Newx = Camera.CosYaw * Direction.x + Camera.SinYaw * Direction.y;
        f32 Newy = Camera.CosYaw * Direction.y - Camera.SinYaw * Direction.x;
        Direction.x = Newx;
        Direction.y = Newy;
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

mat4 Camera_WorldToScreenMatrix(const camera Camera, const bitmap Screen)
{
    mat4 Transform = Mat4_Translation(Camera.Position);
    Transform = Mat4_Mul(Mat4_RotYaw(Camera.Yaw), Transform);
    Transform = Mat4_Mul(Mat4_RotPitch(Camera.Pitch), Transform);

    f32 HalfWidth = Screen.Width * 0.5f;
    f32 HalfHeight = Screen.Height * 0.5f;
    f32 MinDim = Min(HalfWidth, HalfHeight);
    mat4 ScreenMat = (mat4){{
        { MinDim, 0, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, MinDim, 0, 0 },
        { HalfWidth, HalfHeight, 0, 1 },
    }};
    Transform = Mat4_Mul(ScreenMat, Transform);

    return Transform;
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

    vec3 A = { -X, 1, Y, };
    vec3 B = {  X, 1, Y, };
    vec3 C = {  X, 1,-Y, };
    vec3 D = { -X, 1,-Y, };

    if (Vec3_Dot(Position, Vec3_Cross(A, B)) < 0) return false;
    if (Vec3_Dot(Position, Vec3_Cross(B, C)) < 0) return false;
    if (Vec3_Dot(Position, Vec3_Cross(C, D)) < 0) return false;
    if (Vec3_Dot(Position, Vec3_Cross(D, A)) < 0) return false;
    return true;
}

bool Camera_BoxVisible(const camera Camera, const bitmap Screen, box Box)
{
    vec3 Half = (vec3){ 0.5f, 0.5f, 0.5f };
    vec3 HalfDim = Vec3_Mul(Vec3_Sub(Box.Max, Box.Min), Half);
    vec3 Center = Vec3_Sub(Vec3_Add(Box.Min, HalfDim), Camera.Position);

    f32 MinDim = 1.0f / Min(Screen.Width, Screen.Height);
    f32 X = Screen.Width * MinDim;
    f32 Y = Screen.Height * MinDim;

    vec3 A = { -X, 1, Y };
    vec3 B = {  X, 1, Y };
    vec3 C = {  X, 1,-Y };
    vec3 D = { -X, 1,-Y };

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
