
typedef struct point
{
    f32 x, y;
} point;

typedef struct line
{
    struct point a, b;
} line;

typedef struct triangle
{
    struct point a, b, c;
} triangle;

typedef struct rect
{
    f32 x, y, w, h;
} rect;



bool Line_Clip(line *Line, const rect Rect)
{
    f32 invdx = 1.0f / (Line->b.x - Line->a.x);
    f32 invdy = 1.0f / (Line->b.y - Line->a.y);

    f32 t0x = (Rect.x - Line->a.x) * invdx;
    f32 t0y = (Rect.y - Line->a.y) * invdy;
    f32 t1x = (Rect.x + Rect.w - Line->a.x) * invdx;
    f32 t1y = (Rect.y + Rect.h - Line->a.y) * invdy;

    f32 tmin = Max(Min(t0x, t1x), Min(t0y, t1y));
    f32 tmax = Min(Max(t0x, t1x), Max(t0y, t1y));

    if ((tmax < 0) || (tmin > tmax) || (1 < tmin))
        return false;
    
    point a = Line->a;
    point b = Line->b;
    
    if (0 < tmin)
    {
        Line->a.x = Lerp(a.x, b.x, tmin);
        Line->a.y = Lerp(a.y, b.y, tmin);
    }

    if (tmax < 1)
    {
        Line->b.x = Lerp(a.x, b.x, tmax);
        Line->b.y = Lerp(a.y, b.y, tmax);
    }

    return true;
}

inline triangle Triangle_SortY(triangle Triangle)
{
    if (Triangle.a.y > Triangle.b.y)
    {
        if (Triangle.b.y > Triangle.c.y)
        {// c b a
            point T = Triangle.a;
            Triangle.a = Triangle.c;
            Triangle.c = T;
        }
        else
        {
            if (Triangle.a.y > Triangle.c.y)
            {// b c a
                point T = Triangle.a;
                Triangle.a = Triangle.b;
                Triangle.b = Triangle.c;
                Triangle.c = T;
            }
            else
            {// b a c
                point T = Triangle.a;
                Triangle.a = Triangle.b;
                Triangle.b = T;
            }
        }
    }
    else
    {
        if (Triangle.b.y > Triangle.c.y)
        {
            if (Triangle.a.y > Triangle.c.y)
            {// c a b
                point T = Triangle.c;
                Triangle.c = Triangle.b;
                Triangle.b = Triangle.a;
                Triangle.a = T;
            }
            else
            {// a c b
                point T = Triangle.b;
                Triangle.b = Triangle.c;
                Triangle.c = T;                
            }
        }
    }

    return Triangle;
}
