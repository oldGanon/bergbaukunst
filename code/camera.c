
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
    Camera->Pitch = Modulo(Pitch, 2*MATH_PI);
    Camera->SinYaw = Sin(Camera->Yaw);
    Camera->CosYaw = Cos(Camera->Yaw);
    Camera->SinPitch = Sin(Camera->Pitch);
    Camera->CosPitch = Cos(Camera->Pitch);
}

vec3 Camera_Forward(const camera Camera)
{
    vec3 Forward = { 0 };
    
    Forward.x = Camera.SinYaw * Camera.CosPitch;
    Forward.y = Camera.SinPitch;
    Forward.z = Camera.CosPitch * Camera.CosYaw;

    return Forward;
}

vec3 CameraToScreen(const bitmap Screen, vec3 Position)
{
    f32 Width = Screen.Width * 0.5f;
    f32 Height = Screen.Height * 0.5f;
    f32 MinDim = Min(Width, Height);
    Position.x = (Position.x / Position.z) * MinDim + Width;
    Position.y = (Position.y / Position.z) * MinDim + Height;

    return Position;
}

vec3 WorldToCamera(const camera Camera, vec3 Position)
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
