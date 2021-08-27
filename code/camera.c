
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

vec3 Camera_FromWorld(const camera Camera, vec3 Position)
{
    Position.x -= Camera.Position.x;
    Position.y -= Camera.Position.y;
    Position.z -= Camera.Position.z;

    {
        f32 Newx = Camera.CosYaw * Position.x - Camera.SinYaw * Position.z;
        f32 Newz = Camera.CosYaw * Position.z + Camera.SinYaw * Position.x;
        Position.x = Newx;
        Position.z = Newz;
    }
    {
        f32 Newy = Camera.CosPitch * Position.y - Camera.SinPitch * Position.z;
        f32 Newz = Camera.CosPitch * Position.z + Camera.SinPitch * Position.y;
        Position.y = Newy;
        Position.z = Newz;
    }

    return Position;
}

f32 Camera_CalcZ(const camera Camera, vec3 Position)
{
    Position.x -= Camera.Position.x;
    Position.y -= Camera.Position.y;
    Position.z -= Camera.Position.z;
    Position.z = Camera.CosYaw * Position.z + Camera.SinYaw * Position.x;
    Position.z = Camera.CosPitch * Position.z + Camera.SinPitch * Position.y;
    return Position.z;
}

vec3 Camera_WorldToScreen(const camera Camera, const bitmap Screen, vec3 Position)
{
    return Camera_ToScreen(Screen, Camera_FromWorld(Camera, Position));
}
