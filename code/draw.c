
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
    f32 _reserved;
} vertex;

// Bitmap
bitmap Bitmap_Create(u32 Width, u32 Height);
bitmap Bitmap_Section(bitmap Bitmap, i32 X, i32 Y, u32 W, u32 H);
color Bitmap_GetPixel(bitmap Bitmap, i32 X, i32 Y);
void Bitmap_SetPixel(bitmap Bitmap, color Color, i32 X, i32 Y);
void Bitmap_Clear(bitmap Bitmap, color Color);
void Draw_Bitmap(bitmap Target, const bitmap Bitmap, ivec2 Position);

// Rasterizer
void Raserizer_Rasterize(void);
void Raserizer_Clear(color Color);
void Raserizer_Flush(void);
void Raserizer_SetTexture(bitmap Texture);
void Raserizer_Blit(bitmap Target);
void Raserizer_DrawTriangle(vertex A, vertex B, vertex C);
void Raserizer_DrawQuad(vertex A, vertex B, vertex C, vertex D);

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
ivec2 Draw_Character(bitmap, const bitmap, color, ivec2, char);
ivec2 Draw_String(bitmap, const bitmap, color, ivec2, const char *);

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
static vec3 Draw__PerspectiveDivide(i32 Width, i32 Height, vec3 Position)
{
    f32 HalfWidth = Width * 0.5f;
    f32 HalfHeight = Height * 0.5f;
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

void Bitmap_Destroy(bitmap Bitmap)
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

void Draw_Bitmap(bitmap Target, const bitmap Bitmap, ivec2 Position)
{
    bitmap Src = Bitmap_Section(Bitmap,-Position.x,-Position.y, Target.Width, Target.Height);
    bitmap Dst = Bitmap_Section(Target, Position.x, Position.y, Bitmap.Width, Bitmap.Height);
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
    Point = Draw__PerspectiveDivide(Target.Width, Target.Height, Point);
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
    A = Draw__PerspectiveDivide(Target.Width, Target.Height, A);
    B = Draw__PerspectiveDivide(Target.Width, Target.Height, B);
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

ivec2 Draw_Character(bitmap Target, const bitmap Font, color Color, ivec2 Position, char Character)
{
    i32 CW = Font.Width / 8;
    i32 CH = Font.Height / 8;
    i32 CX = (Character % CW) * 8;
    i32 CY = (Character / CW) * 8;
    struct bitmap Char = Bitmap_Section(Font, CX, CY, 8, 8);
    Draw_Bitmap(Target, Char, Position);
    Position.x += 8;
    return Position;
}

ivec2 Draw_String(bitmap Target, const bitmap Font, color Color, ivec2 Position, const char *String)
{
    ivec2 Cursor = Position;
    while (*String)
    {
        char C = *String++;
        if (C == '\n') { Cursor.x = Position.x; Cursor.y -= 8; continue; }
        Cursor = Draw_Character(Target, Font, Color, Cursor, C);
    }
    return Cursor;
}

ivec2 Draw_Number(bitmap Target, const bitmap Font, color Color, ivec2 Position, f32 Number)
{    
    if (Number < 0)
    {
        Position = Draw_Character(Target, Font, Color, Position, '-');
        Number = F32_Negate(Number);
    }
    
    u32 Length = 1;
    while (Number >= 10.0f)
    {
        Number *= 0.1f;
        ++Length;
    }

    while (Length--)
    {
        u8 Decimal = (u8)F32_FloorToI32(Number);
        Number = Fract(Number) * 10;
        char Character = Decimal + '0';
        Position = Draw_Character(Target, Font, Color, Position, Character);
    }

    Position = Draw_Character(Target, Font, Color, Position, '.');

    u32 DecimalPlaces = 2;
    while (DecimalPlaces--)
    {
        u8 Decimal = (u8)F32_FloatToI32(Number);
        Number = Fract(Number) * 10;
        char Character = Decimal + '0';
        Position = Draw_Character(Target, Font, Color, Position, Character);        
    }

    return Position;
}

/********************/
/*   Triangle 2.0   */
/********************/

inline vertex Vertex_Lerp(vertex A, vertex B, f32 t)
{
    A.Position = Lerp(A.Position, B.Position, t);
    A.TexCoord = Lerp(A.TexCoord, B.TexCoord, t);
    A.Shadow = Lerp(A.Shadow, B.Shadow, t);
    return A;
}

inline u32 Draw__TriangleClipZ(vertex *V)
{
    // if (Max(Max(V[0].Position.z, V[1].Position.z), V[2].Position.z) > CAMERA_FAR)
    //     return 0;

    u32 Z = ((V[0].Position.z < CAMERA_NEAR) ? 1 : 0) |
            ((V[1].Position.z < CAMERA_NEAR) ? 2 : 0) |
            ((V[2].Position.z < CAMERA_NEAR) ? 4 : 0);
    if (Z == 0) return 1;

    vertex T;
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

inline bool Draw__PrepareTriangleVerts(i32 Width, i32 Height, vertex *A, vertex *B, vertex *C)
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
    if (MinX > (f32)Width) return false;

     // triangle offscreen bottom
    f32 MaxY = Max(Max(A->Position.y, B->Position.y), C->Position.y);
    if (MaxY < 0) return false;

    // triangle offscreen top
    f32 MinY = Min(Min(A->Position.y, B->Position.y), C->Position.y);
    if (MinY > (f32)Height) return false;
    
    // triangle too small
    if (Abs(Floor(MaxX) - Floor(MinX)) < 1.0f) return false;
    if (Abs(Floor(MaxY) - Floor(MinY)) < 1.0f) return false;
    
    return true;
}

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

void Draw_TriangleTexturedVerts(bitmap Target, const bitmap Texture, vertex A, vertex B, vertex C)
{
    vertex Vertices[6] = { A, B, C };
    u32 TriangleCount = Draw__TriangleClipZ(Vertices);
    for (u32 i = 0; i < TriangleCount; ++i)
    {
        vertex *V = Vertices + (i * 3);
        V[0].Position = Draw__PerspectiveDivide(Target.Width, Target.Height, V[0].Position);
        V[1].Position = Draw__PerspectiveDivide(Target.Width, Target.Height, V[1].Position);
        V[2].Position = Draw__PerspectiveDivide(Target.Width, Target.Height, V[2].Position);

        if (!Draw__PrepareTriangleVerts(Target.Width, Target.Height, &V[0], &V[1], &V[2])) return;
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

//
//
//

#define RASTERIZER_TILE_COUNT 4
#define RASTERIZER_TILE_WIDTH (SCREEN_WIDTH / RASTERIZER_TILE_COUNT)
#define RASTERIZER_TILE_HEIGHT SCREEN_HEIGHT

typedef struct rasterizer_tile
{
    bitmap Texture;

    vec4 Clip;
    vec3 Frustum[4];

    vertex Triangles[3][8];
    u32 TriangleCount;

    event Start;
    event Finish;

    u8 ColorBuffer[RASTERIZER_TILE_WIDTH * RASTERIZER_TILE_HEIGHT];
    f32 DepthBuffer[RASTERIZER_TILE_WIDTH * RASTERIZER_TILE_HEIGHT];
} rasterizer_tile;

typedef struct rasterizer_draw_call
{
    const vertex *Quads;
    u32 QuadCount;
    bitmap Texture;
    mat4 Transform;
    box BoundingBox;
} rasterizer_draw_call;

#define RASTERIZER_MAX_DRAWCALLS 4096
typedef struct rasterizer
{
    u32 DrawCallCount;
    rasterizer_draw_call DrawCalls[RASTERIZER_MAX_DRAWCALLS];

    rasterizer_tile Tiles[RASTERIZER_TILE_COUNT];
} rasterizer;

global rasterizer GlobalRasterizer;

static void Rasterizer__Rasterize(rasterizer_tile *Tile)
{
    const u8 *Texture = (u8 *)Tile->Texture.Pixels;
    u8 *ColorBuffer = Tile->ColorBuffer;
    f32 *DepthBuffer = Tile->DepthBuffer;

    if (!Tile->TriangleCount) return;

    i32 OffsetX = F32_FloorToI32(Tile->Clip.E[0]);
    i32 OffsetY = F32_FloorToI32(Tile->Clip.E[1]);
    __m256 ClipMinX = _mm256_broadcast_ss(&Tile->Clip.E[0]);
    __m256 ClipMinY = _mm256_broadcast_ss(&Tile->Clip.E[1]);
    __m256 ClipMaxX = _mm256_broadcast_ss(&Tile->Clip.E[2]);
    __m256 ClipMaxY = _mm256_broadcast_ss(&Tile->Clip.E[3]);

    u32 TilePitch = RASTERIZER_TILE_WIDTH;
    __m256 ScreenWidth = _mm256_set1_ps(SCREEN_WIDTH);
    __m256 ScreenHeight = _mm256_set1_ps(SCREEN_HEIGHT);

    _mm_setcsr(_mm_getcsr() | 0x8040);
#define PACK_WIDTH_SHIFT 2
#define PACK_HEIGHT_SHIFT 1
#define PACK_WIDTH (1 << PACK_WIDTH_SHIFT)
#define PACK_HEIGHT (1 << PACK_HEIGHT_SHIFT)
#define PACK_WIDTH_MASK (PACK_WIDTH - 1)
#define PACK_HEIGHT_MASK (PACK_HEIGHT - 1)
#define PACK_SHIFT (PACK_WIDTH_SHIFT + PACK_HEIGHT_SHIFT)
#define PACK_SIZE (1 << PACK_SHIFT)

    __m256 ColOffset = _mm256_setr_ps(0 & PACK_WIDTH_MASK, 1 & PACK_WIDTH_MASK, 
                                      2 & PACK_WIDTH_MASK, 3 & PACK_WIDTH_MASK, 
                                      4 & PACK_WIDTH_MASK, 5 & PACK_WIDTH_MASK,
                                      6 & PACK_WIDTH_MASK, 7 & PACK_WIDTH_MASK);
    __m256 RowOffset = _mm256_setr_ps(0 >> PACK_WIDTH_SHIFT, 1 >> PACK_WIDTH_SHIFT,
                                      2 >> PACK_WIDTH_SHIFT, 3 >> PACK_WIDTH_SHIFT,
                                      4 >> PACK_WIDTH_SHIFT, 5 >> PACK_WIDTH_SHIFT,
                                      6 >> PACK_WIDTH_SHIFT, 7 >> PACK_WIDTH_SHIFT);
    __m256 One = _mm256_set1_ps(1);
    __m256 Half = _mm256_set1_ps(0.5f);
    __m256 PackWidth8 = _mm256_set1_ps(PACK_WIDTH);
    __m256 PackHeight8 = _mm256_set1_ps(PACK_HEIGHT);

    __m256i VMul = _mm256_set1_epi32(Tile->Texture.Pitch);
    __m256i UMask = _mm256_set1_epi32(Tile->Texture.Width - 1);
    __m256i VMask = _mm256_set1_epi32(Tile->Texture.Height - 1);
    __m256i SMask = _mm256_set1_epi32(7);
    __m256 SSize  = _mm256_set1_ps(7);

    __m256 Bayer[4] = {
        _mm256_set_ps( 0.0f/16.0f, 8.0f/16.0f, 2.0f/16.0f,10.0f/16.0f,
                      12.0f/16.0f, 4.0f/16.0f,14.0f/16.0f, 6.0f/16.0f),
        _mm256_set_ps( 3.0f/16.0f,11.0f/16.0f, 1.0f/16.0f, 9.0f/16.0f,
                      15.0f/16.0f, 7.0f/16.0f,13.0f/16.0f, 5.0f/16.0f),
    };

    {
        __m256 X[3], Y[3], Z[3], U[3], V[3], S[3];

        for (u32 vertex = 0; vertex < 3; ++vertex)
        {
            __m256 Tmp[8];
            __m256 TTmp[8];

            Tmp[0] = _mm256_loadu_ps((const float *)&Tile->Triangles[vertex][0]);
            Tmp[1] = _mm256_loadu_ps((const float *)&Tile->Triangles[vertex][1]);
            Tmp[2] = _mm256_loadu_ps((const float *)&Tile->Triangles[vertex][2]);
            Tmp[3] = _mm256_loadu_ps((const float *)&Tile->Triangles[vertex][3]);
            Tmp[4] = _mm256_loadu_ps((const float *)&Tile->Triangles[vertex][4]);
            Tmp[5] = _mm256_loadu_ps((const float *)&Tile->Triangles[vertex][5]);
            Tmp[6] = _mm256_loadu_ps((const float *)&Tile->Triangles[vertex][6]);
            Tmp[7] = _mm256_loadu_ps((const float *)&Tile->Triangles[vertex][7]);

            TTmp[0] = _mm256_unpacklo_ps(Tmp[0], Tmp[1]);
            TTmp[1] = _mm256_unpackhi_ps(Tmp[0], Tmp[1]);
            TTmp[2] = _mm256_unpacklo_ps(Tmp[2], Tmp[3]);
            TTmp[3] = _mm256_unpackhi_ps(Tmp[2], Tmp[3]);
            TTmp[4] = _mm256_unpacklo_ps(Tmp[4], Tmp[5]);
            TTmp[5] = _mm256_unpackhi_ps(Tmp[4], Tmp[5]);
            TTmp[6] = _mm256_unpacklo_ps(Tmp[6], Tmp[7]);
            TTmp[7] = _mm256_unpackhi_ps(Tmp[6], Tmp[7]);

            Tmp[0] = _mm256_shuffle_ps(TTmp[0], TTmp[2], _MM_SHUFFLE(1,0,1,0));
            Tmp[1] = _mm256_shuffle_ps(TTmp[0], TTmp[2], _MM_SHUFFLE(3,2,3,2));
            Tmp[2] = _mm256_shuffle_ps(TTmp[1], TTmp[3], _MM_SHUFFLE(1,0,1,0));
            // Tmp[3] = _mm256_shuffle_ps(TTmp[1], TTmp[3], _MM_SHUFFLE(3,2,3,2));
            Tmp[4] = _mm256_shuffle_ps(TTmp[4], TTmp[6], _MM_SHUFFLE(1,0,1,0));
            Tmp[5] = _mm256_shuffle_ps(TTmp[4], TTmp[6], _MM_SHUFFLE(3,2,3,2));
            Tmp[6] = _mm256_shuffle_ps(TTmp[5], TTmp[7], _MM_SHUFFLE(1,0,1,0));
            // Tmp[7] = _mm256_shuffle_ps(TTmp[5], TTmp[7], _MM_SHUFFLE(3,2,3,2));

            X[vertex] = _mm256_permute2f128_ps(Tmp[0], Tmp[4], 0x20);
            Y[vertex] = _mm256_permute2f128_ps(Tmp[1], Tmp[5], 0x20);
            Z[vertex] = _mm256_permute2f128_ps(Tmp[2], Tmp[6], 0x20);
            // __m256 _[vertex] = _mm256_permute2f128_ps(Tmp[3], Tmp[7], 0x20);
            U[vertex] = _mm256_permute2f128_ps(Tmp[0], Tmp[4], 0x31);
            V[vertex] = _mm256_permute2f128_ps(Tmp[1], Tmp[5], 0x31);
            S[vertex] = _mm256_permute2f128_ps(Tmp[2], Tmp[6], 0x31);
            // __m256 _[vertex] = _mm256_permute2f128_ps(Tmp[3], Tmp[7], 0x31);
        }
        
        Z[0] = _mm256_div_ps(One, Z[0]);
        Z[1] = _mm256_div_ps(One, Z[1]);
        Z[2] = _mm256_div_ps(One, Z[2]);
        __m256 A[3] = { Z[0], _mm256_setzero_ps(), _mm256_setzero_ps() };
        __m256 B[3] = { _mm256_setzero_ps(), Z[1], _mm256_setzero_ps() };
        
        // perspective divide
        __m256 XX = _mm256_mul_ps(ScreenWidth, Half);
        X[0] = _mm256_sub_ps(X[0], XX);
        X[1] = _mm256_sub_ps(X[1], XX);
        X[2] = _mm256_sub_ps(X[2], XX);
        X[0] = _mm256_mul_ps(X[0], Z[0]);
        X[1] = _mm256_mul_ps(X[1], Z[1]);
        X[2] = _mm256_mul_ps(X[2], Z[2]);
        XX = _mm256_sub_ps(XX, Half);
        X[0] = _mm256_add_ps(X[0], XX);
        X[1] = _mm256_add_ps(X[1], XX);
        X[2] = _mm256_add_ps(X[2], XX);

        __m256 YY = _mm256_mul_ps(ScreenHeight, Half);
        Y[0] = _mm256_sub_ps(Y[0], YY);
        Y[1] = _mm256_sub_ps(Y[1], YY);
        Y[2] = _mm256_sub_ps(Y[2], YY);
        Y[0] = _mm256_mul_ps(Y[0], Z[0]);
        Y[1] = _mm256_mul_ps(Y[1], Z[1]);
        Y[2] = _mm256_mul_ps(Y[2], Z[2]);
        YY = _mm256_sub_ps(YY, Half);
        Y[0] = _mm256_add_ps(Y[0], YY);
        Y[1] = _mm256_add_ps(Y[1], YY);
        Y[2] = _mm256_add_ps(Y[2], YY);

        // barycentric edge functions
        // FAB(x, y) = (A.y - B.y)x + (B.x - A.x)y + (A.x * B.yy - B.x * A.y) = 0
        // FBC(x, y) = (B.y - C.y)x + (C.x - B.x)y + (B.x * C.yy - C.x * B.y) = 0
        // FCA(x, y) = (C.y - A.y)x + (A.x - C.x)y + (C.x * A.yy - A.x * C.y) = 0

        __m256 F12_dx = _mm256_sub_ps(Y[1], Y[2]);
        __m256 F20_dx = _mm256_sub_ps(Y[2], Y[0]);
        __m256 F01_dx = _mm256_sub_ps(Y[0], Y[1]);

        __m256 F12_dy = _mm256_sub_ps(X[2], X[1]);
        __m256 F20_dy = _mm256_sub_ps(X[0], X[2]);
        __m256 F01_dy = _mm256_sub_ps(X[1], X[0]);

        __m256 F12_0 = _mm256_sub_ps(_mm256_mul_ps(X[1], Y[2]), _mm256_mul_ps(X[2], Y[1]));
        __m256 F20_0 = _mm256_sub_ps(_mm256_mul_ps(X[2], Y[0]), _mm256_mul_ps(X[0], Y[2]));
        __m256 F01_0 = _mm256_sub_ps(_mm256_mul_ps(X[0], Y[1]), _mm256_mul_ps(X[1], Y[0]));

        __m256 InvTriArea = _mm256_div_ps(One, _mm256_sub_ps(_mm256_mul_ps(F01_dy, F20_dx), _mm256_mul_ps(F20_dy, F01_dx)));
        Z[1] = _mm256_mul_ps(_mm256_sub_ps(Z[1], Z[0]), InvTriArea);
        Z[2] = _mm256_mul_ps(_mm256_sub_ps(Z[2], Z[0]), InvTriArea);
        A[1] = _mm256_mul_ps(_mm256_sub_ps(A[1], A[0]), InvTriArea);
        A[2] = _mm256_mul_ps(_mm256_sub_ps(A[2], A[0]), InvTriArea);
        B[1] = _mm256_mul_ps(_mm256_sub_ps(B[1], B[0]), InvTriArea);
        B[2] = _mm256_mul_ps(_mm256_sub_ps(B[2], B[0]), InvTriArea);

        __m256 MinX = _mm256_max_ps(_mm256_mul_ps(_mm256_floor_ps(_mm256_div_ps(_mm256_min_ps(_mm256_min_ps(X[0], X[1]), X[2]), PackWidth8)), PackWidth8), ClipMinX);
        __m256 MinY = _mm256_max_ps(_mm256_mul_ps(_mm256_floor_ps(_mm256_div_ps(_mm256_min_ps(_mm256_min_ps(Y[0], Y[1]), Y[2]), PackHeight8)), PackHeight8), ClipMinY);
        __m256 MaxX = _mm256_min_ps(_mm256_ceil_ps(_mm256_max_ps(_mm256_max_ps(X[0], X[1]), X[2])), ClipMaxX);
        __m256 MaxY = _mm256_min_ps(_mm256_ceil_ps(_mm256_max_ps(_mm256_max_ps(Y[0], Y[1]), Y[2])), ClipMaxY);

        for (u32 Tri = 0; Tri < Tile->TriangleCount; ++Tri)
        {
            __m256i Tri8 = _mm256_broadcastd_epi32(_mm_loadu_si32(&Tri));
            __m256 Z8[3] = { _mm256_permutevar8x32_ps(Z[0], Tri8),
                             _mm256_permutevar8x32_ps(Z[1], Tri8),
                             _mm256_permutevar8x32_ps(Z[2], Tri8) };
            __m256 A8[3] = { _mm256_permutevar8x32_ps(A[0], Tri8),
                             _mm256_permutevar8x32_ps(A[1], Tri8),
                             _mm256_permutevar8x32_ps(A[2], Tri8) };
            __m256 B8[3] = { _mm256_permutevar8x32_ps(B[0], Tri8),
                             _mm256_permutevar8x32_ps(B[1], Tri8),
                             _mm256_permutevar8x32_ps(B[2], Tri8) };
            __m256 S8[3] = { _mm256_permutevar8x32_ps(S[0], Tri8),
                             _mm256_permutevar8x32_ps(S[1], Tri8),
                             _mm256_permutevar8x32_ps(S[2], Tri8) };
            __m256 U8[3] = { _mm256_permutevar8x32_ps(U[0], Tri8),
                             _mm256_permutevar8x32_ps(U[1], Tri8),
                             _mm256_permutevar8x32_ps(U[2], Tri8) };
            __m256 V8[3] = { _mm256_permutevar8x32_ps(V[0], Tri8),
                             _mm256_permutevar8x32_ps(V[1], Tri8),
                             _mm256_permutevar8x32_ps(V[2], Tri8) };
        
            __m256 F12_dx8 = _mm256_permutevar8x32_ps(F12_dx, Tri8);
            __m256 F20_dx8 = _mm256_permutevar8x32_ps(F20_dx, Tri8);
            __m256 F01_dx8 = _mm256_permutevar8x32_ps(F01_dx, Tri8);

            __m256 F12_dy8 = _mm256_permutevar8x32_ps(F12_dy, Tri8);
            __m256 F20_dy8 = _mm256_permutevar8x32_ps(F20_dy, Tri8);
            __m256 F01_dy8 = _mm256_permutevar8x32_ps(F01_dy, Tri8);

            __m256 Col = _mm256_add_ps(ColOffset, _mm256_permutevar8x32_ps(MinX, Tri8));
            __m256 A12_0 = _mm256_mul_ps(F12_dx8, Col);
            __m256 A20_0 = _mm256_mul_ps(F20_dx8, Col);
            __m256 A01_0 = _mm256_mul_ps(F01_dx8, Col);

            __m256 Row = _mm256_add_ps(RowOffset, _mm256_permutevar8x32_ps(MinY, Tri8));
            __m256 B12_0 = _mm256_mul_ps(F12_dy8, Row);
            __m256 B20_0 = _mm256_mul_ps(F20_dy8, Row);
            __m256 B01_0 = _mm256_mul_ps(F01_dy8, Row);

            __m256 F12_Row = _mm256_add_ps(_mm256_add_ps(A12_0, B12_0), _mm256_permutevar8x32_ps(F12_0, Tri8));
            __m256 F20_Row = _mm256_add_ps(_mm256_add_ps(A20_0, B20_0), _mm256_permutevar8x32_ps(F20_0, Tri8));
            __m256 F01_Row = _mm256_add_ps(_mm256_add_ps(A01_0, B01_0), _mm256_permutevar8x32_ps(F01_0, Tri8));

            F12_dx8 = _mm256_mul_ps(F12_dx8, PackWidth8);
            F20_dx8 = _mm256_mul_ps(F20_dx8, PackWidth8);
            F01_dx8 = _mm256_mul_ps(F01_dx8, PackWidth8);

            F12_dy8 = _mm256_mul_ps(F12_dy8, PackHeight8);
            F20_dy8 = _mm256_mul_ps(F20_dy8, PackHeight8);
            F01_dy8 = _mm256_mul_ps(F01_dy8, PackHeight8);

            __m256 ZZ_dx = _mm256_add_ps(_mm256_mul_ps(F20_dx8, Z8[1]), _mm256_mul_ps(F01_dx8, Z8[2]));
            __m256 AA_dx = _mm256_add_ps(_mm256_mul_ps(F20_dx8, A8[1]), _mm256_mul_ps(F01_dx8, A8[2]));
            __m256 BB_dx = _mm256_add_ps(_mm256_mul_ps(F20_dx8, B8[1]), _mm256_mul_ps(F01_dx8, B8[2]));

            i32 iMinX = _mm256_cvtsi256_si32(_mm256_cvtps_epi32(_mm256_permutevar8x32_ps(MinX, Tri8)));
            i32 iMinY = _mm256_cvtsi256_si32(_mm256_cvtps_epi32(_mm256_permutevar8x32_ps(MinY, Tri8)));
            i32 iMaxX = _mm256_cvtsi256_si32(_mm256_cvtps_epi32(_mm256_permutevar8x32_ps(MaxX, Tri8)));
            i32 iMaxY = _mm256_cvtsi256_si32(_mm256_cvtps_epi32(_mm256_permutevar8x32_ps(MaxY, Tri8)));

            i32 PixelX = (iMinX - OffsetX);
            i32 PixelY = (iMinY - OffsetY);
            i32 RowPixel = PixelY * TilePitch + PixelX;
            i32 RowDepthPixel = PixelY * TilePitch + PixelX * PACK_HEIGHT;

            for(i32 y = iMinY; y < iMaxY; y += PACK_HEIGHT)
            {
                i32 Pixel = RowPixel;
                i32 DepthPixel = RowDepthPixel;
                __m256 Alpha = F12_Row;
                __m256 Beta  = F20_Row;
                __m256 Gamma = F01_Row;

                __m256 ZZ = _mm256_add_ps(_mm256_add_ps(Z8[0], _mm256_mul_ps(Beta, Z8[1])), _mm256_mul_ps(Gamma, Z8[2]));
                __m256 AA = _mm256_add_ps(_mm256_add_ps(A8[0], _mm256_mul_ps(Beta, A8[1])), _mm256_mul_ps(Gamma, A8[2]));
                __m256 BB = _mm256_add_ps(_mm256_add_ps(B8[0], _mm256_mul_ps(Beta, B8[1])), _mm256_mul_ps(Gamma, B8[2]));

                __m256 Dither = Bayer[(y>>1)&1];

                for(i32 x = iMinX; x < iMaxX; x += PACK_WIDTH)
                {
                    f32 *DepthPtr = DepthBuffer + DepthPixel;
                    __m256 Depth = _mm256_loadu_ps(DepthPtr);
                    __m256i DepthMask = _mm256_castps_si256(_mm256_cmp_ps(ZZ, Depth, _CMP_LE_OQ));   
                    __m256i EdgeMask = _mm256_srai_epi32(_mm256_castps_si256(_mm256_or_ps(_mm256_or_ps(Alpha, Beta), Gamma)), 32);
                    __m256i Mask = _mm256_or_si256(DepthMask, EdgeMask);
                    if (!_mm256_test_all_ones(Mask))
                    {
                        // interpolation factors
                        __m256 ZZZ = _mm256_div_ps(One, ZZ);
                        __m256 AAA = _mm256_mul_ps(AA, ZZZ);
                        __m256 BBB = _mm256_mul_ps(BB, ZZZ);
                        __m256 CCC = _mm256_sub_ps(One, _mm256_add_ps(AAA, BBB));

                        // interpolate
                        __m256 UUU = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(U8[0], AAA), _mm256_mul_ps(U8[1], BBB)), _mm256_mul_ps(U8[2], CCC));
                        __m256 VVV = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(V8[0], AAA), _mm256_mul_ps(V8[1], BBB)), _mm256_mul_ps(V8[2], CCC));
                        __m256 SSS = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(S8[0], AAA), _mm256_mul_ps(S8[1], BBB)), _mm256_mul_ps(S8[2], CCC));

                        SSS = _mm256_add_ps(_mm256_mul_ps(SSS, SSize), Dither);

                        __m256i iUUU = _mm256_and_si256(_mm256_cvttps_epi32(UUU), UMask);
                        __m256i iVVV = _mm256_and_si256(_mm256_cvttps_epi32(VVV), VMask);
                        __m256i iSSS = _mm256_and_si256(_mm256_cvttps_epi32(SSS), SMask);

                        __m256i iUU8 = _mm256_add_epi32(iUUU, _mm256_mullo_epi32(VMul, iVVV));

                        // gather
                        u8 *Dst0 = ColorBuffer + Pixel;
                        u8 *Dst1 = Dst0 + TilePitch;
                        __m256i Dst = _mm256_set_m128i(_mm_loadu_si32(Dst1), _mm_loadu_si32(Dst0));
                        Dst = _mm256_shuffle_epi8(Dst, _mm256_set_epi8(3,3,3,3,2,2,2,2,1,1,1,1,0,0,0,0,3,3,3,3,2,2,2,2,1,1,1,1,0,0,0,0));

                        // shade
                        __m256i Tex = _mm256_i32gather_epi32((const int *)Texture, iUU8, 1);
                        Tex = _mm256_and_si256(Tex, _mm256_set1_epi32(0xF8));
                        Mask = _mm256_or_si256(Mask, _mm256_cmpeq_epi32(Tex, _mm256_setzero_si256()));
                        Tex = _mm256_or_si256(Tex, iSSS);
                        Dst = _mm256_blendv_epi8(Tex, Dst, Mask);

                        // scatter
                        Dst = _mm256_shuffle_epi8(Dst, _mm256_set_epi8(0,0,0,0,0,0,0,0,0,0,0,0,12,8,4,0,0,0,0,0,0,0,0,0,0,0,0,0,12,8,4,0));
                        _mm_storeu_si32(Dst0, _mm256_castsi256_si128(Dst));
                        _mm_storeu_si32(Dst1, _mm256_extracti128_si256(Dst, 1));

                        // depth update
                        Depth = _mm256_blendv_ps(ZZ, Depth, _mm256_castsi256_ps(Mask));
                        _mm256_storeu_ps(DepthPtr, Depth);
                    }

                    Pixel += PACK_WIDTH;
                    DepthPixel += PACK_WIDTH * PACK_HEIGHT;
                    Alpha = _mm256_add_ps(Alpha, F12_dx8);
                    Beta  = _mm256_add_ps(Beta,  F20_dx8);
                    Gamma = _mm256_add_ps(Gamma, F01_dx8);
                    ZZ = _mm256_add_ps(ZZ, ZZ_dx);
                    AA = _mm256_add_ps(AA, AA_dx);
                    BB = _mm256_add_ps(BB, BB_dx);
                }

                RowPixel += PACK_HEIGHT * TilePitch;
                RowDepthPixel += PACK_HEIGHT * TilePitch;
                F12_Row = _mm256_add_ps(F12_Row, F12_dy8);
                F20_Row = _mm256_add_ps(F20_Row, F20_dy8);
                F01_Row = _mm256_add_ps(F01_Row, F01_dy8);
            }
        }
    }
}

static bool Rasterizer__QuadIsVisible(const rasterizer_tile *Tile, vertex A, vertex B, vertex C, vertex D)
{
    __m128i Sign = _mm_set1_epi32(0x80000000);
    __m128 Near = _mm_set1_ps(CAMERA_NEAR);
    __m128 Half = _mm_set1_ps(0.5f);

    __m128 mA = _mm_loadu_ps(A.Position.E);
    __m128 mB = _mm_loadu_ps(B.Position.E);
    __m128 mC = _mm_loadu_ps(C.Position.E);
    __m128 mD = _mm_loadu_ps(D.Position.E);

    __m128 T0 = _mm_unpacklo_ps(mA, mB);
    __m128 T1 = _mm_unpackhi_ps(mA, mB);
    __m128 T2 = _mm_unpacklo_ps(mC, mD);
    __m128 T3 = _mm_unpackhi_ps(mC, mD);

    __m128 X = _mm_movelh_ps(T0, T2);
    __m128 Y = _mm_movehl_ps(T2, T0);
    __m128 Z = _mm_movelh_ps(T1, T3);

    // quad in front of near plane
    __m128i M = _mm_castps_si128(_mm_cmpgt_ps(Z, Near));
    if (_mm_testz_si128(M, M))
        return false;

    // quad outside of view frustum
    X = _mm_sub_ps(X, _mm_set1_ps(SCREEN_WIDTH / 2));
    Y = _mm_sub_ps(Y, _mm_set1_ps(SCREEN_HEIGHT / 2));
    for (u32 i = 0; i < 4; ++i)
    {
        __m128 N = _mm_loadu_ps(Tile->Frustum[i].E);
        __m128 P = _mm_mul_ps(X, _mm_shuffle_ps(N, N, _MM_SHUFFLE(0,0,0,0)));
        P = _mm_add_ps(P, _mm_mul_ps(Y, _mm_shuffle_ps(N, N, _MM_SHUFFLE(1,1,1,1))));
        P = _mm_add_ps(P, _mm_mul_ps(Z, _mm_shuffle_ps(N, N, _MM_SHUFFLE(2,2,2,2))));
        if (_mm_testz_si128(_mm_castps_si128(P), Sign))
            return false;
    }

    // clipped
    __m128 ZSign = _mm_and_ps(Z, _mm_castsi128_ps(Sign));
    if (!_mm_testz_si128(_mm_castps_si128(ZSign), _mm_castps_si128(ZSign)))
        return true;

    // quad between pixels
    X = _mm_div_ps(X, Z);
    X = _mm_sub_ps(X, Half);
    __m128 X0 = _mm_movehdup_ps(X);
    __m128 X1 = _mm_max_ps(X, X0);
    X0 = _mm_movehl_ps(X0, X1);
    X1 = _mm_max_ss(X1, X0);
    f32 MaxX = _mm_cvtss_f32(X1);
    X0 = _mm_movehdup_ps(X);
    X1 = _mm_min_ps(X, X0);
    X0 = _mm_movehl_ps(X0, X1);
    X1 = _mm_min_ss(X1, X0);
    f32 MinX = _mm_cvtss_f32(X1);
    if (Floor(MinX) == Floor(MaxX))
        return false;

    // quad between pixels
    Y = _mm_div_ps(Y, Z);
    Y = _mm_sub_ps(Y, Half);
    __m128 Y0 = _mm_movehdup_ps(Y);
    __m128 Y1 = _mm_max_ps(Y, Y0);
    Y0 = _mm_movehl_ps(Y0, Y1);
    Y1 = _mm_max_ss(Y1, Y0);
    f32 MaxY = _mm_cvtss_f32(Y1);
    Y0 = _mm_movehdup_ps(Y);
    Y1 = _mm_min_ps(Y, Y0);
    Y0 = _mm_movehl_ps(Y0, Y1);
    Y1 = _mm_min_ss(Y1, Y0);
    f32 MinY = _mm_cvtss_f32(Y1);
    if (Floor(MinX) == Floor(MaxX))
        return false;

    // check winding order
    X = _mm_xor_ps(X, ZSign);
    Y = _mm_xor_ps(Y, ZSign);
    __m128 AAAA = _mm_shuffle_ps(X, Y, _MM_SHUFFLE(0,0,0,0));
    __m128 BCCB = _mm_shuffle_ps(X, Y, _MM_SHUFFLE(1,2,2,1));
    __m128 C0 = _mm_sub_ps(BCCB, AAAA);
    __m128 C1 = _mm_movehl_ps(C0, C0);
    C0 = _mm_mul_ps(C0, C1);
    C1 = _mm_movehdup_ps(C0);
    C0 = _mm_sub_ps(C0, C1);
    if (!_mm_testz_si128(_mm_castps_si128(C0), _mm_set_epi32(0,0,0,0x80000000)))
        return false;

    return true;
}

static void Rasterizer__Flush(rasterizer_tile *Tile)
{
    Rasterizer__Rasterize(Tile);
    Tile->TriangleCount = 0;
}

static void Rasterizer__DrawTriangle(rasterizer_tile *Tile, vertex A, vertex B, vertex C)
{
    vertex Vertices[6] = { A, B, C };
    u32 TriangleCount = Draw__TriangleClipZ(Vertices);
    for (u32 i = 0; i < TriangleCount; ++i)
    {
        vertex *V = Vertices + (i * 3);
        Tile->Triangles[0][Tile->TriangleCount] = V[0];
        Tile->Triangles[1][Tile->TriangleCount] = V[1];
        Tile->Triangles[2][Tile->TriangleCount] = V[2];
        if (++Tile->TriangleCount == 8)
            Rasterizer__Flush(Tile);
    }
}

static void Rasterizer__DrawCall(rasterizer_tile *Tile, const rasterizer_draw_call *DrawCall)
{
    // set texture
    Tile->Texture = DrawCall->Texture;

    // load transform
    __m128 C0 = LOAD_VEC4(DrawCall->Transform.Columns[0]);
    __m128 C1 = LOAD_VEC4(DrawCall->Transform.Columns[1]);
    __m128 C2 = LOAD_VEC4(DrawCall->Transform.Columns[2]);
    __m128 C3 = LOAD_VEC4(DrawCall->Transform.Columns[3]);
    C3 = _mm_and_ps(C3, _mm_castsi128_ps(_mm_set_epi32(0,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF)));
    
    // cull bounding box
    __m256i Sign = _mm256_set1_epi32(0x80000000);
    __m128 BoxMin = _mm_loadu_ps(DrawCall->BoundingBox.Min.E);
    __m128 BoxMax = _mm_loadu_ps(DrawCall->BoundingBox.Max.E);
    __m128 Box[8] = {
        _mm_blend_ps(BoxMin, BoxMax, 0),
        _mm_blend_ps(BoxMin, BoxMax, 1),
        _mm_blend_ps(BoxMin, BoxMax, 2),
        _mm_blend_ps(BoxMin, BoxMax, 3),
        _mm_blend_ps(BoxMin, BoxMax, 4),
        _mm_blend_ps(BoxMin, BoxMax, 5),
        _mm_blend_ps(BoxMin, BoxMax, 6),
        _mm_blend_ps(BoxMin, BoxMax, 7),
    };
    for (u32 i = 0; i < 8; ++i)
    {
        __m128 P = Box[i];
        __m128 P0 = _mm_mul_ps(C0, _mm_shuffle_ps(P, P, _MM_SHUFFLE(0,0,0,0)));
        __m128 P1 = _mm_mul_ps(C1, _mm_shuffle_ps(P, P, _MM_SHUFFLE(1,1,1,1)));
        __m128 P2 = _mm_mul_ps(C2, _mm_shuffle_ps(P, P, _MM_SHUFFLE(2,2,2,2)));
        P = _mm_add_ps(_mm_add_ps(P0, P1), _mm_add_ps(P2, C3));
        Box[i] = P;
    }
    __m256 T0 = _mm256_unpacklo_ps(_mm256_set_m128(Box[0], Box[0]), _mm256_set_m128(Box[1], Box[1]));
    __m256 T1 = _mm256_unpackhi_ps(_mm256_set_m128(Box[0], Box[0]), _mm256_set_m128(Box[1], Box[1]));
    __m256 T2 = _mm256_unpacklo_ps(_mm256_set_m128(Box[2], Box[2]), _mm256_set_m128(Box[3], Box[3]));
    __m256 T3 = _mm256_unpackhi_ps(_mm256_set_m128(Box[2], Box[2]), _mm256_set_m128(Box[3], Box[3]));
    __m256 T4 = _mm256_unpacklo_ps(_mm256_set_m128(Box[4], Box[4]), _mm256_set_m128(Box[5], Box[5]));
    __m256 T5 = _mm256_unpackhi_ps(_mm256_set_m128(Box[4], Box[4]), _mm256_set_m128(Box[5], Box[5]));
    __m256 T6 = _mm256_unpacklo_ps(_mm256_set_m128(Box[6], Box[6]), _mm256_set_m128(Box[7], Box[7]));
    __m256 T7 = _mm256_unpackhi_ps(_mm256_set_m128(Box[6], Box[6]), _mm256_set_m128(Box[7], Box[7]));
    __m256 X = _mm256_permute2f128_ps(_mm256_shuffle_ps(T0, T2, _MM_SHUFFLE(1,0,1,0)),
                                      _mm256_shuffle_ps(T4, T6, _MM_SHUFFLE(1,0,1,0)), 0x20);
    __m256 Y = _mm256_permute2f128_ps(_mm256_shuffle_ps(T0, T2, _MM_SHUFFLE(3,2,3,2)),
                                      _mm256_shuffle_ps(T4, T6, _MM_SHUFFLE(3,2,3,2)), 0x20);
    __m256 Z = _mm256_permute2f128_ps(_mm256_shuffle_ps(T1, T3, _MM_SHUFFLE(1,0,1,0)),
                                      _mm256_shuffle_ps(T5, T7, _MM_SHUFFLE(1,0,1,0)), 0x20);
    X = _mm256_sub_ps(X, _mm256_set1_ps(SCREEN_WIDTH / 2));
    Y = _mm256_sub_ps(Y, _mm256_set1_ps(SCREEN_HEIGHT / 2));
    for (u32 i = 0; i < 4; ++i)
    {
        __m256 P =           _mm256_mul_ps(X, _mm256_broadcast_ss(&Tile->Frustum[i].E[0]));
        P = _mm256_add_ps(P, _mm256_mul_ps(Y, _mm256_broadcast_ss(&Tile->Frustum[i].E[1])));
        P = _mm256_add_ps(P, _mm256_mul_ps(Z, _mm256_broadcast_ss(&Tile->Frustum[i].E[2])));
        if (_mm256_testz_si256(_mm256_castps_si256(P), Sign))
            return;
    }

    // rasterize
    for (u32 i = 0; i < DrawCall->QuadCount; i++)
    {
        vertex V[4] = {
            DrawCall->Quads[i * 4 + 0],
            DrawCall->Quads[i * 4 + 1],
            DrawCall->Quads[i * 4 + 2],
            DrawCall->Quads[i * 4 + 3],
        };

        // transform
        for (u32 j = 0; j < 4; ++j)
        {
            __m128 P = LOAD_VEC3(V[j].Position);
            __m128 P0 = _mm_mul_ps(C0, _mm_shuffle_ps(P, P, _MM_SHUFFLE(0,0,0,0)));
            __m128 P1 = _mm_mul_ps(C1, _mm_shuffle_ps(P, P, _MM_SHUFFLE(1,1,1,1)));
            __m128 P2 = _mm_mul_ps(C2, _mm_shuffle_ps(P, P, _MM_SHUFFLE(2,2,2,2)));
            P = _mm_add_ps(_mm_add_ps(P0, P1), _mm_add_ps(P2, C3));
            STORE_VEC3(V[j].Position, P);
        }
        
        // cull quad
        if (!Rasterizer__QuadIsVisible(Tile, V[0], V[1], V[2], V[3]))
            continue;

        // draw
        Rasterizer__DrawTriangle(Tile, V[0], V[1], V[2]);
        Rasterizer__DrawTriangle(Tile, V[2], V[3], V[0]);
    }

    Rasterizer__Flush(Tile);
}

DWORD Rasterizer_TileThreadProc(void *Parameter)
{
    u64 TileId = (u64)Parameter;
    rasterizer *Rasterizer = &GlobalRasterizer;
    rasterizer_tile *Tile = &Rasterizer->Tiles[TileId];
    for (;;)
    {
        Event_Wait(Tile->Start);
        for (u32 i = 0; i < Rasterizer->DrawCallCount; ++i)
            Rasterizer__DrawCall(Tile, &Rasterizer->DrawCalls[i]);
        Event_Signal(Tile->Finish);
    }
}

void Rasterizer_Init(void)
{
    rasterizer *Rasterizer = &GlobalRasterizer;

    for (u32 i = 0; i < RASTERIZER_TILE_COUNT; ++i)
    {
        rasterizer_tile *Tile = &Rasterizer->Tiles[i];
    
        f32 T0 = (f32)i;
        f32 T1 = (f32)i + 1;
        Tile->Start = Event_Create();
        Tile->Finish = Event_Create();
        Tile->Clip = (vec4){ RASTERIZER_TILE_WIDTH * T0, 0, RASTERIZER_TILE_WIDTH * T1, RASTERIZER_TILE_HEIGHT };
        Tile->Frustum[0] = (vec3){-1, 0, -(SCREEN_WIDTH / 2) + (RASTERIZER_TILE_WIDTH * (T0 + 0.0f)) };
        Tile->Frustum[1] = (vec3){ 1, 0,  (SCREEN_WIDTH / 2) - (RASTERIZER_TILE_WIDTH * (T1 - 0.0f)) };
        Tile->Frustum[2] = (vec3){ 0,-1, 0 - (SCREEN_HEIGHT / 2) };
        Tile->Frustum[3] = (vec3){ 0, 1, 0 - (SCREEN_HEIGHT / 2) };
    }
}

void Rasterizer_Clear(color Color)
{
    rasterizer *Rasterizer = &GlobalRasterizer;

    u32 PixelCount = RASTERIZER_TILE_WIDTH * RASTERIZER_TILE_HEIGHT;
    for (u32 i = 0; i < RASTERIZER_TILE_COUNT; ++i)
    {
        memset(Rasterizer->Tiles[i].ColorBuffer, Color.Value, PixelCount * sizeof(u8));
        memset(Rasterizer->Tiles[i].DepthBuffer, 0, PixelCount * sizeof(f32));
    }
}

void Rasterizer_Rasterize(void)
{
    rasterizer *Rasterizer = &GlobalRasterizer;

    event TilesFinished[RASTERIZER_TILE_COUNT-1];
    for (u32 i = 1; i < RASTERIZER_TILE_COUNT; ++i)
    {
        rasterizer_tile *Tile = &Rasterizer->Tiles[i];
        TilesFinished[i-1] = Tile->Finish;
        Event_Signal(Tile->Start);
    }

    rasterizer_tile *Tile = &Rasterizer->Tiles[0];
    for (u32 i = 0; i < Rasterizer->DrawCallCount; ++i)
        Rasterizer__DrawCall(Tile, &Rasterizer->DrawCalls[i]);
    
    Event_WaitAll(TilesFinished, RASTERIZER_TILE_COUNT-1);

    Rasterizer->DrawCallCount = 0;
}

void Rasterizer_Blit(bitmap Target)
{
    rasterizer *Rasterizer = &GlobalRasterizer;
    
    for (u32 i = 0; i < RASTERIZER_TILE_COUNT; ++i)
    {
        rasterizer_tile *Tile = &Rasterizer->Tiles[i];

        i32 MinX = F32_CeilToI32(Tile->Clip.E[0]);
        i32 MinY = F32_CeilToI32(Tile->Clip.E[1]);
        i32 MaxX = F32_FloorToI32(Tile->Clip.E[2]);
        i32 MaxY = F32_FloorToI32(Tile->Clip.E[3]);
        i32 Width = MaxX - MinX;
        i32 Height = MaxY - MinY;

        u8 *SrcRow = Tile->ColorBuffer;
        u8 *DstRow = &Target.Pixels->Value + MinY * Target.Pitch + MinX;

        for (i32 y = 0; y < Height; ++y)
        {
            u8 *Src = SrcRow;
            u8 *Dst = DstRow;
            for (i32 x = 0; x < Width; ++x)
                *Dst++ = *Src++;
            SrcRow += RASTERIZER_TILE_WIDTH;
            DstRow += Target.Pitch;
        }
    }
}

void Rasterizer_DrawMesh(const vertex *Quads, u32 QuadCount, const bitmap Texture, mat4 Transform, box BoundingBox)
{
    rasterizer *Rasterizer = &GlobalRasterizer;
    if (Rasterizer->DrawCallCount < RASTERIZER_MAX_DRAWCALLS)
        Rasterizer->DrawCalls[Rasterizer->DrawCallCount++] = (rasterizer_draw_call){
            .Quads = Quads,
            .QuadCount = QuadCount,
            .Texture = Texture,
            .Transform = Transform,
            .BoundingBox = BoundingBox,
        };
}
