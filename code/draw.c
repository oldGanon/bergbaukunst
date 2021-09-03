
typedef struct color
{
    u8 Value;
} color;

#define COLOR_TRANSPARENT (color){   0 }
#define COLOR_BLACK       (color){   1 }
#define COLOR_GRAY        (color){  16 }
#define COLOR_WHITE       (color){  32 }
#define COLOR_RED         (color){  48 }
#define COLOR_PINK        (color){  64 }
#define COLOR_DARKBROWN   (color){  80 }
#define COLOR_BROWN       (color){  96 }
#define COLOR_ORANGE      (color){ 112 }
#define COLOR_YELLOW      (color){ 128 }
#define COLOR_DARKGREEN   (color){ 144 }
#define COLOR_GREEN       (color){ 160 }
#define COLOR_LIGHTGREEN  (color){ 176 }
#define COLOR_DARKBLUE    (color){ 192 }
#define COLOR_BLUE        (color){ 208 }
#define COLOR_LIGHTBLUE   (color){ 224 }
#define COLOR_SKYBLUE     (color){ 240 }

#define CAMERA_NEAR 0.1f
#define CAMERA_FAR 256.0f

enum bitmap_flags
{
    BITMAP_ONHEAP,
};

typedef struct bitmap
{
    u32 Width, Height;
    u32 Pitch;
    u32 Flags;
    color *Pixels;
} bitmap;

typedef struct palette
{
    u32 Colors[256];
} palette;

typedef struct vertex
{
    vec3 Position;
    vec2 TexCoord;
    f32 Shadow;
} vertex;

// Bitmap
bitmap Bitmap_Create(u32 Width, u32 Height);
bitmap Bitmap_Section(bitmap Bitmap, i32 X, i32 Y, u32 W, u32 H);
color Bitmap_GetPixel(bitmap Bitmap, i32 X, i32 Y);
void Bitmap_SetPixel(bitmap Bitmap, color Color, i32 X, i32 Y);
void Bitmap_Clear(bitmap Bitmap, color Color);
void Draw_Bitmap(bitmap Target, const bitmap Bitmap, i32 X, i32 Y);

// Point
void Draw_PointStruct(bitmap, color, point);
void Draw_PointIVec2(bitmap, color, ivec2);
void Draw_PointVec3(bitmap, color, vec3);
void Draw_PointVec2(bitmap, color, vec2);

// Line
void Draw_LineStruct(bitmap, color, line);
void Draw_LineIVec2(bitmap, color, ivec2, ivec2);
void Draw_LineVec3(bitmap, color, vec3, vec3);
void Draw_LineVec2(bitmap, color, vec2, vec2);

// Triangle
void Draw_TriangleStruct(bitmap, color, triangle);
void Draw_TriangleIVec2(bitmap, color, ivec2, ivec2, ivec2);
void Draw_TriangleVec2(bitmap, color, vec2, vec2, vec2);
void Draw_TriangleVec3(bitmap, color, vec3, vec3, vec3);

// Rect
void Draw_RectIVec2(bitmap, color, ivec2, ivec2);
void Draw_RectVec2(bitmap, color, vec2, vec2);

// String
void Draw_String(bitmap, const bitmap, color, ivec2, const char *);

// Vertex Triangle
void Draw_TriangleVerts(bitmap, color, vertex, vertex, vertex);
void Draw_TriangleTexturedVerts(bitmap, const bitmap, vertex, vertex, vertex);

// Vertex Quad
void Draw_QuadVerts(bitmap, color, vertex, vertex, vertex, vertex);
void Draw_QuadTexturedVerts(bitmap, const bitmap, vertex, vertex, vertex, vertex);

// Generics
#define Draw_Point(BUFFER, COLOR, X, ...) _Generic((X), \
    point: Draw_PointStruct, \
    ivec2: Draw_PointIVec2, \
    vec3: Draw_PointVec3, \
    vec2: Draw_PointVec2 \
)(BUFFER, COLOR, X, __VA_ARGS__)

#define Draw_Line(BUFFER, COLOR, X, ...) _Generic((X), \
    line: Draw_LineStruct, \
    ivec2: Draw_LineIVec2, \
    vec3: Draw_LineVec3, \
    vec2: Draw_LineVec2 \
)(BUFFER, COLOR, X, __VA_ARGS__)

#define Draw_Rect(BUFFER, COLOR, X, ...) _Generic((X), \
    rect: Draw_RectStruct, \
    ivec2: Draw_RectIVec2, \
    vec2: Draw_RectVec2 \
)(BUFFER, COLOR, X, __VA_ARGS__)

#define Draw_Triangle(BUFFER, COLOR, X, ...) _Generic((COLOR), \
    color: _Generic((X), \
        vertex: Draw_TriangleVerts,\
        triangle: Draw_TriangleStruct, \
        ivec2: Draw_TriangleIVec2, \
        vec3: Draw_TriangleVec3, \
        vec2: Draw_TriangleVec2 \
    ), \
    bitmap: Draw_TriangleTexturedVerts, \
    const bitmap: Draw_TriangleTexturedVerts \
)(BUFFER, COLOR, X, __VA_ARGS__)

#define Draw_Quad(BUFFER, COLOR, X, ...) _Generic((COLOR), \
    color: Draw_QuadVerts, \
    bitmap: Draw_QuadTexturedVerts, \
    const bitmap: Draw_QuadTexturedVerts \
)(BUFFER, COLOR, X, __VA_ARGS__)

// Util
inline vec3 Draw__PerspectiveDivide(bitmap Target, vec3 Position)
{
    f32 HalfWidth = Target.Width * 0.5f;
    f32 HalfHeight = Target.Height * 0.5f;
    Position.x = (Position.x - HalfWidth) / Position.z + HalfWidth;
    Position.y = (Position.y - HalfHeight) / Position.z + HalfHeight;
    return Position;
}

/************/
/*  Bitmap  */
/************/

bitmap Bitmap_Create(u32 Width, u32 Height)
{
    return (bitmap) {
        .Width = Width,
        .Height = Height,
        .Pitch = Width,
        .Flags = BITMAP_ONHEAP,
        .Pixels = (color *)malloc(sizeof(u32) * Width * Height)
    };
}

void Bitmap_Delete(bitmap Bitmap)
{
    if (Bitmap.Flags & BITMAP_ONHEAP)
        free(Bitmap.Pixels);
}

bitmap Bitmap_Section(bitmap Bitmap, i32 X, i32 Y, u32 W, u32 H)
{
    bitmap Section = { 0 };

    if (W == 0) return Section;
    if (H == 0) return Section;
    if (X >= (i32)Bitmap.Width)  return Section;
    if (Y >= (i32)Bitmap.Height) return Section;

    i32 X1 = X + W;
    i32 Y1 = Y + H;
    if (X1 < 0) return Section;
    if (Y1 < 0) return Section;

    if (X < 0) X = 0;
    if (Y < 0) Y = 0;
    if (X1 >= (i32)Bitmap.Width)  X1 = Bitmap.Width;
    if (Y1 >= (i32)Bitmap.Height) Y1 = Bitmap.Height;

    Section.Width = X1 - X;
    Section.Height = Y1 - Y;
    Section.Pitch = Bitmap.Pitch;
    Section.Pixels = Bitmap.Pixels + X + Y * Bitmap.Pitch;
    return Section;
}

void Bitmap_Blit(bitmap Dst, bitmap Src)
{
    if (Dst.Width != Src.Width) return;
    if (Dst.Height != Src.Height) return;
    for (u32 y = 0; y < Dst.Height; ++y)
    for (u32 x = 0; x < Dst.Width; ++x)
        if (Src.Pixels[x + y * Src.Pitch].Value)
            Dst.Pixels[x + y * Dst.Pitch] = Src.Pixels[x + y * Src.Pitch];
}

inline color Bitmap__GetPixelFast(bitmap Bitmap, i32 X, i32 Y)
{
    return *(Bitmap.Pixels + X + Y * Bitmap.Pitch);
}

color Bitmap_GetPixel(bitmap Bitmap, i32 X, i32 Y)
{
    if ((u32)X >= Bitmap.Width)  return (color){ 0 };
    if ((u32)Y >= Bitmap.Height) return (color){ 0 };
    return Bitmap__GetPixelFast(Bitmap, X, Y);
}

inline void Bitmap__SetPixelFast(bitmap Bitmap, color Color, i32 X, i32 Y)
{
    *(Bitmap.Pixels + X + Y * Bitmap.Pitch) = Color;
}

void Bitmap_SetPixel(bitmap Bitmap, color Color, i32 X, i32 Y)
{
    if ((u32)X >= Bitmap.Width)  return;
    if ((u32)Y >= Bitmap.Height) return;
    Bitmap__SetPixelFast(Bitmap, Color, X, Y);
}

void Bitmap_Clear(bitmap Bitmap, color Color)
{
    for (u32 y = 0; y < Bitmap.Height; ++y)
    for (u32 x = 0; x < Bitmap.Width; ++x)
        Bitmap__SetPixelFast(Bitmap, Color, x, y);
}

void Draw_Bitmap(bitmap Target, const bitmap Bitmap, i32 X, i32 Y)
{
    bitmap Src = Bitmap_Section(Bitmap,-X,-Y, Target.Width, Target.Height);
    bitmap Dst = Bitmap_Section(Target, X, Y, Bitmap.Width, Bitmap.Height);
    Bitmap_Blit(Dst, Src);
}

/*************/
/*   Point   */
/*************/

void Draw_PointStruct(bitmap Target, color Color, point Point)
{
    Bitmap_SetPixel(Target, Color, F32_FloorToI32(Point.x), F32_FloorToI32(Point.y));
}

void Draw_PointIVec2(bitmap Target, color Color, ivec2 Point)
{
    Bitmap_SetPixel(Target, Color, Point.x, Point.y);
}

void Draw_PointVec2(bitmap Target, color Color, vec2 Point)
{
    Bitmap_SetPixel(Target, Color, F32_FloorToI32(Point.x), F32_FloorToI32(Point.y));
}

void Draw_PointVec3(bitmap Target, color Color, vec3 Point)
{
    if (Point.z <= 0) return;
    Point = Draw__PerspectiveDivide(Target, Point);
    Bitmap_SetPixel(Target, Color, F32_FloorToI32(Point.x), F32_FloorToI32(Point.y));
}

/************/
/*   Line   */
/************/

inline void Draw__VerticalLine(bitmap Target, color Color, i32 X, i32 Y0, i32 Y1)
{
    if ((u32)X >= Target.Width) return;

    if (Y0 < 0) Y0 = 0;
    if (Y1 > (i32)Target.Height) Y1 = Target.Height;

    for (i32 y = Y0; y < Y1; ++y)
        Bitmap__SetPixelFast(Target, Color, X, y);
}

inline void Draw__HorizontalLine(bitmap Target, color Color, i32 X0, i32 X1, i32 Y)
{
    if ((u32)Y >= Target.Height) return;
    
    if (X0 < 0) X0 = 0;
    if (X1 > (i32)Target.Width) X1 = Target.Width;

    for (i32 x = X0; x < X1; ++x)
        Bitmap__SetPixelFast(Target, Color, x, Y);
}

inline bool Draw__ClipLine(vec2 Min, vec2 Max, vec2 *A, vec2 *B)
{
    vec2 a = *A;
    vec2 b = *B;

    vec2 One = (vec2) { 1, 1 };
    vec2 Inv = Div(One, Sub(b, a));
    vec2 t0 = Mul(Sub(Min, a), Inv);
    vec2 t1 = Mul(Sub(Max, a), Inv);
    vec2 tt0 = Min(t0, t1);
    vec2 tt1 = Max(t0, t1);
    f32 tmin = Max(tt0.x, tt0.y);
    f32 tmax = Min(tt1.x, tt1.y);

    if ((tmax < 0) || (tmin > tmax) || (1 < tmin))
        return false;
    
    if (0 < tmin) *A = Lerp(a, b, tmin);
    if (tmax < 1) *B = Lerp(a, b, tmax);

    return true;
}

void Draw_LineStruct(bitmap Target, color Color, line Line)
{
    rect Clip = (rect){ .x = 0.0625f, .y = 0.0625f, .w = (f32)Target.Width-0.125f, .h = (f32)Target.Height-0.125f };
    
    if (!Line_Clip(&Line, Clip)) return;
    
    Line.a.x -= 0.5f;
    Line.a.y -= 0.5f;
    Line.b.x -= 0.5f;
    Line.b.y -= 0.5f;

    {   // check if vertical or horizontal line
        const i32 ix0 = F32_CeilToI32(Min(Line.a.x, Line.b.x));
        const i32 ix1 = F32_CeilToI32(Max(Line.a.x, Line.b.x));
        const i32 iy0 = F32_CeilToI32(Min(Line.a.y, Line.b.y));
        const i32 iy1 = F32_CeilToI32(Max(Line.a.y, Line.b.y));
        if (ix0 == ix1) { Draw__VerticalLine(Target, Color, ix0, iy0, iy1); return; }
        if (iy0 == iy1) { Draw__HorizontalLine(Target, Color, ix0, ix1, iy0); return; }
    }

    f32 dx = Line.b.x - Line.a.x;
    f32 dy = Line.b.y - Line.a.y;
    if (Abs(dy) <= Abs(dx))
    {
        dy /= dx;
        f32 y, x0, x1;
        if (dx < 0)
        {
            x0 = Ceil(Line.b.x);
            x1 = Ceil(Line.a.x);
            f32 d = dy * (x0 - Line.b.x);
            y = Line.b.y + d + 0.5f;
        }
        else
        {
            x0 = Ceil(Line.a.x);
            x1 = Ceil(Line.b.x);
            f32 d = dy * (x0 - Line.a.x);
            y = Line.a.y + d + 0.5f;
        }
        const i32 ix0 = F32_FloatToI32(x0);
        const i32 ix1 = F32_FloatToI32(x1);
        for (i32 x = ix0; x < ix1; ++x)
        {
            Bitmap__SetPixelFast(Target, Color, x, F32_FloorToI32(y));
            y += dy;
        }
    }
    else
    {
        dx /= dy;
        f32 x, y0, y1;
        if (dy < 0)
        {
            y0 = Ceil(Line.b.y);
            y1 = Ceil(Line.a.y);
            f32 d = dx * (y0 - Line.b.y);
            x = Line.b.x + d + 0.5f;
        }
        else
        {
            y0 = Ceil(Line.a.y);
            y1 = Ceil(Line.b.y);
            f32 d = dx * (y0 - Line.a.y);
            x = Line.a.x + d + 0.5f;
        }
        const i32 iy0 = F32_FloatToI32(y0);
        const i32 iy1 = F32_FloatToI32(y1);
        for (i32 y = iy0; y < iy1; ++y)
        {
            Bitmap__SetPixelFast(Target, Color, F32_FloorToI32(x), y);
            x += dx;
        }
    }
}

void Draw_LineIVec2(bitmap Target, color Color, ivec2 A, ivec2 B)
{
    Draw_LineStruct(Target, Color, (line){
        .a = { .x = (A.x + 0.5f), .y = (A.y + 0.5f) },
        .b = { .x = (B.x + 0.5f), .y = (B.y + 0.5f) }
    });
}

void Draw_LineVec2(bitmap Target, color Color, vec2 A, vec2 B)
{
    Draw_LineStruct(Target, Color, (line){
        .a = { .x = A.x, .y = A.y },
        .b = { .x = B.x, .y = B.y }
    });
}

void Draw_LineVec3(bitmap Target, color Color, vec3 A, vec3 B)
{
    if (A.z <= CAMERA_NEAR && B.z <= CAMERA_NEAR) return;
    if (A.z <= CAMERA_NEAR) A = Vec3_Lerp(B, A, (B.z - CAMERA_NEAR) / (B.z - A.z));
    if (B.z <= CAMERA_NEAR) B = Vec3_Lerp(A, B, (A.z - CAMERA_NEAR) / (A.z - B.z));
    A = Draw__PerspectiveDivide(Target, A);
    B = Draw__PerspectiveDivide(Target, B);
    Draw_LineVec2(Target, Color, A.xy, B.xy);
}

/**************/
/*  Triangle  */
/**************/

void Draw_TriangleStruct(bitmap Target, color Color, triangle Triangle)
{
    rect Clip = (rect){ .x = 0.0f, .y = 0.0f, .w = (f32)Target.Width, .h = (f32)Target.Height };

    Triangle.a.x -= 0.5f;
    Triangle.a.y -= 0.5f;
    Triangle.b.x -= 0.5f;
    Triangle.b.y -= 0.5f;
    Triangle.c.x -= 0.5f;
    Triangle.c.y -= 0.5f;

    f32 minx = Clip.x;
    f32 miny = Clip.y;
    f32 maxx = Clip.x + Clip.w;
    f32 maxy = Clip.y + Clip.h;

    const triangle Tri = Triangle_SortY(Triangle);
    
    const i32 iy0 = F32_CeilToI32(Max(Tri.a.y, miny));
    const i32 iy1 = F32_CeilToI32(Clamp(Tri.b.y, miny, maxy));
    const i32 iy2 = F32_CeilToI32(Min(Tri.c.y, maxy));

    const f32 dx01 = (Tri.b.x - Tri.a.x) / (Tri.b.y - Tri.a.y);
    const f32 dx02 = (Tri.c.x - Tri.a.x) / (Tri.c.y - Tri.a.y);
    const f32 dx12 = (Tri.c.x - Tri.b.x) / (Tri.c.y - Tri.b.y);

    const f32 da = Max(miny, Ceil(Tri.a.y)) - Tri.a.y;
    const f32 db = Max(miny, Ceil(Tri.b.y)) - Tri.b.y;

    f32 dx0 = Min(dx01, dx02);
    f32 dx1 = Max(dx01, dx02);        
    f32 x0 = Tri.a.x + (dx0 * da);
    f32 x1 = Tri.a.x + (dx1 * da);
    for (i32 y = iy0; y < iy1; ++y)
    {
        const i32 ix0 = F32_CeilToI32(Max(x0, minx));
        const i32 ix1 = F32_CeilToI32(Min(x1, maxx));
        for (i32 x = ix0; x < ix1; ++x)
            Bitmap__SetPixelFast(Target, Color, x, y);
        x0 += dx0;
        x1 += dx1;
    }

    dx0 = Max(dx12, dx02);
    dx1 = Min(dx12, dx02);
    if (dx01 < dx02) x0 = Tri.b.x + (dx0 * db);
    else             x1 = Tri.b.x + (dx1 * db);
    for (i32 y = iy1; y < iy2; ++y)
    {
        const i32 ix0 = F32_CeilToI32(Max(x0, minx));
        const i32 ix1 = F32_CeilToI32(Min(x1, maxx));
        for (i32 x = ix0; x < ix1; ++x)
            Bitmap__SetPixelFast(Target, Color, x, y);
        x0 += dx0;
        x1 += dx1;
    }
}

void Draw_TriangleIVec2(bitmap Target, color Color, ivec2 A, ivec2 B, ivec2 C)
{
    triangle Triangle = (triangle){ 
        .a = { .x = (A.x + 0.5f), .y = (A.y + 0.5f) },
        .b = { .x = (B.x + 0.5f), .y = (B.y + 0.5f) },
        .c = { .x = (C.x + 0.5f), .y = (C.y + 0.5f) }
    };
    Draw_TriangleStruct(Target, Color, Triangle);
}

void Draw_TriangleVec2(bitmap Target, color Color, vec2 A, vec2 B, vec2 C)
{
    triangle Triangle = (triangle){ 
        .a = { .x = A.x, .y = A.y },
        .b = { .x = B.x, .y = B.y },
        .c = { .x = C.x, .y = C.y }
    };
    Draw_TriangleStruct(Target, Color, Triangle);
}

/************/
/*   Rect   */
/************/

void Draw_RectStruct(bitmap Target, color Color, rect Rect)
{
    bitmap ClearRect = Bitmap_Section(Target, F32_FloorToI32(Rect.x), 
                                              F32_FloorToI32(Rect.y),
                                              F32_FloorToI32(Rect.w),
                                              F32_FloorToI32(Rect.h));
    Bitmap_Clear(ClearRect, Color);
}

void Draw_RectIVec2(bitmap Target, color Color, ivec2 Min, ivec2 Dim)
{
    bitmap ClearRect = Bitmap_Section(Target, Min.x, Min.y, Dim.x, Dim.y);
    Bitmap_Clear(ClearRect, Color);
}

void Draw_RectVec2(bitmap Target, color Color, vec2 Min, vec2 Dim)
{
    bitmap ClearRect = Bitmap_Section(Target, F32_FloorToI32(Min.x), 
                                              F32_FloorToI32(Min.y),
                                              F32_FloorToI32(Dim.x),
                                              F32_FloorToI32(Dim.y));
    Bitmap_Clear(ClearRect, Color);
}

/**************/
/*   String   */
/**************/

void Draw_String(bitmap Target, const bitmap Font, color Color, ivec2 Position, const char *String)
{
    i32 X = Position.x;
    i32 Y = Position.y;
    i32 CW = Font.Width / 8;
    i32 CH = Font.Height / 8;
    while (*String)
    {
        char C = *String++;
        if (C == '\n') { X = Position.x; Y -= 8; continue; }
        i32 CX = (C % CW) * 8;
        i32 CY = (C / CW) * 8;
        struct bitmap Char = Bitmap_Section(Font, CX, CY, 8, 8);
        Draw_Bitmap(Target, Char, X, Y);
        X += 8;
    }
}

/**/
/**/
/**/
#if 0
void Draw__TriangleTexturedShadedVerts3D(bitmap Target, const bitmap Texture, vertex A, vertex B, vertex C)
{
    _mm_setcsr(_mm_getcsr() | 0x8040);
#define PACK_WIDTH_SHIFT 2
#define PACK_HEIGHT_SHIFT 0
#define PACK_WIDTH (1 << PACK_WIDTH_SHIFT)
#define PACK_HEIGHT (1 << PACK_HEIGHT_SHIFT)

    __m128 ColOffset = _mm_setr_ps(0, 1, 2, 3);
    __m128 RowOffset = _mm_setr_ps(0, 0, 0, 0);
    __m128 One = _mm_set1_ps(1);

    __m128i vmul = _mm_set1_epi32(Texture.Pitch);
    __m128i umask = _mm_set1_epi32(Texture.Width - 1);
    __m128i vmask = _mm_set1_epi32(Texture.Height - 1);
    __m128i smask = _mm_set1_epi32(7);
    __m128 ssize  = _mm_set1_ps(7);

    __m128 Bayer[4] = {
        _mm_set_ps( 0.0f/16.0f, 8.0f/16.0f, 2.0f/16.0f,10.0f/16.0f),
        _mm_set_ps(12.0f/16.0f, 4.0f/16.0f,14.0f/16.0f, 6.0f/16.0f),
        _mm_set_ps( 3.0f/16.0f,11.0f/16.0f, 1.0f/16.0f, 9.0f/16.0f),
        _mm_set_ps(15.0f/16.0f, 7.0f/16.0f,13.0f/16.0f, 5.0f/16.0f),
    };

    f32 MinX = 0;
    f32 MinY = 0;
    f32 MaxX = (f32)Target.Width;
    f32 MaxY = (f32)Target.Height;

    {
        f32 X[3] = { A.Position.x, B.Position.x, C.Position.x };
        f32 Y[3] = { A.Position.y, B.Position.y, C.Position.y };
        
        f32 z[3] = { 1.0f / A.Position.z, 1.0f / B.Position.z, 1.0f / C.Position.z };
        f32 a[3] = { z[0], 0, 0 };
        f32 b[3] = { 0, z[1], 0 };
        f32 s[3] = { A.Shadow, B.Shadow, C.Shadow };
        f32 u[3] = { A.TexCoord.u, B.TexCoord.u, C.TexCoord.u };
        f32 v[3] = { A.TexCoord.v, B.TexCoord.v, C.TexCoord.v };
        
        // barycentric edge functions
        // FAB(x, y) = (A.y - B.y)x + (B.x - A.x)y + (A.x * B.yy - B.x * A.y) = 0
        // FBC(x, y) = (B.y - C.y)x + (C.x - B.x)y + (B.x * C.yy - C.x * B.y) = 0
        // FCA(x, y) = (C.y - A.y)x + (A.x - C.x)y + (C.x * A.yy - A.x * C.y) = 0

        f32 F12_dx = Y[1] - Y[2];
        f32 F20_dx = Y[2] - Y[0];
        f32 F01_dx = Y[0] - Y[1];

        f32 F12_dy = X[2] - X[1];
        f32 F20_dy = X[0] - X[2];
        f32 F01_dy = X[1] - X[0];

        f32 F12_0 = X[1] * Y[2] - X[2] * Y[1];
        f32 F20_0 = X[2] * Y[0] - X[0] * Y[2];
        f32 F01_0 = X[0] * Y[1] - X[1] * Y[0];

        f32 invTriArea = 1.0f / (F01_dy * F20_dx - F20_dy * F01_dx);
        z[1] = (z[1] - z[0]) * invTriArea;
        z[2] = (z[2] - z[0]) * invTriArea;
        a[1] = (a[1] - a[0]) * invTriArea;
        a[2] = (a[2] - a[0]) * invTriArea;
        b[1] = (b[1] - b[0]) * invTriArea;
        b[2] = (b[2] - b[0]) * invTriArea;

        MinX = Max(Floor(Min(Min(X[0], X[1]), X[2]) / PACK_WIDTH) * PACK_WIDTH, MinX);
        MinY = Max(Floor(Min(Min(Y[0], Y[1]), Y[2]) / PACK_HEIGHT) * PACK_HEIGHT, MinY);
        MaxX = Min(Ceil(Max(Max(X[0], X[1]), X[2])), MaxX);
        MaxY = Min(Ceil(Max(Max(Y[0], Y[1]), Y[2])), MaxY);

        {
            __m128 z4[3] = { _mm_set1_ps(z[0]), _mm_set1_ps(z[1]), _mm_set1_ps(z[2]) };
            __m128 a4[3] = { _mm_set1_ps(a[0]), _mm_set1_ps(a[1]), _mm_set1_ps(a[2]) };
            __m128 b4[3] = { _mm_set1_ps(b[0]), _mm_set1_ps(b[1]), _mm_set1_ps(b[2]) };
            __m128 s4[3] = { _mm_set1_ps(s[0]), _mm_set1_ps(s[1]), _mm_set1_ps(s[2]) };
            __m128 u4[3] = { _mm_set1_ps(u[0]), _mm_set1_ps(u[1]), _mm_set1_ps(u[2]) };
            __m128 v4[3] = { _mm_set1_ps(v[0]), _mm_set1_ps(v[1]), _mm_set1_ps(v[2]) };
        
            __m128 F12_dx4 = _mm_set1_ps(F12_dx);
            __m128 F20_dx4 = _mm_set1_ps(F20_dx);
            __m128 F01_dx4 = _mm_set1_ps(F01_dx);

            __m128 F12_dy4 = _mm_set1_ps(F12_dy);
            __m128 F20_dy4 = _mm_set1_ps(F20_dy);
            __m128 F01_dy4 = _mm_set1_ps(F01_dy);

            __m128 Col = _mm_add_ps(ColOffset, _mm_set1_ps(MinX));
            __m128 A12_0 = _mm_mul_ps(F12_dx4, Col);
            __m128 A20_0 = _mm_mul_ps(F20_dx4, Col);
            __m128 A01_0 = _mm_mul_ps(F01_dx4, Col);

            __m128 Row = _mm_add_ps(RowOffset, _mm_set1_ps(MinY));
            __m128 B12_0 = _mm_mul_ps(F12_dy4, Row);
            __m128 B20_0 = _mm_mul_ps(F20_dy4, Row);
            __m128 B01_0 = _mm_mul_ps(F01_dy4, Row);

            __m128 F12_Row = _mm_add_ps(_mm_add_ps(A12_0, B12_0), _mm_set1_ps(F12_0));
            __m128 F20_Row = _mm_add_ps(_mm_add_ps(A20_0, B20_0), _mm_set1_ps(F20_0));
            __m128 F01_Row = _mm_add_ps(_mm_add_ps(A01_0, B01_0), _mm_set1_ps(F01_0));

            F12_dx4 = _mm_mul_ps(F12_dx4, _mm_set1_ps(PACK_WIDTH));
            F20_dx4 = _mm_mul_ps(F20_dx4, _mm_set1_ps(PACK_WIDTH));
            F01_dx4 = _mm_mul_ps(F01_dx4, _mm_set1_ps(PACK_WIDTH));

            F12_dy4 = _mm_mul_ps(F12_dy4, _mm_set1_ps(PACK_HEIGHT));
            F20_dy4 = _mm_mul_ps(F20_dy4, _mm_set1_ps(PACK_HEIGHT));
            F01_dy4 = _mm_mul_ps(F01_dy4, _mm_set1_ps(PACK_HEIGHT));

            __m128 zz_dx = _mm_add_ps(_mm_mul_ps(F20_dx4, z4[1]), _mm_mul_ps(F01_dx4, z4[2]));
            __m128 aa_dx = _mm_add_ps(_mm_mul_ps(F20_dx4, a4[1]), _mm_mul_ps(F01_dx4, a4[2]));
            __m128 bb_dx = _mm_add_ps(_mm_mul_ps(F20_dx4, b4[1]), _mm_mul_ps(F01_dx4, b4[2]));

            i32 iMinX = F32_FloatToI32(MinX);
            i32 iMinY = F32_FloatToI32(MinY);
            i32 iMaxX = F32_FloatToI32(MaxX);
            i32 iMaxY = F32_FloatToI32(MaxY);

            i32 RowIndex = iMinY * Target.Pitch + iMinX;

            for(i32 y = iMinY; y < iMaxY; y += PACK_HEIGHT)
            {
                i32 Index = RowIndex;
                __m128 Alpha = F12_Row;
                __m128 Beta  = F20_Row;
                __m128 Gamma = F01_Row;

                __m128 zz = _mm_add_ps(_mm_add_ps(z4[0], _mm_mul_ps(Beta, z4[1])), _mm_mul_ps(Gamma, z4[2]));
                __m128 aa = _mm_add_ps(_mm_add_ps(a4[0], _mm_mul_ps(Beta, a4[1])), _mm_mul_ps(Gamma, a4[2]));
                __m128 bb = _mm_add_ps(_mm_add_ps(b4[0], _mm_mul_ps(Beta, b4[1])), _mm_mul_ps(Gamma, b4[2]));

                __m128 dither = Bayer[y&3];

                for(i32 x = iMinX; x < iMaxX; x += PACK_WIDTH)
                {
                    __m128i Mask4 = _mm_srai_epi32(_mm_castps_si128(_mm_or_ps(_mm_or_ps(Alpha, Beta), Gamma)), 32);
                    Mask4 = _mm_cmpeq_epi32(Mask4, _mm_setzero_si128());
                    if (!_mm_test_all_zeros(Mask4, Mask4))
                    {
                        __m128 zzz = _mm_div_ps(One, zz);
                        __m128 aaa = _mm_mul_ps(aa, zzz);
                        __m128 bbb = _mm_mul_ps(bb, zzz);
                        __m128 ccc = _mm_sub_ps(One, _mm_add_ps(aaa, bbb));

                        __m128 uuu = _mm_add_ps(_mm_add_ps(_mm_mul_ps(u4[0], aaa), _mm_mul_ps(u4[1], bbb)), _mm_mul_ps(u4[2], ccc));
                        __m128 vvv = _mm_add_ps(_mm_add_ps(_mm_mul_ps(v4[0], aaa), _mm_mul_ps(v4[1], bbb)), _mm_mul_ps(v4[2], ccc));
                        __m128 sss = _mm_add_ps(_mm_add_ps(_mm_mul_ps(s4[0], aaa), _mm_mul_ps(s4[1], bbb)), _mm_mul_ps(s4[2], ccc));

                        sss = _mm_add_ps(_mm_mul_ps(sss, ssize), dither);

                        __m128i iuuu = _mm_and_si128(_mm_cvttps_epi32(uuu), umask);
                        __m128i ivvv = _mm_and_si128(_mm_cvttps_epi32(vvv), vmask);
                        __m128i isss = _mm_and_si128(_mm_cvttps_epi32(sss), smask);

                        __m128i iuv8 = _mm_add_epi32(iuuu, _mm_mullo_epi32(ivvv, vmul));

                        i32 iuv[4], is[4];
                        _mm_storeu_epi32(iuv, iuv8);
                        _mm_storeu_epi32(is, isss);

                        u32 M[4];
                        _mm_storeu_epi32(M, Mask4);

                        for (i32 p = 0; p < 4; ++p)
                        {
                            color *Dst = Target.Pixels + Index;
                            color Texel = *(Texture.Pixels + iuv[p]);
                            Texel.Value &= 0xF8;
                            Texel.Value &= M[p];
                            if (Texel.Value) Dst[p].Value = Texel.Value | (is[p] & 0xFF);
                        }
                    }

                    Index += 4;
                    Alpha = _mm_add_ps(Alpha, F12_dx4);
                    Beta  = _mm_add_ps(Beta,  F20_dx4);
                    Gamma = _mm_add_ps(Gamma, F01_dx4);
                    zz = _mm_add_ps(zz, zz_dx);
                    aa = _mm_add_ps(aa, aa_dx);
                    bb = _mm_add_ps(bb, bb_dx);
                }

                RowIndex += PACK_HEIGHT * Target.Pitch;
                F12_Row = _mm_add_ps(F12_Row, F12_dy4);
                F20_Row = _mm_add_ps(F20_Row, F20_dy4);
                F01_Row = _mm_add_ps(F01_Row, F01_dy4);
            }
        }
    }
}
#else
void Draw__TriangleTexturedShadedVerts3D(bitmap Target, const bitmap Texture, vertex A, vertex B, vertex C)
{
    _mm_setcsr(_mm_getcsr() | 0x8040);
#define PACK_WIDTH_SHIFT 2
#define PACK_HEIGHT_SHIFT 1
#define PACK_WIDTH (1 << PACK_WIDTH_SHIFT)
#define PACK_HEIGHT (1 << PACK_HEIGHT_SHIFT)

    __m256 ColOffset = _mm256_setr_ps(0, 1, 2, 3, 0, 1, 2, 3);
    __m256 RowOffset = _mm256_setr_ps(0, 0, 0, 0, 1, 1, 1, 1);
    __m256 One = _mm256_set1_ps(1);

    __m256i vmul = _mm256_set1_epi32(Texture.Pitch);
    __m256i umask = _mm256_set1_epi32(Texture.Width - 1);
    __m256i vmask = _mm256_set1_epi32(Texture.Height - 1);
    __m256i smask = _mm256_set1_epi32(7);
    __m256 ssize  = _mm256_set1_ps(7);

    __m256 Bayer[4] = {
        _mm256_set_ps( 0.0f/16.0f, 8.0f/16.0f, 2.0f/16.0f,10.0f/16.0f,
                      12.0f/16.0f, 4.0f/16.0f,14.0f/16.0f, 6.0f/16.0f),
        _mm256_set_ps( 3.0f/16.0f,11.0f/16.0f, 1.0f/16.0f, 9.0f/16.0f,
                      15.0f/16.0f, 7.0f/16.0f,13.0f/16.0f, 5.0f/16.0f),
    };

    f32 MinX = 0;
    f32 MinY = 0;
    f32 MaxX = (f32)Target.Width;
    f32 MaxY = (f32)Target.Height;

    {
        f32 X[3] = { A.Position.x, B.Position.x, C.Position.x };
        f32 Y[3] = { A.Position.y, B.Position.y, C.Position.y };
        
        f32 z[3] = { 1.0f / A.Position.z, 1.0f / B.Position.z, 1.0f / C.Position.z };
        f32 a[3] = { z[0], 0, 0 };
        f32 b[3] = { 0, z[1], 0 };
        f32 s[3] = { A.Shadow, B.Shadow, C.Shadow };
        f32 u[3] = { A.TexCoord.u, B.TexCoord.u, C.TexCoord.u };
        f32 v[3] = { A.TexCoord.v, B.TexCoord.v, C.TexCoord.v };
        
        // barycentric edge functions
        // FAB(x, y) = (A.y - B.y)x + (B.x - A.x)y + (A.x * B.yy - B.x * A.y) = 0
        // FBC(x, y) = (B.y - C.y)x + (C.x - B.x)y + (B.x * C.yy - C.x * B.y) = 0
        // FCA(x, y) = (C.y - A.y)x + (A.x - C.x)y + (C.x * A.yy - A.x * C.y) = 0

        f32 F12_dx = Y[1] - Y[2];
        f32 F20_dx = Y[2] - Y[0];
        f32 F01_dx = Y[0] - Y[1];

        f32 F12_dy = X[2] - X[1];
        f32 F20_dy = X[0] - X[2];
        f32 F01_dy = X[1] - X[0];

        f32 F12_0 = X[1] * Y[2] - X[2] * Y[1];
        f32 F20_0 = X[2] * Y[0] - X[0] * Y[2];
        f32 F01_0 = X[0] * Y[1] - X[1] * Y[0];

        f32 invTriArea = 1.0f / (F01_dy * F20_dx - F20_dy * F01_dx);
        z[1] = (z[1] - z[0]) * invTriArea;
        z[2] = (z[2] - z[0]) * invTriArea;
        a[1] = (a[1] - a[0]) * invTriArea;
        a[2] = (a[2] - a[0]) * invTriArea;
        b[1] = (b[1] - b[0]) * invTriArea;
        b[2] = (b[2] - b[0]) * invTriArea;

        MinX = Max(Floor(Min(Min(X[0], X[1]), X[2]) / PACK_WIDTH) * PACK_WIDTH, MinX);
        MinY = Max(Floor(Min(Min(Y[0], Y[1]), Y[2]) / PACK_HEIGHT) * PACK_HEIGHT, MinY);
        MaxX = Min(Ceil(Max(Max(X[0], X[1]), X[2])), MaxX);
        MaxY = Min(Ceil(Max(Max(Y[0], Y[1]), Y[2])), MaxY);

        {
            __m256 z8[3] = { _mm256_set1_ps(z[0]), _mm256_set1_ps(z[1]), _mm256_set1_ps(z[2]) };
            __m256 a8[3] = { _mm256_set1_ps(a[0]), _mm256_set1_ps(a[1]), _mm256_set1_ps(a[2]) };
            __m256 b8[3] = { _mm256_set1_ps(b[0]), _mm256_set1_ps(b[1]), _mm256_set1_ps(b[2]) };
            __m256 s8[3] = { _mm256_set1_ps(s[0]), _mm256_set1_ps(s[1]), _mm256_set1_ps(s[2]) };
            __m256 u8[3] = { _mm256_set1_ps(u[0]), _mm256_set1_ps(u[1]), _mm256_set1_ps(u[2]) };
            __m256 v8[3] = { _mm256_set1_ps(v[0]), _mm256_set1_ps(v[1]), _mm256_set1_ps(v[2]) };
        
            __m256 F12_dx8 = _mm256_set1_ps(F12_dx);
            __m256 F20_dx8 = _mm256_set1_ps(F20_dx);
            __m256 F01_dx8 = _mm256_set1_ps(F01_dx);

            __m256 F12_dy8 = _mm256_set1_ps(F12_dy);
            __m256 F20_dy8 = _mm256_set1_ps(F20_dy);
            __m256 F01_dy8 = _mm256_set1_ps(F01_dy);

            __m256 Col = _mm256_add_ps(ColOffset, _mm256_set1_ps(MinX));
            __m256 A12_0 = _mm256_mul_ps(F12_dx8, Col);
            __m256 A20_0 = _mm256_mul_ps(F20_dx8, Col);
            __m256 A01_0 = _mm256_mul_ps(F01_dx8, Col);

            __m256 Row = _mm256_add_ps(RowOffset, _mm256_set1_ps(MinY));
            __m256 B12_0 = _mm256_mul_ps(F12_dy8, Row);
            __m256 B20_0 = _mm256_mul_ps(F20_dy8, Row);
            __m256 B01_0 = _mm256_mul_ps(F01_dy8, Row);

            __m256 F12_Row = _mm256_add_ps(_mm256_add_ps(A12_0, B12_0), _mm256_set1_ps(F12_0));
            __m256 F20_Row = _mm256_add_ps(_mm256_add_ps(A20_0, B20_0), _mm256_set1_ps(F20_0));
            __m256 F01_Row = _mm256_add_ps(_mm256_add_ps(A01_0, B01_0), _mm256_set1_ps(F01_0));

            F12_dx8 = _mm256_mul_ps(F12_dx8, _mm256_set1_ps(PACK_WIDTH));
            F20_dx8 = _mm256_mul_ps(F20_dx8, _mm256_set1_ps(PACK_WIDTH));
            F01_dx8 = _mm256_mul_ps(F01_dx8, _mm256_set1_ps(PACK_WIDTH));

            F12_dy8 = _mm256_mul_ps(F12_dy8, _mm256_set1_ps(PACK_HEIGHT));
            F20_dy8 = _mm256_mul_ps(F20_dy8, _mm256_set1_ps(PACK_HEIGHT));
            F01_dy8 = _mm256_mul_ps(F01_dy8, _mm256_set1_ps(PACK_HEIGHT));

            __m256 zz_dx = _mm256_add_ps(_mm256_mul_ps(F20_dx8, z8[1]), _mm256_mul_ps(F01_dx8, z8[2]));
            __m256 aa_dx = _mm256_add_ps(_mm256_mul_ps(F20_dx8, a8[1]), _mm256_mul_ps(F01_dx8, a8[2]));
            __m256 bb_dx = _mm256_add_ps(_mm256_mul_ps(F20_dx8, b8[1]), _mm256_mul_ps(F01_dx8, b8[2]));

            i32 iMinX = F32_FloatToI32(MinX);
            i32 iMinY = F32_FloatToI32(MinY);
            i32 iMaxX = F32_FloatToI32(MaxX);
            i32 iMaxY = F32_FloatToI32(MaxY);

            i32 RowIndex = iMinY * Target.Pitch + iMinX;

            for(i32 y = iMinY; y < iMaxY; y += PACK_HEIGHT)
            {
                i32 Index = RowIndex;
                __m256 Alpha = F12_Row;
                __m256 Beta  = F20_Row;
                __m256 Gamma = F01_Row;

                __m256 zz = _mm256_add_ps(_mm256_add_ps(z8[0], _mm256_mul_ps(Beta, z8[1])), _mm256_mul_ps(Gamma, z8[2]));
                __m256 aa = _mm256_add_ps(_mm256_add_ps(a8[0], _mm256_mul_ps(Beta, a8[1])), _mm256_mul_ps(Gamma, a8[2]));
                __m256 bb = _mm256_add_ps(_mm256_add_ps(b8[0], _mm256_mul_ps(Beta, b8[1])), _mm256_mul_ps(Gamma, b8[2]));

                __m256 dither = Bayer[(y>>1)&1];

                for(i32 x = iMinX; x < iMaxX; x += PACK_WIDTH)
                {
                    __m256i mask = _mm256_srai_epi32(_mm256_castps_si256(_mm256_or_ps(_mm256_or_ps(Alpha, Beta), Gamma)), 32);
                    if (!_mm256_test_all_ones(mask))
                    {
                        __m256 zzz = _mm256_div_ps(One, zz);
                        __m256 aaa = _mm256_mul_ps(aa, zzz);
                        __m256 bbb = _mm256_mul_ps(bb, zzz);
                        __m256 ccc = _mm256_sub_ps(One, _mm256_add_ps(aaa, bbb));

                        // interpolate
                        __m256 uuu = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(u8[0], aaa), _mm256_mul_ps(u8[1], bbb)), _mm256_mul_ps(u8[2], ccc));
                        __m256 vvv = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(v8[0], aaa), _mm256_mul_ps(v8[1], bbb)), _mm256_mul_ps(v8[2], ccc));
                        __m256 sss = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(s8[0], aaa), _mm256_mul_ps(s8[1], bbb)), _mm256_mul_ps(s8[2], ccc));

                        sss = _mm256_add_ps(_mm256_mul_ps(sss, ssize), dither);

                        __m256i iuuu = _mm256_and_si256(_mm256_cvttps_epi32(uuu), umask);
                        __m256i ivvv = _mm256_and_si256(_mm256_cvttps_epi32(vvv), vmask);
                        __m256i isss = _mm256_and_si256(_mm256_cvttps_epi32(sss), smask);

                        __m256i iuv8 = _mm256_add_epi32(iuuu, _mm256_mullo_epi32(vmul, ivvv));

                        // gather
                        color *Dst0 = Target.Pixels + Index;
                        color *Dst1 = Dst0 + Target.Pitch;
                        __m256i dst = _mm256_set_m128i(_mm_loadu_si32(Dst1), _mm_loadu_si32(Dst0));
                        dst = _mm256_shuffle_epi8(dst, _mm256_set_epi8(3,3,3,3,2,2,2,2,1,1,1,1,0,0,0,0,3,3,3,3,2,2,2,2,1,1,1,1,0,0,0,0));

                        // shade
                        __m256i tex = _mm256_i32gather_epi32((const int *)Texture.Pixels, iuv8, 1);
                        tex = _mm256_and_si256(tex, _mm256_set1_epi32(0xF8));
                        mask = _mm256_or_si256(mask, _mm256_cmpeq_epi32(tex, _mm256_setzero_si256()));
                        tex = _mm256_or_si256(tex, isss);
                        dst = _mm256_blendv_epi8(tex, dst, mask);

                        // scatter
                        dst = _mm256_shuffle_epi8(dst, _mm256_set_epi8(0,0,0,0,0,0,0,0,0,0,0,0,12,8,4,0,0,0,0,0,0,0,0,0,0,0,0,0,12,8,4,0));
                        _mm_storeu_si32(Dst0, _mm256_castsi256_si128(dst));
                        _mm_storeu_si32(Dst1, _mm256_extracti128_si256(dst, 1));
                    }

                    Index += PACK_WIDTH;
                    Alpha = _mm256_add_ps(Alpha, F12_dx8);
                    Beta  = _mm256_add_ps(Beta,  F20_dx8);
                    Gamma = _mm256_add_ps(Gamma, F01_dx8);
                    zz = _mm256_add_ps(zz, zz_dx);
                    aa = _mm256_add_ps(aa, aa_dx);
                    bb = _mm256_add_ps(bb, bb_dx);
                }

                RowIndex += PACK_HEIGHT * Target.Pitch;
                F12_Row = _mm256_add_ps(F12_Row, F12_dy8);
                F20_Row = _mm256_add_ps(F20_Row, F20_dy8);
                F01_Row = _mm256_add_ps(F01_Row, F01_dy8);
            }
        }
    }
}
#endif

inline vertex Vertex_Lerp(vertex A, vertex B, f32 t)
{
    A.Position = Lerp(A.Position, B.Position, t);
    A.TexCoord = Lerp(A.TexCoord, B.TexCoord, t);
    A.Shadow = Lerp(A.Shadow, B.Shadow, t);
    return A;
}

inline u32 Draw__TriangleClipZ(vertex *V)
{
    if (Max(Max(V[0].Position.z, V[1].Position.z), V[2].Position.z) > CAMERA_FAR)
        return 0;

    vertex T;
    u32 Z = ((V[0].Position.z < CAMERA_NEAR) ? 1 : 0) |
            ((V[1].Position.z < CAMERA_NEAR) ? 2 : 0) |
            ((V[2].Position.z < CAMERA_NEAR) ? 4 : 0);
    switch (Z)
    {
        case 0: return 1;
        
        case 4: T = V[0]; V[0] = V[1]; V[1] = V[2]; V[2] = T;
        case 2: T = V[0]; V[0] = V[1]; V[1] = V[2]; V[2] = T;
        case 1:
        {
            f32 t01 = (CAMERA_NEAR - V[0].Position.z) / (V[1].Position.z - V[0].Position.z);
            f32 t02 = (CAMERA_NEAR - V[0].Position.z) / (V[2].Position.z - V[0].Position.z);
            vertex V01 = Vertex_Lerp(V[0], V[1], t01);
            vertex V02 = Vertex_Lerp(V[0], V[2], t02);
            V[0] = V01; V[3] = V[2]; V[4] = V02; V[5] = V01;
            return 2;
        }
        
        case 3: T = V[0]; V[0] = V[1]; V[1] = V[2]; V[2] = T;
        case 5: T = V[0]; V[0] = V[1]; V[1] = V[2]; V[2] = T;
        case 6:
        {
            f32 t01 = (CAMERA_NEAR - V[0].Position.z) / (V[1].Position.z - V[0].Position.z);
            f32 t02 = (CAMERA_NEAR - V[0].Position.z) / (V[2].Position.z - V[0].Position.z);
            V[1] = Vertex_Lerp(V[0], V[1], t01);
            V[2] = Vertex_Lerp(V[0], V[2], t02);
            return 1;
        }
        
        case 7: return 0;
    }
    return 0;
}

inline bool Draw__PrepareTriangleVerts(const bitmap Target, vertex *A, vertex *B, vertex *C)
{
    // triangle winding order
    f32 Cross = (B->Position.x - A->Position.x) * (C->Position.y - A->Position.y) - 
                (B->Position.y - A->Position.y) * (C->Position.x - A->Position.x);
    if (Cross < 0.0f) return false;

    // move pixel center
    vec3 PixelCenter = (vec3){ 0.5f, 0.5f, 0.0f };
    A->Position = Vec3_Sub(A->Position, PixelCenter);
    B->Position = Vec3_Sub(B->Position, PixelCenter);
    C->Position = Vec3_Sub(C->Position, PixelCenter);

    // triangle offscreen left
    f32 MaxX = Max(Max(A->Position.x, B->Position.x), C->Position.x);
    if (MaxX < 0) return false;
    
    // triangle offscreen right
    f32 MinX = Min(Min(A->Position.x, B->Position.x), C->Position.x);
    if (MinX > (f32)Target.Width) return false;

     // triangle offscreen bottom
    f32 MaxY = Max(Max(A->Position.y, B->Position.y), C->Position.y);
    if (MaxY < 0) return false;

    // triangle offscreen top
    f32 MinY = Min(Min(A->Position.y, B->Position.y), C->Position.y);
    if (MinY > (f32)Target.Height) return false;

    // triangle too small
    if (Abs(Floor(MaxX) - Floor(MinX)) < 1.0f) return false;
    if (Abs(Floor(MaxY) - Floor(MinY)) < 1.0f) return false;
    
    return true;
}

void Draw_TriangleTexturedVerts(bitmap Target, const bitmap Texture, vertex A, vertex B, vertex C)
{
    vertex Vertices[6] = { A, B, C };
    u32 TriangleCount = Draw__TriangleClipZ(Vertices);
    for (u32 i = 0; i < TriangleCount; ++i)
    {
        vertex *V = Vertices + (i * 3);
        V[0].Position = Draw__PerspectiveDivide(Target, V[0].Position);
        V[1].Position = Draw__PerspectiveDivide(Target, V[1].Position);
        V[2].Position = Draw__PerspectiveDivide(Target, V[2].Position);

        if (!Draw__PrepareTriangleVerts(Target, &V[0], &V[1], &V[2])) return;
        Draw__TriangleTexturedShadedVerts3D(Target, Texture, V[0], V[1], V[2]);
    }
}

void Draw_TriangleVerts(bitmap Target, color Color, vertex A, vertex B, vertex C)
{
    const bitmap ColorTexture = (const bitmap){ .Width = 1, .Height = 1, .Pitch = 0, .Pixels = &Color };
    Draw_TriangleTexturedVerts(Target, ColorTexture, A, B, C);
}

void Draw_QuadVerts(bitmap Target, color Color, vertex A, vertex B, vertex C, vertex D)
{
    // TODO: check convex
    // TODO: check coplanar
    Draw_TriangleVerts(Target, Color, A, B, C);
    Draw_TriangleVerts(Target, Color, C, D, A);
}

void Draw_QuadTexturedVerts(bitmap Target, bitmap Texture, vertex A, vertex B, vertex C, vertex D)
{
    // TODO: check convex
    // TODO: check coplanar
    Draw_TriangleTexturedVerts(Target, Texture, A, B, C);
    Draw_TriangleTexturedVerts(Target, Texture, C, D, A);
}
