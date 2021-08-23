
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

enum bitmap_flags
{
    BITMAP_ONHEAP,
    BITMAP_HASDEPTH
};

typedef struct bitmap
{
    u32 Width, Height;
    u32 Pitch;
    u32 Flags;
    color *Pixels;
} bitmap;

typedef struct vertex
{
    vec3 Position;
    vec2 TexCoord;
    f32 Shadow;
} vertex;

bitmap Bitmap_Create(u32 Width, u32 Height);
bitmap Bitmap_Section(bitmap Bitmap, i32 X, i32 Y, u32 W, u32 H);
color Bitmap_GetPixel(bitmap Buffer, i32 X, i32 Y);
void Bitmap_SetPixel(bitmap Buffer, color Color, i32 X, i32 Y);
void Bitmap_Clear(bitmap Buffer, color Color);

void Draw_PointStruct(bitmap Target, color Color, point Point);
void Draw_LineStruct(bitmap Target, color Color, line Line);
void Draw_TriangleStruct(bitmap Target, color Color, triangle Triangle);
void Draw_RectStruct(bitmap Target, color Color, rect Rect);

void Draw_PointInt(bitmap Target, color Color, i32 X, i32 Y);
void Draw_LineInt(bitmap Target, color Color, i32 X0, i32 Y0, i32 X1, i32 Y1);
void Draw_TriangleInt(bitmap Target, color Color, i32 X0, i32 Y0, i32 X1, i32 Y1, i32 X2, i32 Y2);
void Draw_RectInt(bitmap Target, color Color, i32 X, i32 Y, i32 W, i32 H);

void Draw_Bitmap(bitmap Target, bitmap Bitmap, i32 X, i32 Y);
void Draw_String(bitmap Target, bitmap Font, color Color, i32 X, i32 Y, const char *String);

void Draw_TriangleVerts(bitmap Target, color Color, vertex A, vertex B, vertex C);
void Draw_TriangleTexturedVerts(bitmap Target, bitmap Texture, vertex A, vertex B, vertex C);

void Draw_QuadVerts(bitmap Target, color Color, vertex A, vertex B, vertex C, vertex D);
void Draw_QuadTexturedVerts(bitmap Target, bitmap Texture, vertex A, vertex B, vertex C, vertex D);


#define Draw_Point(BUFFER, COLOR, X, ...) _Generic((X), \
    point: Draw_PointStruct, \
    i8: Draw_PointInt, i16: Draw_PointInt, i32: Draw_PointInt, i64: Draw_PointInt, \
    u8: Draw_PointInt, u16: Draw_PointInt, u32: Draw_PointInt, u64: Draw_PointInt  \
)(BUFFER, COLOR, X, __VA_ARGS__)

#define Draw_Line(BUFFER, COLOR, X, ...) _Generic((X), \
    line: Draw_LineStruct, \
    i8: Draw_LineInt, i16: Draw_LineInt, i32: Draw_LineInt, i64: Draw_LineInt, \
    u8: Draw_LineInt, u16: Draw_LineInt, u32: Draw_LineInt, u64: Draw_LineInt \
)(BUFFER, COLOR, X, __VA_ARGS__)


#define Draw_TriangleColor(BUFFER, COLOR, X, ...) _Generic((X), \
    vertex: Draw_TriangleVerts,\
    triangle: Draw_TriangleStruct, \
    i8: Draw_TriangleInt, i16: Draw_TriangleInt, i32: Draw_TriangleInt, i64: Draw_TriangleInt, \
    u8: Draw_TriangleInt, u16: Draw_TriangleInt, u32: Draw_TriangleInt, u64: Draw_TriangleInt \
)(BUFFER, COLOR, X, __VA_ARGS__)

#define Draw_TriangleTextured(BUFFER, COLOR, X, ...) _Generic((X), \
    vertex: Draw_TriangleTexturedVerts \
)(BUFFER, COLOR, X, __VA_ARGS__)

#define Draw_Triangle(BUFFER, COLOR, X, ...) _Generic((COLOR), \
    color:  _Generic((X), \
        vertex: Draw_TriangleVerts,\
        triangle: Draw_TriangleStruct, \
        i8: Draw_TriangleInt, i16: Draw_TriangleInt, i32: Draw_TriangleInt, i64: Draw_TriangleInt, \
        u8: Draw_TriangleInt, u16: Draw_TriangleInt, u32: Draw_TriangleInt, u64: Draw_TriangleInt \
    ), \
    bitmap: _Generic((X), \
        vertex: Draw_TriangleTexturedVerts \
    ) \
)(BUFFER, COLOR, X, __VA_ARGS__)


#define Draw_QuadColor(BUFFER, COLOR, X, ...) _Generic((X), \
    vertex: Draw_QuadVerts \
)(BUFFER, COLOR, X, __VA_ARGS__)

#define Draw_QuadTextured(BUFFER, COLOR, X, ...) _Generic((X), \
    vertex: Draw_QuadTexturedVerts \
)(BUFFER, COLOR, X, __VA_ARGS__)

#define Draw_Quad(BUFFER, COLOR, X, ...) _Generic((COLOR), \
    color:  _Generic((X), \
        vertex: Draw_QuadVerts\
    ), \
    bitmap: _Generic((X), \
        vertex: Draw_QuadTexturedVerts \
    ) \
)(BUFFER, COLOR, X, __VA_ARGS__)


#define Draw_Rect(BUFFER, COLOR, X, ...) _Generic((X), \
    rect: Draw_RectStruct, \
    i8: Draw_RectInt, i16: Draw_RectInt, i32: Draw_RectInt, i64: Draw_RectInt, \
    u8: Draw_RectInt, u16: Draw_RectInt, u32: Draw_RectInt, u64: Draw_RectInt \
)(BUFFER, COLOR, X, __VA_ARGS__)



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

inline color Bitmap__GetPixelFast(bitmap Buffer, i32 X, i32 Y)
{
    return *(Buffer.Pixels + X + Y * Buffer.Pitch);
}

color Bitmap_GetPixel(bitmap Buffer, i32 X, i32 Y)
{
    if ((u32)X >= Buffer.Width)  return (color){ 0 };
    if ((u32)Y >= Buffer.Height) return (color){ 0 };
    return Bitmap__GetPixelFast(Buffer, X, Y);
}

inline void Bitmap__SetPixelFast(bitmap Buffer, color Color, i32 X, i32 Y)
{
    *(Buffer.Pixels + X + Y * Buffer.Pitch) = Color;
}

void Bitmap_SetPixel(bitmap Buffer, color Color, i32 X, i32 Y)
{
    if ((u32)X >= Buffer.Width)  return;
    if ((u32)Y >= Buffer.Height) return;
    Bitmap__SetPixelFast(Buffer, Color, X, Y);
}

void Bitmap_Clear(bitmap Buffer, color Color)
{
    for (u32 y = 0; y < Buffer.Height; ++y)
    for (u32 x = 0; x < Buffer.Width; ++x)
        Bitmap__SetPixelFast(Buffer, Color, x, y);
}



void Draw_PointStruct(bitmap Target, color Color, point Point)
{
    Bitmap_SetPixel(Target, Color, Floor_toInt(Point.x), Floor_toInt(Point.y));
}

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

void Draw_LineStruct(bitmap Target, color Color, line Line)
{
    rect Clip = (rect){ .x = 0.0f, .y = 0.0f, .w = (f32)Target.Width, .h = (f32)Target.Height };
    
    if (!Line_Clip(&Line, Clip)) return;
    
    Line.a.x -= 0.5f;
    Line.a.y -= 0.5f;
    Line.b.x -= 0.5f;
    Line.b.y -= 0.5f;

    {   // check if vertical or horizontal line
        const i32 ix0 = Ceil_toInt(Min(Line.a.x, Line.b.x));
        const i32 ix1 = Ceil_toInt(Max(Line.a.x, Line.b.x));
        const i32 iy0 = Ceil_toInt(Min(Line.a.y, Line.b.y));
        const i32 iy1 = Ceil_toInt(Max(Line.a.y, Line.b.y));
        if (ix0 == ix1) Draw__VerticalLine(Target, Color, ix0, iy0, iy1);
        if (iy0 == iy1) Draw__HorizontalLine(Target, Color, ix0, ix1, iy0);
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
        const i32 ix0 = Float_toInt(x0);
        const i32 ix1 = Float_toInt(x1);
        for (i32 x = ix0; x < ix1; ++x)
        {
            Bitmap__SetPixelFast(Target, Color, x, Floor_toInt(y));
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
        const i32 iy0 = Float_toInt(y0);
        const i32 iy1 = Float_toInt(y1);
        for (i32 y = iy0; y < iy1; ++y)
        {
            Bitmap__SetPixelFast(Target, Color, Floor_toInt(x), y);
            x += dx;
        }
    }
}

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
    
    const i32 iy0 = Ceil_toInt(Max(Tri.a.y, miny));
    const i32 iy1 = Ceil_toInt(Clamp(Tri.b.y, miny, maxy));
    const i32 iy2 = Ceil_toInt(Min(Tri.c.y, maxy));

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
        const i32 ix0 = Ceil_toInt(Max(x0, minx));
        const i32 ix1 = Ceil_toInt(Min(x1, maxx));
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
        const i32 ix0 = Ceil_toInt(Max(x0, minx));
        const i32 ix1 = Ceil_toInt(Min(x1, maxx));
        for (i32 x = ix0; x < ix1; ++x)
            Bitmap__SetPixelFast(Target, Color, x, y);
        x0 += dx0;
        x1 += dx1;
    }
}

void Draw_RectStruct(bitmap Target, color Color, rect Rect)
{
    bitmap ClearRect = Bitmap_Section(Target, Floor_toInt(Rect.x), 
                                              Floor_toInt(Rect.y),
                                              Floor_toInt(Rect.w),
                                              Floor_toInt(Rect.h));
    Bitmap_Clear(ClearRect, Color);
}



void Draw_PointInt(bitmap Target, color Color, i32 X, i32 Y)
{
    Bitmap_SetPixel(Target, Color, X, Y);
}

void Draw_LineInt(bitmap Target, color Color, i32 X0, i32 Y0, i32 X1, i32 Y1)
{
    line Line = (line){ 
        .a = { .x = (X0 + 0.5f), .y = (Y0 + 0.5f) },
        .b = { .x = (X1 + 0.5f), .y = (Y1 + 0.5f) }
    };
    Draw_LineStruct(Target, Color, Line);
}

void Draw_TriangleInt(bitmap Target, color Color, i32 X0, i32 Y0, i32 X1, i32 Y1, i32 X2, i32 Y2)
{
    triangle Triangle = (triangle){ 
        .a = { .x = (X0 + 0.5f), .y = (Y0 + 0.5f) },
        .b = { .x = (X1 + 0.5f), .y = (Y1 + 0.5f) },
        .c = { .x = (X2 + 0.5f), .y = (Y2 + 0.5f) }
    };
    Draw_TriangleStruct(Target, Color, Triangle);
}

void Draw_RectInt(bitmap Target, color Color, i32 X, i32 Y, i32 W, i32 H)
{
    bitmap Rect = Bitmap_Section(Target, X, Y, W, H);
    Bitmap_Clear(Rect, Color);
}



void Draw_Bitmap(bitmap Target, bitmap Bitmap, i32 X, i32 Y)
{
    bitmap Src = Bitmap_Section(Bitmap,-X,-Y, Target.Width, Target.Height);
    bitmap Dst = Bitmap_Section(Target, X, Y, Bitmap.Width, Bitmap.Height);
    Bitmap_Blit(Dst, Src);
}

void Draw_String(bitmap Target, bitmap Font, color Color, i32 X, i32 Y, const char *String)
{
    i32 XX = X;
    i32 CW = Font.Width / 8;
    i32 CH = Font.Height / 8;
    while (*String)
    {
        char C = *String++;
        if (C == '\n') { XX = X; Y -= 8; continue; }
        i32 CX = (C % CW) * 8;
        i32 CY = (C / CW) * 8;
        struct bitmap Char = Bitmap_Section(Font, CX, CY, 8, 8);
        Draw_Bitmap(Target, Char, XX, Y);
        XX += 8;
    }
}



inline bool Draw__TriangleClipZ(vertex A, vertex B, vertex C)
{
    // TODO: proper clipping

    if (A.Position.z <= 0 || B.Position.z <= 0 || C.Position.z <= 0)
        return false;

    return true;
}

// struct tri_grad
// {
//     f32 t, dt_dy;
// };

// struct tri_attr
// {
//     struct tri_grad l, r;
// };

inline void Draw__TriangleVerts2DInternal(bitmap Target, const color Color, const rect Clip, 
                                          f32 y0, f32 y1, f32 x0, f32 x1, f32 dx0_dy, f32 dx1_dy)
{
    const f32 minx = Clip.x;
    const f32 miny = Clip.y;
    const f32 maxx = Clip.x + Clip.w;
    const f32 maxy = Clip.y + Clip.h;

    const f32 yy0 = Ceil(Max(y0, miny));
    const f32 yy1 = Ceil(Min(y1, maxy));
    const i32 iy0 = Float_toInt(yy0);
    const i32 iy1 = Float_toInt(yy1);
    const f32 dy = yy0 - y0;

    x0 += dx0_dy * dy;
    x1 += dx1_dy * dy;

    if ((x0 > x1) || ((x0 == x1) && (dx0_dy > dx1_dy)))
    {
        f32 xt = x0; x0 = x1; x1 = xt;
        f32 dxt_dy = dx0_dy; dx0_dy = dx1_dy; dx1_dy = dxt_dy;
    }

    i32 height = iy1 - iy0;
    color *DstRow = Target.Pixels + Target.Pitch * iy0;

    while (height--)
    {
        const f32 xx0 = Ceil(Max(x0, minx));
        const f32 xx1 = Ceil(Min(x1, maxx));
        const i32 ix0 = Float_toInt(xx0);
        const i32 ix1 = Float_toInt(xx1);

        i32 width = ix1 - ix0;
        color *Dst = DstRow + ix0;
        
        while (width--) *Dst++ = Color;

        DstRow += Target.Pitch;

        x0 += dx0_dy;
        x1 += dx1_dy;
    }
}

inline void Draw__TriangleVerts2D(bitmap Target, const color Color, vertex A, vertex B, vertex C, rect Clip)
{
    if (C.Position.y < Clip.y) return;

    const f32 invdy02 = 1.0f / (C.Position.y - A.Position.y);
    const f32 dx02_dy = (C.Position.x - A.Position.x) * invdy02;

    // bottom triangle
    if (B.Position.y > Clip.y)
    {
        const f32 invdy01 = 1.0f / (B.Position.y - A.Position.y);
        const f32 dx01_dy = (B.Position.x - A.Position.x) * invdy01;

        f32 x0 = A.Position.x;
        f32 x1 = A.Position.x;
        f32 y0 = A.Position.y;
        f32 y1 = B.Position.y;

        Draw__TriangleVerts2DInternal(Target, Color, Clip, 
                                      y0, y1, x0, x1, dx01_dy, dx02_dy);
    }

    // top triangle
    const f32 invdy12 = 1.0f / (C.Position.y - B.Position.y);
    const f32 dx12_dy = (C.Position.x - B.Position.x) * invdy12;

    f32 x0 = A.Position.x;
    f32 x1 = B.Position.x;
    f32 y0 = B.Position.y;
    f32 y1 = C.Position.y;

    x0 += dx02_dy * (B.Position.y - A.Position.y);

    Draw__TriangleVerts2DInternal(Target, Color, Clip, 
                                  y0, y1, x0, x1, dx02_dy, dx12_dy);
}

inline void Draw__TriangleVerts3DInternal(bitmap Target, const color Color, const rect Clip, 
                                          f32 y0, f32 y1, 
                                          f32 x0, f32 x1,f32 dx0_dy, f32 dx1_dy, 
                                          f32 invz0, f32 invz1, f32 d1_dyz0, f32 d1_dyz1)
{
    const f32 minx = Clip.x;
    const f32 miny = Clip.y;
    const f32 maxx = Clip.x + Clip.w;
    const f32 maxy = Clip.y + Clip.h;

    const f32 invdx = 1.0f / (dx1_dy - dx0_dy);
    const f32 d1_dxz = (d1_dyz1 - d1_dyz0) * invdx;

    const f32 yy0 = Ceil(Max(y0, miny));
    const f32 yy1 = Ceil(Min(y1, maxy));
    const i32 iy0 = Float_toInt(yy0);
    const i32 iy1 = Float_toInt(yy1);
    const f32 dy = yy0 - y0;

    x0 += dx0_dy * dy;
    x1 += dx1_dy * dy;
    invz0 += d1_dyz0 * dy;
    invz1 += d1_dyz1 * dy;

    if ((x0 > x1) || ((x0 == x1) && (dx0_dy > dx1_dy)))
    {
        f32 xt = x0; x0 = x1; x1 = xt;
        f32 invzt = invz0; invz0 = invz1; invz1 = invzt;

        f32 dxt_dy   = dx0_dy;   dx0_dy   = dx1_dy;   dx1_dy   = dxt_dy;
        f32 d1_dyzt  = d1_dyz0;  d1_dyz0  = d1_dyz1;  d1_dyz1  = d1_dyzt;
    }

    i32 height = iy1 - iy0;
    color *DstRow = Target.Pixels + Target.Pitch * iy0;

    while (height--)
    {
        const f32 xx0 = Ceil(Max(x0, minx));
        const f32 xx1 = Ceil(Min(x1, maxx));
        const i32 ix0 = Float_toInt(xx0);
        const i32 ix1 = Float_toInt(xx1);
        const f32 dx = xx0 - x0;

        f32 invz = invz0 + d1_dxz * dx;

        i32 width = ix1 - ix0;
        color *Dst = DstRow + ix0;
        
        while (width--)
        {
            f32 z = 1.0f / invz;
            *Dst++ = Color;
            invz += d1_dxz;
        }

        DstRow += Target.Pitch;

        x0 += dx0_dy;
        x1 += dx1_dy;
        invz0 += d1_dyz0;
    }
}

inline void Draw__TriangleVerts3D(bitmap Target, const color Color, vertex A, vertex B, vertex C, const rect Clip)
{
    if (!Draw__TriangleClipZ(A, B, C)) return;

    if (C.Position.y < Clip.y) return;

    const f32 d1_dy02 = 1.0f / (C.Position.y - A.Position.y);
    const f32 dx02_dy = (C.Position.x - A.Position.x) * d1_dy02;
    const f32 d1_dyz02 = (1.0f / C.Position.z - 1.0f / A.Position.z) * d1_dy02;

    // bottom triangle
    if (B.Position.y > Clip.y)
    {
        const f32 d1_dy01 = 1.0f / (B.Position.y - A.Position.y);
        const f32 dx01_dy = (B.Position.x - A.Position.x) * d1_dy01;
        const f32 d1_dyz01 = (1.0f / B.Position.z - 1.0f / A.Position.z) * d1_dy01;

        f32 x0 = A.Position.x;
        f32 x1 = A.Position.x;
        f32 y0 = A.Position.y;
        f32 y1 = B.Position.y;
        f32 invz0 = 1.0f / A.Position.z;
        f32 invz1 = 1.0f / A.Position.z;

        Draw__TriangleVerts3DInternal(Target, Color, Clip, 
                                      y0, y1,
                                      x0, x1, dx01_dy, dx02_dy,
                                      invz0, invz1, d1_dyz01, d1_dyz02);
    }

    // top triangle
    const f32 d1_dy12 = 1.0f / (C.Position.y - B.Position.y);
    const f32 dx12_dy = (C.Position.x - B.Position.x) * d1_dy12;
    const f32 d1_dyz12 = (1.0f / C.Position.z - 1.0f / B.Position.z) * d1_dy12;

    f32 x0 = A.Position.x;
    f32 x1 = B.Position.x;
    f32 y0 = B.Position.y;
    f32 y1 = C.Position.y;
    f32 invz0 = 1.0f / A.Position.z;
    f32 invz1 = 1.0f / B.Position.z;

    const f32 dy = (B.Position.y - A.Position.y);
    x0 += dx02_dy * dy;
    invz0 += d1_dyz02 * dy;

    Draw__TriangleVerts3DInternal(Target, Color, Clip, 
                                  y0, y1,
                                  x0, x1, dx02_dy, dx12_dy,
                                  invz0, invz1, d1_dyz02, d1_dyz12);
}

inline void Draw__TriangleTexturedVerts2DInternal(bitmap Target, const bitmap Texture, const rect Clip, 
                                                  f32 y0, f32 y1,
                                                  f32 x0, f32 x1, f32 dx0_dy, f32 dx1_dy, 
                                                  f32 u0, f32 u1, f32 du0_dy, f32 du1_dy,
                                                  f32 v0, f32 v1, f32 dv0_dy, f32 dv1_dy)
{
    const f32 minx = Clip.x;
    const f32 miny = Clip.y;
    const f32 maxx = Clip.x + Clip.w;
    const f32 maxy = Clip.y + Clip.h;

    const f32 invdx = 1.0f / (dx1_dy - dx0_dy);
    const f32 du_dx = (du1_dy - du0_dy) * invdx;
    const f32 dv_dx = (dv1_dy - dv0_dy) * invdx;

    const f32 yy0 = Ceil(Max(y0, miny));
    const f32 yy1 = Ceil(Min(y1, maxy));
    const i32 iy0 = Float_toInt(yy0);
    const i32 iy1 = Float_toInt(yy1);
    const f32 dy = yy0 - y0;

    x0 += dx0_dy * dy;
    x1 += dx1_dy * dy;
    u0 += du0_dy * dy;
    u1 += du1_dy * dy;
    v0 += dv0_dy * dy;
    v1 += dv1_dy * dy;

    if ((x0 > x1) || ((x0 == x1) && (dx0_dy > dx1_dy)))
    {
        f32 xt = x0; x0 = x1; x1 = xt;
        f32 ut = u0; u0 = u1; u1 = ut;
        f32 vt = v0; v0 = v1; v1 = vt;
        f32 dxt_dy = dx0_dy; dx0_dy = dx1_dy; dx1_dy = dxt_dy;
        f32 dut_dy = du0_dy; du0_dy = du1_dy; du1_dy = dut_dy;
        f32 dvt_dy = dv0_dy; dv0_dy = dv1_dy; dv1_dy = dvt_dy;
    }

    i32 height = iy1 - iy0;
    color *DstRow = Target.Pixels + Target.Pitch * iy0;

    while (height--)
    {
        const f32 xx0 = Ceil(Max(x0, minx));
        const f32 xx1 = Ceil(Min(x1, maxx));
        const i32 ix0 = Float_toInt(xx0);
        const i32 ix1 = Float_toInt(xx1);
        const f32 dx = xx0 - x0;

        f32 u = u0 + du_dx * dx;
        f32 v = v0 + dv_dx * dx;

        i32 width = ix1 - ix0;
        color *Dst = DstRow + ix0;
        
        while (width--)
        {
            const i32 iu = Floor_toInt(u);
            const i32 iv = Floor_toInt(v);
            color Color = *(Texture.Pixels + Texture.Pitch * iv + iu);
            if (Color.Value) *Dst = Color;

            ++Dst;

            u += du_dx;
            v += dv_dx;
        }

        DstRow += Target.Pitch;

        x0 += dx0_dy;
        x1 += dx1_dy;
        u0 += du0_dy;
        v0 += dv0_dy;
    }
}

inline void Draw__TriangleTexturedVerts2D(bitmap Target, const bitmap Texture, vertex A, vertex B, vertex C, const rect Clip)
{
    if (C.Position.y < Clip.y) return;

    const f32 invdy02 = 1.0f / (C.Position.y - A.Position.y);
    const f32 dx02dy = (C.Position.x - A.Position.x) * invdy02;
    const f32 du02dy = (C.TexCoord.u - A.TexCoord.u) * invdy02;
    const f32 dv02dy = (C.TexCoord.v - A.TexCoord.v) * invdy02;

    // bottom triangle
    if (B.Position.y > Clip.y)
    {
        const f32 invdy01 = 1.0f / (B.Position.y - A.Position.y);
        const f32 dx01dy = (B.Position.x - A.Position.x) * invdy01;
        const f32 du01dy = (B.TexCoord.u - A.TexCoord.u) * invdy01;
        const f32 dv01dy = (B.TexCoord.v - A.TexCoord.v) * invdy01;    

        f32 x0 = A.Position.x;
        f32 x1 = A.Position.x;
        f32 y0 = A.Position.y;
        f32 y1 = B.Position.y;
        f32 u0 = A.TexCoord.u;
        f32 u1 = A.TexCoord.u;
        f32 v0 = A.TexCoord.v;
        f32 v1 = A.TexCoord.v;

        Draw__TriangleTexturedVerts2DInternal(Target, Texture, Clip, 
                                              y0, y1,
                                              x0, x1, dx01dy, dx02dy,
                                              u0, u1, du01dy, du02dy,
                                              v0, v1, dv01dy, dv02dy);
    }

    // top triangle
    const f32 invdy12 = 1.0f / (C.Position.y - B.Position.y);
    const f32 dx12dy = (C.Position.x - B.Position.x) * invdy12;
    const f32 du12dy = (C.TexCoord.u - B.TexCoord.u) * invdy12;
    const f32 dv12dy = (C.TexCoord.v - B.TexCoord.v) * invdy12;

    f32 x0 = A.Position.x;
    f32 x1 = B.Position.x;
    f32 y0 = B.Position.y;
    f32 y1 = C.Position.y;
    f32 u0 = A.TexCoord.u;
    f32 u1 = B.TexCoord.u;
    f32 v0 = A.TexCoord.v;
    f32 v1 = B.TexCoord.v;

    x0 += dx02dy * (B.Position.y - A.Position.y);
    u0 += du02dy * (B.Position.y - A.Position.y);
    v0 += dv02dy * (B.Position.y - A.Position.y);
    Draw__TriangleTexturedVerts2DInternal(Target, Texture, Clip, 
                                          y0, y1,
                                          x0, x1, dx02dy, dx12dy,
                                          u0, u1, du02dy, du12dy,
                                          v0, v1, dv02dy, dv12dy);
}

inline void Draw__TriangleTexturedVerts3DInternal(bitmap Target, const bitmap Texture, const rect Clip, 
                                                  f32 y0, f32 y1, 
                                                  f32 x0, f32 x1,f32 dx0_dy, f32 dx1_dy, 
                                                  f32 invz0, f32 invz1, f32 d1_dyz0, f32 d1_dyz1, 
                                                  f32 u0_z0, f32 u1_z1, f32 du0_dyz0, f32 du1_dyz1,
                                                  f32 v0_z0, f32 v1_z1, f32 dv0_dyz0, f32 dv1_dyz1)
{
    const f32 minx = Clip.x;
    const f32 miny = Clip.y;
    const f32 maxx = Clip.x + Clip.w;
    const f32 maxy = Clip.y + Clip.h;

    const i32 umask = Texture.Width - 1;
    const i32 vmask = Texture.Height - 1;

    const f32 invdx = 1.0f / (dx1_dy - dx0_dy);
    const f32 d1_dxz = (d1_dyz1 - d1_dyz0) * invdx;
    const f32 du_dxz = (du1_dyz1 - du0_dyz0) * invdx;
    const f32 dv_dxz = (dv1_dyz1 - dv0_dyz0) * invdx;

    const f32 yy0 = Ceil(Max(y0, miny));
    const f32 yy1 = Ceil(Min(y1, maxy));
    const i32 iy0 = Float_toInt(yy0);
    const i32 iy1 = Float_toInt(yy1);
    const f32 dy = yy0 - y0;

    x0 += dx0_dy * dy;
    x1 += dx1_dy * dy;
    invz0 += d1_dyz0 * dy;
    invz1 += d1_dyz1 * dy;

    u0_z0 += du0_dyz0 * dy;
    u1_z1 += du1_dyz1 * dy;
    v0_z0 += dv0_dyz0 * dy;
    v1_z1 += dv1_dyz1 * dy;

    if ((x0 > x1) || ((x0 == x1) && (dx0_dy > dx1_dy)))
    {
        f32 xt = x0; x0 = x1; x1 = xt;
        f32 invzt = invz0; invz0 = invz1; invz1 = invzt;
        f32 ut_zt = u0_z0; u0_z0 = u1_z1; u1_z1 = ut_zt;
        f32 vt_zt = v0_z0; v0_z0 = v1_z1; v1_z1 = vt_zt;

        f32 dxt_dy   = dx0_dy;   dx0_dy   = dx1_dy;   dx1_dy   = dxt_dy;
        f32 d1_dyzt  = d1_dyz0;  d1_dyz0  = d1_dyz1;  d1_dyz1  = d1_dyzt;
        f32 dut_dyzt = du0_dyz0; du0_dyz0 = du1_dyz1; du1_dyz1 = dut_dyzt;
        f32 dvt_dyzt = dv0_dyz0; dv0_dyz0 = dv1_dyz1; dv1_dyz1 = dvt_dyzt;
    }

    i32 height = iy1 - iy0;
    color *DstRow = Target.Pixels + Target.Pitch * iy0;

    while (height--)
    {
        const f32 xx0 = Ceil(Max(x0, minx));
        const f32 xx1 = Ceil(Min(x1, maxx));
        const i32 ix0 = Float_toInt(xx0);
        const i32 ix1 = Float_toInt(xx1);
        const f32 dx = xx0 - x0;

        f32 invz = invz0 + d1_dxz * dx;
        f32 u_z = u0_z0 + du_dxz * dx;
        f32 v_z = v0_z0 + dv_dxz * dx;

        i32 width = ix1 - ix0;
        color *Dst = DstRow + ix0;
        
        while (width--)
        {
            f32 z = 1.0f / invz;
            const i32 iu = Floor_toInt(u_z * z) & umask;
            const i32 iv = Floor_toInt(v_z * z) & vmask;
            color Color = *(Texture.Pixels + Texture.Pitch * iv + iu);
            if (Color.Value) *Dst = Color;
            ++Dst;

            invz += d1_dxz;
            u_z += du_dxz;
            v_z += dv_dxz;
        }

        DstRow += Target.Pitch;

        x0 += dx0_dy;
        x1 += dx1_dy;
        invz0 += d1_dyz0;
        u0_z0 += du0_dyz0;
        v0_z0 += dv0_dyz0;
    }
}

inline void Draw__TriangleTexturedVerts3D(bitmap Target, const bitmap Texture, vertex A, vertex B, vertex C, const rect Clip)
{
    if (!Draw__TriangleClipZ(A, B, C)) return;

    if (C.Position.y < Clip.y) return;

    const f32 d1_dy02 = 1.0f / (C.Position.y - A.Position.y);
    const f32 dx02_dy = (C.Position.x - A.Position.x) * d1_dy02;
    const f32 d1_dyz02 = (1.0f / C.Position.z - 1.0f / A.Position.z) * d1_dy02;
    const f32 du02_dyz02 = (C.TexCoord.u / C.Position.z - A.TexCoord.u / A.Position.z) * d1_dy02;
    const f32 dv02_dyz02 = (C.TexCoord.v / C.Position.z - A.TexCoord.v / A.Position.z) * d1_dy02;

    // bottom triangle
    if (B.Position.y > Clip.y)
    {
        const f32 d1_dy01 = 1.0f / (B.Position.y - A.Position.y);
        const f32 dx01_dy = (B.Position.x - A.Position.x) * d1_dy01;
        const f32 d1_dyz01 = (1.0f / B.Position.z - 1.0f / A.Position.z) * d1_dy01;
        const f32 du01_dyz01 = (B.TexCoord.u / B.Position.z - A.TexCoord.u / A.Position.z) * d1_dy01;
        const f32 dv01_dyz01 = (B.TexCoord.v / B.Position.z - A.TexCoord.v / A.Position.z) * d1_dy01;

        f32 x0 = A.Position.x;
        f32 x1 = A.Position.x;
        f32 y0 = A.Position.y;
        f32 y1 = B.Position.y;
        f32 invz0 = 1.0f / A.Position.z;
        f32 invz1 = 1.0f / A.Position.z;
        f32 u0_z0 = A.TexCoord.u * invz0;
        f32 u1_z1 = A.TexCoord.u * invz1;
        f32 v0_z0 = A.TexCoord.v * invz0;
        f32 v1_z1 = A.TexCoord.v * invz1;

        Draw__TriangleTexturedVerts3DInternal(Target, Texture, Clip, 
                                              y0, y1,
                                              x0, x1, dx01_dy, dx02_dy,
                                              invz0, invz1, d1_dyz01, d1_dyz02, 
                                              u0_z0, u1_z1, du01_dyz01, du02_dyz02,
                                              v0_z0, v1_z1, dv01_dyz01, dv02_dyz02);
    }

    // top triangle
    const f32 d1_dy12 = 1.0f / (C.Position.y - B.Position.y);
    const f32 dx12_dy = (C.Position.x - B.Position.x) * d1_dy12;
    const f32 d1_dyz12 = (1.0f / C.Position.z - 1.0f / B.Position.z) * d1_dy12;
    const f32 du12_dyz12 = (C.TexCoord.u / C.Position.z - B.TexCoord.u / B.Position.z) * d1_dy12;
    const f32 dv12_dyz12 = (C.TexCoord.v / C.Position.z - B.TexCoord.v / B.Position.z) * d1_dy12;

    f32 x0 = A.Position.x;
    f32 x1 = B.Position.x;
    f32 y0 = B.Position.y;
    f32 y1 = C.Position.y;
    f32 invz0 = 1.0f / A.Position.z;
    f32 invz1 = 1.0f / B.Position.z;
    f32 u0_z0 = A.TexCoord.u * invz0;
    f32 u1_z1 = B.TexCoord.u * invz1;
    f32 v0_z0 = A.TexCoord.v * invz0;
    f32 v1_z1 = B.TexCoord.v * invz1;

    const f32 dy = (B.Position.y - A.Position.y);
    x0 += dx02_dy * dy;
    invz0 += d1_dyz02 * dy;
    u0_z0 += du02_dyz02 * dy;
    v0_z0 += dv02_dyz02 * dy;

    Draw__TriangleTexturedVerts3DInternal(Target, Texture, Clip, 
                                          y0, y1,
                                          x0, x1, dx02_dy, dx12_dy,
                                          invz0, invz1, d1_dyz02, d1_dyz12, 
                                          u0_z0, u1_z1, du02_dyz02, du12_dyz12,
                                          v0_z0, v1_z1, dv02_dyz02, dv12_dyz12);
}

inline bool Draw__PrepareTriangleVerts(vertex *A, vertex *B, vertex *C)
{
    // triangle winding order
    f32 Cross = (B->Position.x - A->Position.x) * (C->Position.y - A->Position.y) - 
                (B->Position.y - A->Position.y) * (C->Position.x - A->Position.x);
    if (Cross < 0.0f) return false;

    // move pixel center
    A->Position.x -= 0.5f;
    A->Position.y -= 0.5f;
    B->Position.x -= 0.5f;
    B->Position.y -= 0.5f;
    C->Position.x -= 0.5f;
    C->Position.y -= 0.5f;

    // sort vertices vertically
    if (A->Position.y > B->Position.y)
    {
        if (B->Position.y > C->Position.y) { vertex T = *A; *A = *C; *C = T; }
        else
        {
            if (A->Position.y > C->Position.y) { vertex T = *A; *A = *B; *B = *C; *C = T; }
            else                               { vertex T = *A; *A = *B; *B = T; }
        }
    }
    else
    {
        if (B->Position.y > C->Position.y)
        {
            if (A->Position.y > C->Position.y) { vertex T = *C; *C = *B; *B = *A; *A = T; }
            else                               { vertex T = *B; *B = *C; *C = T; }
        }
    }

    return true;
}

void Draw_TriangleVerts(bitmap Target, const color Color, vertex A, vertex B, vertex C)
{
    rect Clip = (rect){ .x = 0.0f, .y = 0.0f, .w = (f32)Target.Width, .h = (f32)Target.Height };

    if (!Draw__PrepareTriangleVerts(&A, &B, &C)) return;

    if ((A.Position.z == B.Position.z) && (B.Position.z  == C.Position.z) && (A.Position.z >= 0.0f))
    {
        Draw__TriangleVerts2D(Target, Color, A, B, C, Clip);
    }
    else
    {
        Draw__TriangleVerts3D(Target, Color, A, B, C, Clip);
    }
}

void Draw_TriangleTexturedVerts(bitmap Target, const bitmap Texture, vertex A, vertex B, vertex C)
{
    rect Clip = (rect){ .x = 0.0f, .y = 0.0f, .w = (f32)Target.Width, .h = (f32)Target.Height };

    if (!Draw__PrepareTriangleVerts(&A, &B, &C)) return;

    if ((A.Position.z == B.Position.z) && (B.Position.z  == C.Position.z) && (A.Position.z >= 0.0f))
    {
        Draw__TriangleTexturedVerts2D(Target, Texture, A, B, C, Clip);
    }
    else
    {
        Draw__TriangleTexturedVerts3D(Target, Texture, A, B, C, Clip);
    }
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





/* SIMD RASTER LOOP
    __m128 p = _mm_setr_ps(u0, v0, x0, x1);
    __m128 dp = _mm_setr_ps(du0, dv0, dx0, dx1);
    __m128 duvdx = _mm_setr_ps(dudx, dvdx, 0, 0);

    __m128 minxx = _mm_set1_ps(minx);
    __m128 maxxx = _mm_set1_ps(maxx);

    for (i32 iy = iy1; iy < iy2; ++iy)
    {
        __m128 x = _mm_shuffle_ps(p, p, 0xFA);
        __m128 xx = _mm_ceil_ps(_mm_min_ps(_mm_max_ps(x, minxx), maxxx));
        __m128 dx = _mm_sub_ps(xx, x);
        __m128 uv = _mm_add_ps(p, _mm_mul_ps(duvdx, dx));

        i32 ixx[4];
        _mm_store_si128((__m128i *)ixx, _mm_cvtps_epi32(_mm_shuffle_ps(xx, xx, 0x0C)));
        for (i32 ix = ixx[0]; ix < ixx[1]; ++ix)
        {
            i32 iuv[4];
            _mm_store_si128((__m128i *)iuv, _mm_cvtps_epi32(_mm_floor_ps(uv)));
            color Color = Bitmap_GetPixelFast(Texture, iuv[0], iuv[1]);
            if (Color.Value) Bitmap_SetPixelFast(Buffer, Color, ix, iy);
            
            uv = _mm_add_ps(uv, duvdx);
        }
        p = _mm_add_ps(p, dp);
    }
*/