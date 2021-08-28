
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

typedef struct color_and_depth
{
    u32 Value;
} color_and_depth;

enum bitmap_flags
{
    BITMAP_ONHEAP,
};

typedef struct bitmap
{
    u32 Width, Height;
    u32 Pitch;
    u32 Flags;
    union
    {
        color *Pixels;
        color_and_depth *DepthPixels;
    };
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

void Draw_Bitmap(bitmap Target, const bitmap Bitmap, i32 X, i32 Y);
void Draw_String(bitmap Target, const bitmap Font, color Color, i32 X, i32 Y, const char *String);

void Draw_TriangleVerts(bitmap Target, color Color, vertex A, vertex B, vertex C);
void Draw_TriangleTexturedVerts(bitmap Target, const bitmap Texture, vertex A, vertex B, vertex C);

void Draw_QuadVerts(bitmap Target, color Color, vertex A, vertex B, vertex C, vertex D);
void Draw_QuadTexturedVerts(bitmap Target, const bitmap Texture, vertex A, vertex B, vertex C, vertex D);


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
    bitmap: Draw_TriangleTexturedVerts, \
    const bitmap: Draw_TriangleTexturedVerts \
)(BUFFER, COLOR, X, __VA_ARGS__)


#define Draw_QuadColor(BUFFER, COLOR, X, ...) _Generic((X), \
    vertex: Draw_QuadVerts, \
)(BUFFER, COLOR, X, __VA_ARGS__)

#define Draw_QuadTextured(BUFFER, COLOR, X, ...) _Generic((X), \
    vertex: Draw_QuadTexturedVerts \
)(BUFFER, COLOR, X, __VA_ARGS__)

#define Draw_Quad(BUFFER, COLOR, X, ...) _Generic((COLOR), \
    color: Draw_QuadVerts, \
    bitmap: Draw_QuadTexturedVerts, \
    const bitmap: Draw_QuadTexturedVerts \
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



void Draw_Bitmap(bitmap Target, const bitmap Bitmap, i32 X, i32 Y)
{
    bitmap Src = Bitmap_Section(Bitmap,-X,-Y, Target.Width, Target.Height);
    bitmap Dst = Bitmap_Section(Target, X, Y, Bitmap.Width, Bitmap.Height);
    Bitmap_Blit(Dst, Src);
}

void Draw_String(bitmap Target, const bitmap Font, color Color, i32 X, i32 Y, const char *String)
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



// struct tri_grad
// {
//     f32 t, dt_dy;
// };

// struct tri_attr
// {
//     struct tri_grad l, r;
// };

#if defined(USE_SCANLINE_RASTERIZER)

inline void Draw__TriangleTexturedShadedVerts3DInternal(bitmap Target, const bitmap Texture, 
                                                        f32 y0, f32 y1, 
                                                        f32 x0, f32 x1,f32 dx0_dy, f32 dx1_dy, 
                                                        f32 invz0, f32 invz1, f32 d1_dyz0, f32 d1_dyz1, 
                                                        f32 u0_z0, f32 u1_z1, f32 du0_dyz0, f32 du1_dyz1,
                                                        f32 v0_z0, f32 v1_z1, f32 dv0_dyz0, f32 dv1_dyz1,
                                                        f32 s0_z0, f32 s1_z1, f32 ds0_dyz0, f32 ds1_dyz1)
{
    const f32 minx = 0;
    const f32 miny = 0;
    const f32 maxx = (f32)Target.Width;
    const f32 maxy = (f32)Target.Height;

    const i32 umask = Texture.Width - 1;
    const i32 vmask = Texture.Height - 1;

    const f32 invdx = 1.0f / (dx1_dy - dx0_dy);
    const f32 d1_dxz = (d1_dyz1 - d1_dyz0) * invdx;
    const f32 du_dxz = (du1_dyz1 - du0_dyz0) * invdx;
    const f32 dv_dxz = (dv1_dyz1 - dv0_dyz0) * invdx;
    const f32 ds_dxz = (ds1_dyz1 - ds0_dyz0) * invdx;

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
    s0_z0 += ds0_dyz0 * dy;
    s1_z1 += ds1_dyz1 * dy;

    if ((x0 > x1) || ((x0 == x1) && (dx0_dy > dx1_dy)))
    {
        f32 xt = x0; x0 = x1; x1 = xt;
        f32 invzt = invz0; invz0 = invz1; invz1 = invzt;
        f32 ut_zt = u0_z0; u0_z0 = u1_z1; u1_z1 = ut_zt;
        f32 vt_zt = v0_z0; v0_z0 = v1_z1; v1_z1 = vt_zt;
        f32 st_zt = s0_z0; s0_z0 = s1_z1; s1_z1 = st_zt;

        f32 dxt_dy   = dx0_dy;   dx0_dy   = dx1_dy;   dx1_dy   = dxt_dy;
        f32 d1_dyzt  = d1_dyz0;  d1_dyz0  = d1_dyz1;  d1_dyz1  = d1_dyzt;
        f32 dut_dyzt = du0_dyz0; du0_dyz0 = du1_dyz1; du1_dyz1 = dut_dyzt;
        f32 dvt_dyzt = dv0_dyz0; dv0_dyz0 = dv1_dyz1; dv1_dyz1 = dvt_dyzt;
        f32 dst_dyzt = ds0_dyz0; ds0_dyz0 = ds1_dyz1; ds1_dyz1 = dst_dyzt;
    }

    i32 height = iy1 - iy0;
    color *DstRow = Target.Pixels + Target.Pitch * iy0;

    while (height-- > 0)
    {
        const f32 xx0 = Ceil(Max(x0, minx));
        const f32 xx1 = Ceil(Min(x1, maxx));
        const i32 ix0 = Float_toInt(xx0);
        const i32 ix1 = Float_toInt(xx1);
        const f32 dx = xx0 - x0;

        f32 invz = invz0 + d1_dxz * dx;
        f32 u_z = u0_z0 + du_dxz * dx;
        f32 v_z = v0_z0 + dv_dxz * dx;
        f32 s_z = s0_z0 + ds_dxz * dx;

        i32 width = ix1 - ix0;
        color *Dst = DstRow + ix0;
        
        while (width-- > 0)
        {
            f32 z = 1.0f / invz;
            const i32 iu = Floor_toInt(u_z * z) & umask;
            const i32 iv = Floor_toInt(v_z * z) & vmask;
            const u8 is = Floor_toInt(s_z * z * 15.999f) & 15;
            color Color = *(Texture.Pixels + Texture.Pitch * iv + iu);
            if (Color.Value) Dst->Value = Color.Value | is;
            ++Dst;

            invz += d1_dxz;
            u_z += du_dxz;
            v_z += dv_dxz;
            s_z += ds_dxz;
        }

        DstRow += Target.Pitch;

        x0 += dx0_dy;
        x1 += dx1_dy;
        invz0 += d1_dyz0;
        u0_z0 += du0_dyz0;
        v0_z0 += dv0_dyz0;
        s0_z0 += ds0_dyz0;
    }
}

inline void Draw__TriangleTexturedShadedVerts3D(bitmap Target, const bitmap Texture, vertex A, vertex B, vertex C)
{
    const f32 d1_dy02 = 1.0f / (C.Position.y - A.Position.y);
    const f32 dx02_dy = (C.Position.x - A.Position.x) * d1_dy02;
    const f32 d1_dyz02 = (1.0f / C.Position.z - 1.0f / A.Position.z) * d1_dy02;
    const f32 du02_dyz02 = (C.TexCoord.u / C.Position.z - A.TexCoord.u / A.Position.z) * d1_dy02;
    const f32 dv02_dyz02 = (C.TexCoord.v / C.Position.z - A.TexCoord.v / A.Position.z) * d1_dy02;
    const f32 ds02_dyz02 = (C.Shadow / C.Position.z - A.Shadow / A.Position.z) * d1_dy02;

    // bottom triangle
    if (B.Position.y > 0)
    {
        const f32 d1_dy01 = 1.0f / (B.Position.y - A.Position.y);
        const f32 dx01_dy = (B.Position.x - A.Position.x) * d1_dy01;
        const f32 d1_dyz01 = (1.0f / B.Position.z - 1.0f / A.Position.z) * d1_dy01;
        const f32 du01_dyz01 = (B.TexCoord.u / B.Position.z - A.TexCoord.u / A.Position.z) * d1_dy01;
        const f32 dv01_dyz01 = (B.TexCoord.v / B.Position.z - A.TexCoord.v / A.Position.z) * d1_dy01;
        const f32 ds01_dyz01 = (B.Shadow / B.Position.z - A.Shadow / A.Position.z) * d1_dy01;

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
        f32 s0_z0 = A.Shadow * invz0;
        f32 s1_z1 = A.Shadow * invz1;

        Draw__TriangleTexturedShadedVerts3DInternal(Target, Texture, 
                                                    y0, y1,
                                                    x0, x1, dx01_dy, dx02_dy,
                                                    invz0, invz1, d1_dyz01, d1_dyz02, 
                                                    u0_z0, u1_z1, du01_dyz01, du02_dyz02,
                                                    v0_z0, v1_z1, dv01_dyz01, dv02_dyz02,
                                                    s0_z0, s1_z1, ds01_dyz01, ds02_dyz02);
    }

    // top triangle
    if (B.Position.y < (f32)Target.Height)
    {
        const f32 d1_dy12 = 1.0f / (C.Position.y - B.Position.y);
        const f32 dx12_dy = (C.Position.x - B.Position.x) * d1_dy12;
        const f32 d1_dyz12 = (1.0f / C.Position.z - 1.0f / B.Position.z) * d1_dy12;
        const f32 du12_dyz12 = (C.TexCoord.u / C.Position.z - B.TexCoord.u / B.Position.z) * d1_dy12;
        const f32 dv12_dyz12 = (C.TexCoord.v / C.Position.z - B.TexCoord.v / B.Position.z) * d1_dy12;
        const f32 ds12_dyz12 = (C.Shadow / C.Position.z - B.Shadow / B.Position.z) * d1_dy12;

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
        f32 s0_z0 = A.Shadow * invz0;
        f32 s1_z1 = B.Shadow * invz1;

        const f32 dy = (B.Position.y - A.Position.y);
        x0 += dx02_dy * dy;
        invz0 += d1_dyz02 * dy;
        u0_z0 += du02_dyz02 * dy;
        v0_z0 += dv02_dyz02 * dy;
        s0_z0 += ds02_dyz02 * dy;

        Draw__TriangleTexturedShadedVerts3DInternal(Target, Texture, 
                                                    y0, y1,
                                                    x0, x1, dx02_dy, dx12_dy,
                                                    invz0, invz1, d1_dyz02, d1_dyz12, 
                                                    u0_z0, u1_z1, du02_dyz02, du12_dyz12,
                                                    v0_z0, v1_z1, dv02_dyz02, dv12_dyz12,
                                                    s0_z0, s1_z1, ds02_dyz02, ds12_dyz12);
    }
}

#else

void Draw__TriangleTexturedShadedVerts3D(bitmap Target, const bitmap Texture, vertex A, vertex B, vertex C)
{
#define PACK_WIDTH_SHIFT 2
#define PACK_HEIGHT_SHIFT 0
#define PACK_WIDTH (1 << PACK_WIDTH_SHIFT)
#define PACK_HEIGHT (1 << PACK_HEIGHT_SHIFT)

    __m128 ColOffset = _mm_setr_ps(0, 1, 2, 3);
    __m128 RowOffset = _mm_setr_ps(0, 0, 0, 0);
    __m128 One = _mm_set1_ps(1);

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

            i32 iMinX = Float_toInt(MinX);
            i32 iMinY = Float_toInt(MinY);
            i32 iMaxX = Float_toInt(MaxX);
            i32 iMaxY = Float_toInt(MaxY);

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

                for(i32 x = iMinX; x < iMaxX; x += PACK_WIDTH)
                {
                    __m128i Mask = _mm_srai_epi32(_mm_castps_si128(_mm_or_ps(_mm_or_ps(Alpha, Beta), Gamma)), 32);
                    if (!_mm_test_all_ones(Mask))
                    {
                        __m128 zzz = _mm_div_ps(One, zz);
                        __m128 aaa = _mm_mul_ps(aa, zzz);
                        __m128 bbb = _mm_mul_ps(bb, zzz);
                        __m128 ccc = _mm_sub_ps(One, _mm_add_ps(aaa, bbb));

                        __m128 uuu = _mm_add_ps(_mm_add_ps(_mm_mul_ps(u4[0], aaa), _mm_mul_ps(u4[1], bbb)), _mm_mul_ps(u4[2], ccc));
                        __m128 vvv = _mm_add_ps(_mm_add_ps(_mm_mul_ps(v4[0], aaa), _mm_mul_ps(v4[1], bbb)), _mm_mul_ps(v4[2], ccc));
                        __m128 sss = _mm_add_ps(_mm_add_ps(_mm_mul_ps(s4[0], aaa), _mm_mul_ps(s4[1], bbb)), _mm_mul_ps(s4[2], ccc));

                        f32 U[4], V[4], S[4];
                        _mm_store_ps(U, uuu);
                        _mm_store_ps(V, vvv);
                        _mm_store_ps(S, sss);
                        u32 M[4];
                        _mm_storeu_epi32(M, Mask);
                        color *Dst = Target.Pixels + Index;
                        for (i32 p = 0; p < 4; ++p)
                        {
                            if (M[p]) continue;
                            const i32 iu = Floor_toInt(U[p]) & 255;
                            const i32 iv = Floor_toInt(V[p]) & 255;
                            const u8 is = Floor_toInt(S[p] * 15.999f) & 15;
                            color Color = *(Texture.Pixels + Texture.Pitch * iv + iu);
                            if (Color.Value) Dst[p].Value = Color.Value | is;
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

#endif

inline vertex Vertex_Lerp(vertex A, vertex B, f32 t)
{
    A.Position.x = Lerp(A.Position.x, B.Position.x, t);
    A.Position.y = Lerp(A.Position.y, B.Position.y, t);
    A.Position.z = Lerp(A.Position.z, B.Position.z, t);
    A.TexCoord.x = Lerp(A.TexCoord.x, B.TexCoord.x, t);
    A.TexCoord.y = Lerp(A.TexCoord.y, B.TexCoord.y, t);
    A.Shadow = Lerp(A.Shadow, B.Shadow, t);
    return A;
}

inline u32 Draw__TriangleClipZ(vertex *V)
{
#define CAMERA_FAR 128.0f
    if (Max(Max(V[0].Position.z, V[1].Position.z), V[2].Position.z) > CAMERA_FAR)
        return 0;

#define CAMERA_NEAR 0.1f
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
    // move pixel center
    vec3 PixelCenter = (vec3){ 0.5f, 0.5f, 0.0f };
    A->Position = Vec3_Sub(A->Position, PixelCenter);
    B->Position = Vec3_Sub(B->Position, PixelCenter);
    C->Position = Vec3_Sub(C->Position, PixelCenter);

    // triangle winding order
    f32 Cross = (B->Position.x - A->Position.x) * (C->Position.y - A->Position.y) - 
                (B->Position.y - A->Position.y) * (C->Position.x - A->Position.x);
    if (Cross < 0.0f) return false;

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
#if 0

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
#endif
    return true;
}

inline vec3 Draw__PerspectiveDivide(bitmap Target, vec3 Position)
{
    f32 HalfWidth = Target.Width * 0.5f;
    f32 HalfHeight = Target.Height * 0.5f;
    Position.x = (Position.x - HalfWidth) / Position.z + HalfWidth;
    Position.y = (Position.y - HalfHeight) / Position.z + HalfHeight;
    return Position;
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