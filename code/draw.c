
#if BYTES_PER_PIXEL == 4

  struct color
  {
      union
      {
          u32 Value;
          struct
          {
              u8 Red, Green, Blue, Alpha;
          };
      };
  };
  
  #define COLOR_U32(X) (struct color){ .Value = X }
  #define COLOR_RGB(R,G,B) (struct color){ .Red = R, .Green = G, .Blue = B, .Alpha = 0xFF }
  #define COLOR_RGBA(R,G,B,A) (struct color){ .Red = R, .Green = G, .Blue = B, .Alpha = A }
  #define COLOR_HEX(X) (struct color){ .Red = X & 0xFF, .Green = (X >> 8) & 0xFF, .Blue = (X >> 16) & 0xFF, .Alpha = 0xFF }
  
  #define COLOR_WHITE COLOR_HEX(0xFFFFFF)
  #define COLOR_BLUE COLOR_HEX(0x87CEEB)

#elif BYTES_PER_PIXEL == 1

  struct color { u8 Value; };

  #define COLOR_WHITE (struct color) { .Value = 32 }
  #define COLOR_BLUE (struct color) { .Value = 240 }

#endif

enum bitmap_flags
{
    BITMAP_ONHEAP
};

struct bitmap
{
    u32 Width, Height;
    u32 Pitch;
    u32 Flags;
    struct color *Pixels;
};

struct bitmap Bitmap_Create(u32 Width, u32 Height);
struct bitmap Bitmap_Section(struct bitmap Bitmap, i32 X, i32 Y, u32 W, u32 H);
struct color Bitmap_GetPixel(struct bitmap Buffer, i32 X, i32 Y);
void Bitmap_SetPixel(struct bitmap Buffer, struct color Color, i32 X, i32 Y);
void Bitmap_Clear(struct bitmap Buffer, struct color Color);

void Draw_PointStruct(struct bitmap Buffer, struct color Color, struct point Point);
void Draw_LineStruct(struct bitmap Buffer, struct color Color, struct line Line);
void Draw_TriangleStruct(struct bitmap Buffer, struct color Color, struct triangle Triangle);
void Draw_RectStruct(struct bitmap Buffer, struct color Color, struct rect Rect);

void Draw_PointInt(struct bitmap Buffer, struct color Color, i32 X, i32 Y);
void Draw_LineInt(struct bitmap Buffer, struct color Color, i32 X0, i32 Y0, i32 X1, i32 Y1);
void Draw_TriangleInt(struct bitmap Buffer, struct color Color, i32 X0, i32 Y0, i32 X1, i32 Y1, i32 X2, i32 Y2);
void Draw_RectInt(struct bitmap Buffer, struct color Color, i32 X, i32 Y, i32 W, i32 H);

void Draw_Bitmap(struct bitmap Buffer, struct bitmap Bitmap, i32 X, i32 Y);
void Draw_String(struct bitmap Buffer, struct bitmap Font, struct color Color, i32 X, i32 Y, const char *String);

void Draw_TriangleTextured(struct bitmap Buffer, struct bitmap Texture, struct triangle Position, struct triangle TexCoord);



#define Draw_Point(BUFFER, COLOR, X, ...) _Generic((X), \
    struct point: Draw_PointStruct, \
         i8: Draw_PointInt, i16: Draw_PointInt, i32: Draw_PointInt, i64: Draw_PointInt, \
         u8: Draw_PointInt, u16: Draw_PointInt, u32: Draw_PointInt, u64: Draw_PointInt  \
)(BUFFER, COLOR, X, __VA_ARGS__)

#define Draw_Line(BUFFER, COLOR, X, ...) _Generic((X), \
    struct line: Draw_LineStruct, \
         i8: Draw_LineInt, i16: Draw_LineInt, i32: Draw_LineInt, i64: Draw_LineInt, \
         u8: Draw_LineInt, u16: Draw_LineInt, u32: Draw_LineInt, u64: Draw_LineInt \
)(BUFFER, COLOR, X, __VA_ARGS__)

#define Draw_Triangle(BUFFER, COLOR, X, ...) _Generic((X), \
    struct triangle: Draw_TriangleStruct, \
         i8: Draw_TriangleInt, i16: Draw_TriangleInt, i32: Draw_TriangleInt, i64: Draw_TriangleInt, \
         u8: Draw_TriangleInt, u16: Draw_TriangleInt, u32: Draw_TriangleInt, u64: Draw_TriangleInt \
)(BUFFER, COLOR, X, __VA_ARGS__)

#define Draw_Rect(BUFFER, COLOR, X, ...) _Generic((X), \
    struct rect: Draw_RectStruct, \
         i8: Draw_RectInt, i16: Draw_RectInt, i32: Draw_RectInt, i64: Draw_RectInt, \
         u8: Draw_RectInt, u16: Draw_RectInt, u32: Draw_RectInt, u64: Draw_RectInt \
)(BUFFER, COLOR, X, __VA_ARGS__)



struct bitmap Bitmap_Create(u32 Width, u32 Height)
{
    return (struct bitmap) {
        .Width = Width,
        .Height = Height,
        .Pitch = Width,
        .Flags = BITMAP_ONHEAP,
        .Pixels = (struct color *)malloc(sizeof(u32) * Width * Height)
    };
}

struct bitmap Bitmap_Section(struct bitmap Bitmap, i32 X, i32 Y, u32 W, u32 H)
{
    struct bitmap Section = { 0 };

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

void Bitmap_Blit(struct bitmap Dst, struct bitmap Src)
{
    if (Dst.Width != Src.Width) return;
    if (Dst.Height != Src.Height) return;
    for (u32 y = 0; y < Dst.Height; ++y)
    for (u32 x = 0; x < Dst.Width; ++x)
        if (Src.Pixels[x + y * Src.Pitch].Value)
            Dst.Pixels[x + y * Dst.Pitch] = Src.Pixels[x + y * Src.Pitch];
}

inline struct color Bitmap_GetPixelFast(struct bitmap Buffer, i32 X, i32 Y)
{
    return *(Buffer.Pixels + X + Y * Buffer.Pitch);
}

struct color Bitmap_GetPixel(struct bitmap Buffer, i32 X, i32 Y)
{
    if ((u32)X >= Buffer.Width)  return (struct color){ 0 };
    if ((u32)Y >= Buffer.Height) return (struct color){ 0 };
    return Bitmap_GetPixelFast(Buffer, X, Y);
}

inline void Bitmap_SetPixelFast(struct bitmap Buffer, struct color Color, i32 X, i32 Y)
{
    *(Buffer.Pixels + X + Y * Buffer.Pitch) = Color;
}

void Bitmap_SetPixel(struct bitmap Buffer, struct color Color, i32 X, i32 Y)
{
    if ((u32)X >= Buffer.Width)  return;
    if ((u32)Y >= Buffer.Height) return;
    Bitmap_SetPixelFast(Buffer, Color, X, Y);
}

void Bitmap_Clear(struct bitmap Buffer, struct color Color)
{
    for (u32 y = 0; y < Buffer.Height; ++y)
    for (u32 x = 0; x < Buffer.Width; ++x)
        Bitmap_SetPixelFast(Buffer, Color, x, y);
}



void Draw_PointStruct(struct bitmap Buffer, struct color Color, struct point Point)
{
    Bitmap_SetPixel(Buffer, Color, Floor_toInt(Point.x), Floor_toInt(Point.y));
}

inline void Draw_VerticalLine(struct bitmap Buffer, struct color Color, i32 X, i32 Y0, i32 Y1)
{
    if ((u32)X >= Buffer.Width) return;

    if (Y0 < 0) Y0 = 0;
    if (Y1 > (i32)Buffer.Height) Y1 = Buffer.Height;

    for (i32 y = Y0; y < Y1; ++y)
        Bitmap_SetPixelFast(Buffer, Color, X, y);
}

inline void Draw_HorizontalLine(struct bitmap Buffer, struct color Color, i32 X0, i32 X1, i32 Y)
{
    if ((u32)Y >= Buffer.Height) return;
    
    if (X0 < 0) X0 = 0;
    if (X1 > (i32)Buffer.Width) X1 = Buffer.Width;

    for (i32 x = X0; x < X1; ++x)
        Bitmap_SetPixelFast(Buffer, Color, x, Y);
}

void Draw_LineStruct(struct bitmap Buffer, struct color Color, struct line Line)
{
    struct rect Clip = (struct rect){ .x = 0.0f, .y = 0.0f, .w = (f32)Buffer.Width, .h = (f32)Buffer.Height };
    
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
        if (ix0 == ix1) Draw_VerticalLine(Buffer, Color, ix0, iy0, iy1);
        if (iy0 == iy1) Draw_HorizontalLine(Buffer, Color, ix0, ix1, iy0);
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
            Bitmap_SetPixelFast(Buffer, Color, x, Floor_toInt(y));
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
            Bitmap_SetPixelFast(Buffer, Color, Floor_toInt(x), y);
            x += dx;
        }
    }
}

void Draw_TriangleStruct(struct bitmap Buffer, struct color Color, struct triangle Triangle)
{
    struct rect Clip = (struct rect){ .x = 0.0f, .y = 0.0f, .w = (f32)Buffer.Width, .h = (f32)Buffer.Height };

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

    const struct triangle Tri = Triangle_SortY(Triangle);
    
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
            Bitmap_SetPixelFast(Buffer, Color, x, y);
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
            Bitmap_SetPixelFast(Buffer, Color, x, y);
        x0 += dx0;
        x1 += dx1;
    }
}

void Draw_RectStruct(struct bitmap Buffer, struct color Color, struct rect Rect)
{
    struct bitmap ClearRect = Bitmap_Section(Buffer, Floor_toInt(Rect.x), 
                                                     Floor_toInt(Rect.y),
                                                     Floor_toInt(Rect.w),
                                                     Floor_toInt(Rect.h));
    Bitmap_Clear(ClearRect, Color);
}



void Draw_PointInt(struct bitmap Buffer, struct color Color, i32 X, i32 Y)
{
    Bitmap_SetPixel(Buffer, Color, X, Y);
}

void Draw_LineInt(struct bitmap Buffer, struct color Color, i32 X0, i32 Y0, i32 X1, i32 Y1)
{
    struct line Line = (struct line){ 
        .a = { .x = (X0 + 0.5f), .y = (Y0 + 0.5f) },
        .b = { .x = (X1 + 0.5f), .y = (Y1 + 0.5f) }
    };
    Draw_LineStruct(Buffer, Color, Line);
}

void Draw_TriangleInt(struct bitmap Buffer, struct color Color, i32 X0, i32 Y0, i32 X1, i32 Y1, i32 X2, i32 Y2)
{
    struct triangle Triangle = (struct triangle){ 
        .a = { .x = (X0 + 0.5f), .y = (Y0 + 0.5f) },
        .b = { .x = (X1 + 0.5f), .y = (Y1 + 0.5f) },
        .c = { .x = (X2 + 0.5f), .y = (Y2 + 0.5f) }
    };
    Draw_TriangleStruct(Buffer, Color, Triangle);
}

void Draw_RectInt(struct bitmap Buffer, struct color Color, i32 X, i32 Y, i32 W, i32 H)
{
    struct bitmap Rect = Bitmap_Section(Buffer, X, Y, W, H);
    Bitmap_Clear(Rect, Color);
}



void Draw_Bitmap(struct bitmap Buffer, struct bitmap Bitmap, i32 X, i32 Y)
{
    struct bitmap Src = Bitmap_Section(Bitmap,-X,-Y, Buffer.Width, Buffer.Height);
    struct bitmap Dst = Bitmap_Section(Buffer, X, Y, Bitmap.Width, Bitmap.Height);
    Bitmap_Blit(Dst, Src);
}

void Draw_String(struct bitmap Buffer, struct bitmap Font, struct color Color, i32 X, i32 Y, const char *String)
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
        Draw_Bitmap(Buffer, Char, XX, Y);
        XX += 8;
    }
}

#if 1

void Draw_TriangleTextured(struct bitmap Buffer, struct bitmap Texture, struct triangle Position, struct triangle TexCoord)
{
    struct rect Clip = (struct rect){ .x = 0.0f, .y = 0.0f, .w = (f32)Buffer.Width, .h = (f32)Buffer.Height };

    Position.a.x -= 0.5f;
    Position.a.y -= 0.5f;
    Position.b.x -= 0.5f;
    Position.b.y -= 0.5f;
    Position.c.x -= 0.5f;
    Position.c.y -= 0.5f;

    f32 minx = Clip.x;
    f32 miny = Clip.y;
    f32 maxx = Clip.x + Clip.w;
    f32 maxy = Clip.y + Clip.h;

    const struct triangle Pos = Triangle_SortY(Position);
    const struct triangle Tex = Triangle_SortY2(Position, TexCoord);

    const f32 invdy01 = 1.0f / (Pos.b.y - Pos.a.y);
    const f32 dx01dy = (Pos.b.x - Pos.a.x) * invdy01;
    const f32 du01dy = (Tex.b.x - Tex.a.x) * invdy01;
    const f32 dv01dy = (Tex.b.y - Tex.a.y) * invdy01;

    const f32 invdy02 = 1.0f / (Pos.c.y - Pos.a.y);
    const f32 dx02dy = (Pos.c.x - Pos.a.x) * invdy02;
    const f32 du02dy = (Tex.c.x - Tex.a.x) * invdy02;
    const f32 dv02dy = (Tex.c.y - Tex.a.y) * invdy02;

    const f32 invdy12 = 1.0f / (Pos.c.y - Pos.b.y);
    const f32 dx12dy = (Pos.c.x - Pos.b.x) * invdy12;
    const f32 du12dy = (Tex.c.x - Tex.b.x) * invdy12;
    const f32 dv12dy = (Tex.c.y - Tex.b.y) * invdy12;

    f32 x0 = Pos.a.x;
    f32 x1 = Pos.a.x;
    f32 y0 = Pos.a.y;
    f32 y1 = Pos.b.y;
    f32 u0 = Tex.a.x;
    f32 v0 = Tex.a.y;

    {// bottom triangle
        f32 dx0dy, dx1dy, du0dy, du1dy, dv0dy, dv1dy;
        if (dx01dy < dx02dy)
        {
            dx0dy = dx01dy; dx1dy = dx02dy;
            du0dy = du01dy; du1dy = du02dy;
            dv0dy = dv01dy; dv1dy = dv02dy;
        }
        else
        {
            dx0dy = dx02dy; dx1dy = dx01dy;
            du0dy = du02dy; du1dy = du01dy;
            dv0dy = dv02dy; dv1dy = dv01dy;
        }

        f32 invdx = 1.0f / (dx1dy - dx0dy);
        f32 dudx = (du1dy - du0dy) * invdx;
        f32 dvdx = (dv1dy - dv0dy) * invdx;

        const f32 d = Max(miny, Ceil(y0)) - y0;
        x0 += dx0dy * d;
        x1 += dx1dy * d;
        u0 += du0dy * d;
        v0 += dv0dy * d;
    
        const i32 iy0 = Ceil_toInt(Max(y0, miny));
        const i32 iy1 = Ceil_toInt(Min(y1, maxy));
        
        i32 height = iy1 - iy0;
        struct color *DstRow = Buffer.Pixels + Buffer.Pitch * iy0;

        while (height--)
        {
            const f32 xx0 = Ceil(Max(x0, minx));
            const f32 xx1 = Ceil(Min(x1, maxx));
            const i32 ix0 = Float_toInt(xx0);
            const i32 ix1 = Float_toInt(xx1);
            const f32 dx = xx0 - x0;

            f32 u = u0 + dudx * dx;
            f32 v = v0 + dvdx * dx;

            i32 width = ix1 - ix0;
            struct color *Dst = DstRow + ix0;
            
            while (width--)
            {
                const i32 iu = Floor_toInt(u);
                const i32 iv = Floor_toInt(v);
                struct color Color = *(Texture.Pixels + Texture.Pitch * iv + iu);
                if (Color.Value) *Dst = Color;

                ++Dst;

                u += dudx;
                v += dvdx;
            }

            DstRow += Buffer.Pitch;

            x0 += dx0dy;
            x1 += dx1dy;
            u0 += du0dy;
            v0 += dv0dy;
        }
    }

    y0 = Pos.b.y;
    y1 = Pos.c.y;

    {// top triangle
        f32 dx0dy, dx1dy, du0dy, du1dy, dv0dy, dv1dy;
        if (dx12dy < dx02dy)
        {
            dx0dy = dx02dy; dx1dy = dx12dy;
            du0dy = du02dy; du1dy = du12dy;
            dv0dy = dv02dy; dv1dy = dv12dy;
        }
        else
        {
            dx0dy = dx12dy; dx1dy = dx02dy;
            du0dy = du12dy; du1dy = du02dy;
            dv0dy = dv12dy; dv1dy = dv02dy;
        }

        f32 invdx = 1.0f / (dx1dy - dx0dy);
        f32 dudx = (du1dy - du0dy) * invdx;
        f32 dvdx = (dv1dy - dv0dy) * invdx;

        const f32 d = Max(miny, Ceil(y0)) - y0;
        if (dx01dy < dx02dy)
        {
            x0 = Pos.b.x + (dx0dy * d);
            u0 = Tex.b.x + (du0dy * d);
            v0 = Tex.b.y + (dv0dy * d);
        }
        else
        {
            x1 = Pos.b.x + (dx1dy * d);
        }

        const i32 iy0 = Ceil_toInt(Max(y0, miny));
        const i32 iy1 = Ceil_toInt(Min(y1, maxy));

        i32 height = iy1 - iy0;
        struct color *DstRow = Buffer.Pixels + Buffer.Pitch * iy0;

        while (height--)
        {
            const f32 xx0 = Ceil(Max(x0, minx));
            const f32 xx1 = Ceil(Min(x1, maxx));
            const i32 ix0 = Float_toInt(xx0);
            const i32 ix1 = Float_toInt(xx1);
            const f32 dx = xx0 - x0;
            f32 u = u0 + dudx * dx;
            f32 v = v0 + dvdx * dx;


            i32 width = ix1 - ix0;
            struct color *Dst = DstRow + ix0;
            
            for (i32 ix = ix0; ix < ix1; ++ix)
            {
                const i32 iu = Floor_toInt(u);
                const i32 iv = Floor_toInt(v);
                struct color Color = *(Texture.Pixels + Texture.Pitch * iv + iu);
                if (Color.Value) *Dst = Color;
                ++Dst;
                
                u += dudx;
                v += dvdx;
            }

            DstRow += Buffer.Pitch;

            x0 += dx0dy;
            x1 += dx1dy;
            u0 += du0dy;
            v0 += dv0dy;
        }
    }
}

#else

inline void Draw__TriangleTextured(struct bitmap Buffer, struct bitmap Texture, struct rect Clip, 
                                   f32 x0, f32 y0, f32 x1, f32 y1,
                                   f32 u0, f32 v0, f32 u1, f32 v1,
                                   f32 dx0dy, f32 dx1dy, 
                                   f32 du0dy, f32 du1dy,
                                   f32 dv0dy, f32 dv1dy)
{
    const f32 minx = Clip.x;
    const f32 miny = Clip.y;
    const f32 maxx = Clip.x + Clip.w;
    const f32 maxy = Clip.y + Clip.h;

    const f32 invdx = 1.0f / (dx1dy - dx0dy);
    const f32 dudx = (du1dy - du0dy) * invdx;
    const f32 dvdx = (dv1dy - dv0dy) * invdx;

    const f32 yy0 = Ceil(Max(y0, miny));
    const f32 yy1 = Ceil(Min(y1, maxy));
    const i32 iy0 = Float_toInt(yy0);
    const i32 iy1 = Float_toInt(yy1);
    const f32 dy = yy0 - y0;

    x0 += dx0dy * dy;
    x1 += dx1dy * dy;
    u0 += du0dy * dy;
    u1 += du1dy * dy;
    v0 += dv0dy * dy;
    v1 += dv1dy * dy;

    if (x0 > x1)
    {
        f32 xt = x0; x0 = x1; x1 = xt;
        f32 ut = u0; u0 = u1; u1 = ut;
        f32 vt = v0; v0 = v1; v1 = vt;
        f32 dxtdy = dx0dy; dx0dy = dx1dy; dx1dy = dxtdy;
        f32 dutdy = du0dy; du0dy = du1dy; du1dy = dutdy;
        f32 dvtdy = dv0dy; dv0dy = dv1dy; dv1dy = dvtdy;
    }

    i32 height = iy1 - iy0;
    struct color *DstRow = Buffer.Pixels + Buffer.Pitch * iy0;

    while (height--)
    {
        const f32 xx0 = Ceil(Max(x0, minx));
        const f32 xx1 = Ceil(Min(x1, maxx));
        const i32 ix0 = Float_toInt(xx0);
        const i32 ix1 = Float_toInt(xx1);
        const f32 dx = xx0 - x0;

        f32 u = u0 + dudx * dx;
        f32 v = v0 + dvdx * dx;

        i32 width = ix1 - ix0;
        struct color *Dst = DstRow + ix0;
        
        while (width--)
        {
            const i32 iu = Floor_toInt(u);
            const i32 iv = Floor_toInt(v);
            struct color Color = *(Texture.Pixels + Texture.Pitch * iv + iu);
            if (Color.Value) *Dst = Color;

            ++Dst;

            u += dudx;
            v += dvdx;
        }

        DstRow += Buffer.Pitch;

        x0 += dx0dy;
        x1 += dx1dy;
        u0 += du0dy;
        v0 += dv0dy;
    }
}

void Draw_TriangleTextured(struct bitmap Buffer, struct bitmap Texture, struct vertex A, struct vertex B, struct vertex C)
{
    struct rect Clip = (struct rect){ .x = 0.0f, .y = 0.0f, .w = (f32)Buffer.Width, .h = (f32)Buffer.Height };

    f32 minx = Clip.x;
    f32 miny = Clip.y;
    f32 maxx = Clip.x + Clip.w;
    f32 maxy = Clip.y + Clip.h;

    A.x -= 0.5f;
    A.y -= 0.5f;
    B.x -= 0.5f;
    B.y -= 0.5f;
    C.x -= 0.5f;
    C.y -= 0.5f;

    if (A.y > B.y)
    {
        if (B.y > C.y) { struct vertex T = A; A = C; C = T; }
        else
        {
            if (A.y > C.y) { struct vertex T = A; A = B; B = C; C = T; }
            else           { struct vertex T = A; A = B; B = T; }
        }
    }
    else
    {
        if (B.y > C.y)
        {
            if (A.y > C.y) { struct vertex T = C; C = B; B = A; A = T; }
            else           { struct vertex T = B; B = C; C = T; }
        }
    }

    // bottom triangle
    const f32 invdy01 = 1.0f / (B.y - A.y);
    const f32 dx01dy = (B.x - A.x) * invdy01;
    const f32 du01dy = (B.u - A.u) * invdy01;
    const f32 dv01dy = (B.v - A.v) * invdy01;    

    const f32 invdy02 = 1.0f / (C.y - A.y);
    const f32 dx02dy = (C.x - A.x) * invdy02;
    const f32 du02dy = (C.u - A.u) * invdy02;
    const f32 dv02dy = (C.v - A.v) * invdy02;

    f32 x0 = A.x; f32 x1 = A.x;
    f32 y0 = A.y; f32 y1 = B.y;
    f32 u0 = A.u; f32 u1 = A.u;
    f32 v0 = A.v; f32 v1 = A.v;
    Draw__TriangleTexturedPerspectiveCorrect(Buffer, Texture, Clip, 
                                             x0, y0, x1, y1, 
                                             u0, v0, u1, v1, 
                                             dx01dy, dx02dy,
                                             du01dy, du02dy,
                                             dv01dy, dv02dy);

    // top triangle
    const f32 invdy12 = 1.0f / (C.y - B.y);
    const f32 dx12dy = (C.x - B.x) * invdy12;
    const f32 du12dy = (C.u - B.u) * invdy12;
    const f32 dv12dy = (C.v - B.v) * invdy12;

    x0 = A.x; x1 = B.x;
    y0 = B.y; y1 = C.y;
    u0 = A.u; u1 = B.u;
    v0 = A.v; v1 = B.v;

    x0 = A.x + (dx02dy * (B.y - A.y));
    u0 = A.u + (du02dy * (B.y - A.y));
    v0 = A.v + (dv02dy * (B.y - A.y));
    Draw__TriangleTexturedPerspectiveCorrect(Buffer, Texture, Clip, 
                                             x0, y0, x1, y1, 
                                             u0, v0, u1, v1, 
                                             dx02dy, dx12dy,
                                             du02dy, du12dy,
                                             dv02dy, dv12dy);
}
#endif



struct vertex
{
    vec3 Position;
    f32 u, v;
};

// struct {
//     f32 t, dt_dy;
// } x[2], invz[2], u[2], v[2];

// struct {
//     f32 t0, t1, dt0_dy, dt1_dy;
// } x, invz, u, v;

// struct {
//     struct {
//         f32 t, dt_dy;
//     } x, invz, u, v;
// } left, right;

inline void Draw__TriangleTextured3D(struct bitmap Buffer, const struct bitmap Texture, const struct rect Clip, 
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
    struct color *DstRow = Buffer.Pixels + Buffer.Pitch * iy0;

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
        struct color *Dst = DstRow + ix0;
        
        while (width--)
        {
            f32 z = 1.0f / invz;
            const i32 iu = Floor_toInt(u_z * z) & umask;
            const i32 iv = Floor_toInt(v_z * z) & vmask;
            struct color Color = *(Texture.Pixels + Texture.Pitch * iv + iu);
            if (Color.Value) *Dst = Color;
            ++Dst;

            invz += d1_dxz;
            u_z += du_dxz;
            v_z += dv_dxz;
        }

        DstRow += Buffer.Pitch;

        x0 += dx0_dy;
        x1 += dx1_dy;
        invz0 += d1_dyz0;
        u0_z0 += du0_dyz0;
        v0_z0 += dv0_dyz0;
    }
}

void Draw_TriangleTextured3D(struct bitmap Buffer, struct bitmap Texture, struct vertex A, struct vertex B, struct vertex C)
{
    struct rect Clip = (struct rect){ .x = 0.0f, .y = 0.0f, .w = (f32)Buffer.Width, .h = (f32)Buffer.Height };

    // clip z
    // TODO: proper clipping
    if (A.Position.z <= 0 || B.Position.z <= 0 || C.Position.z <= 0) return;

    // triangle winding order
    f32 Cross = (B.Position.x-A.Position.x) * (C.Position.y-A.Position.y) - 
                (B.Position.y-A.Position.y) * (C.Position.x-A.Position.x);
    if (Cross < 0.0f) return;

    // move pixel center
    A.Position.x -= 0.5f;
    A.Position.y -= 0.5f;
    B.Position.x -= 0.5f;
    B.Position.y -= 0.5f;
    C.Position.x -= 0.5f;
    C.Position.y -= 0.5f;

    // sort vertices vertically
    if (A.Position.y > B.Position.y)
    {
        if (B.Position.y > C.Position.y) { struct vertex T = A; A = C; C = T; }
        else
        {
            if (A.Position.y > C.Position.y) { struct vertex T = A; A = B; B = C; C = T; }
            else                             { struct vertex T = A; A = B; B = T; }
        }
    }
    else
    {
        if (B.Position.y > C.Position.y)
        {
            if (A.Position.y > C.Position.y) { struct vertex T = C; C = B; B = A; A = T; }
            else                             { struct vertex T = B; B = C; C = T; }
        }
    }

    if (C.Position.y < 0) return;

    const f32 d1_dy02 = 1.0f / (C.Position.y - A.Position.y);
    const f32 dx02_dy = (C.Position.x - A.Position.x) * d1_dy02;
    const f32 d1_dyz02 = (1.0f / C.Position.z - 1.0f / A.Position.z) * d1_dy02;
    const f32 du02_dyz02 = (C.u / C.Position.z - A.u / A.Position.z) * d1_dy02;
    const f32 dv02_dyz02 = (C.v / C.Position.z - A.v / A.Position.z) * d1_dy02;

    // bottom triangle
    if (B.Position.y > 0)
    {
        const f32 d1_dy01 = 1.0f / (B.Position.y - A.Position.y);
        const f32 dx01_dy = (B.Position.x - A.Position.x) * d1_dy01;
        const f32 d1_dyz01 = (1.0f / B.Position.z - 1.0f / A.Position.z) * d1_dy01;
        const f32 du01_dyz01 = (B.u / B.Position.z - A.u / A.Position.z) * d1_dy01;
        const f32 dv01_dyz01 = (B.v / B.Position.z - A.v / A.Position.z) * d1_dy01;

        f32 x0 = A.Position.x;
        f32 x1 = A.Position.x;
        f32 y0 = A.Position.y;
        f32 y1 = B.Position.y;
        f32 invz0 = 1.0f / A.Position.z;
        f32 invz1 = 1.0f / A.Position.z;
        f32 u0_z0 = A.u * invz0;
        f32 u1_z1 = A.u * invz1;
        f32 v0_z0 = A.v * invz0;
        f32 v1_z1 = A.v * invz1;

        Draw__TriangleTextured3D(Buffer, Texture, Clip, 
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
    const f32 du12_dyz12 = (C.u / C.Position.z - B.u / B.Position.z) * d1_dy12;
    const f32 dv12_dyz12 = (C.v / C.Position.z - B.v / B.Position.z) * d1_dy12;

    f32 x0 = A.Position.x;
    f32 x1 = B.Position.x;
    f32 y0 = B.Position.y;
    f32 y1 = C.Position.y;
    f32 invz0 = 1.0f / A.Position.z;
    f32 invz1 = 1.0f / B.Position.z;
    f32 u0_z0 = A.u * invz0;
    f32 u1_z1 = B.u * invz1;
    f32 v0_z0 = A.v * invz0;
    f32 v1_z1 = B.v * invz1;

    const f32 dy = (B.Position.y - A.Position.y);
    x0 = A.Position.x + (dx02_dy * dy);
    invz0 += d1_dyz02 * dy;
    u0_z0 += du02_dyz02 * dy;
    v0_z0 += dv02_dyz02 * dy;

    Draw__TriangleTextured3D(Buffer, Texture, Clip, 
                             y0, y1,
                             x0, x1, dx02_dy, dx12_dy,
                             invz0, invz1, d1_dyz02, d1_dyz12, 
                             u0_z0, u1_z1, du02_dyz02, du12_dyz12,
                             v0_z0, v1_z1, dv02_dyz02, dv12_dyz12);
}

void Draw_QuadTextured3D(struct bitmap Target, struct bitmap Texture, struct vertex A, struct vertex B, struct vertex C, struct vertex D)
{
    // check convex
    // check coplanar
    Draw_TriangleTextured3D(Target, Texture, A, B, C);
    Draw_TriangleTextured3D(Target, Texture, C, D, A);
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
            struct color Color = Bitmap_GetPixelFast(Texture, iuv[0], iuv[1]);
            if (Color.Value) Bitmap_SetPixelFast(Buffer, Color, ix, iy);
            
            uv = _mm_add_ps(uv, duvdx);
        }
        p = _mm_add_ps(p, dp);
    }
*/