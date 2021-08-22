
struct point
{
    f32 x, y;
};

struct line
{
    struct point a, b;
};

struct triangle
{
    struct point a, b, c;
};

struct rect
{
    f32 x, y, w, h;
};



bool Line_Clip(struct line *Line, const struct rect Rect)
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
    
    struct point a = Line->a;
    struct point b = Line->b;
    
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

inline struct triangle Triangle_SortX(struct triangle Triangle)
{
    if (Triangle.a.x > Triangle.b.x)
    {
        if (Triangle.b.x > Triangle.c.x)
        {// c b a
            struct point T = Triangle.a;
            Triangle.a = Triangle.c;
            Triangle.c = T;
        }
        else
        {
            if (Triangle.a.x > Triangle.c.x)
            {// b c a
                struct point T = Triangle.a;
                Triangle.a = Triangle.b;
                Triangle.b = Triangle.c;
                Triangle.c = T;
            }
            else
            {// b a c
                struct point T = Triangle.a;
                Triangle.a = Triangle.b;
                Triangle.b = T;
            }
        }
    }
    else
    {
        if (Triangle.b.x > Triangle.c.x)
        {
            if (Triangle.a.x > Triangle.c.x)
            {// c a b
                struct point T = Triangle.c;
                Triangle.c = Triangle.b;
                Triangle.b = Triangle.a;
                Triangle.a = T;
            }
            else
            {// a c b
                struct point T = Triangle.b;
                Triangle.b = Triangle.c;
                Triangle.c = T;                
            }
        }
    }

    return Triangle;
}

inline struct triangle Triangle_SortY(struct triangle Triangle)
{
    if (Triangle.a.y > Triangle.b.y)
    {
        if (Triangle.b.y > Triangle.c.y)
        {// c b a
            struct point T = Triangle.a;
            Triangle.a = Triangle.c;
            Triangle.c = T;
        }
        else
        {
            if (Triangle.a.y > Triangle.c.y)
            {// b c a
                struct point T = Triangle.a;
                Triangle.a = Triangle.b;
                Triangle.b = Triangle.c;
                Triangle.c = T;
            }
            else
            {// b a c
                struct point T = Triangle.a;
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
                struct point T = Triangle.c;
                Triangle.c = Triangle.b;
                Triangle.b = Triangle.a;
                Triangle.a = T;
            }
            else
            {// a c b
                struct point T = Triangle.b;
                Triangle.b = Triangle.c;
                Triangle.c = T;                
            }
        }
    }

    return Triangle;
}

inline struct triangle Triangle_SortY2(struct triangle Order, struct triangle Values)
{
    if (Order.a.y > Order.b.y)
    {
        if (Order.b.y > Order.c.y)
        {// c b a
            struct point T = Values.a;
            Values.a = Values.c;
            Values.c = T;
        }
        else
        {
            if (Order.a.y > Order.c.y)
            {// b c a
                struct point T = Values.a;
                Values.a = Values.b;
                Values.b = Values.c;
                Values.c = T;
            }
            else
            {// b a c
                struct point T = Values.a;
                Values.a = Values.b;
                Values.b = T;
            }
        }
    }
    else
    {
        if (Order.b.y > Order.c.y)
        {
            if (Order.a.y > Order.c.y)
            {// c a b
                struct point T = Values.c;
                Values.c = Values.b;
                Values.b = Values.a;
                Values.a = T;
            }
            else
            {// a c b
                struct point T = Values.b;
                Values.b = Values.c;
                Values.c = T;                
            }
        }
    }

    return Values;
}

i32 Triangle_Clip(const struct triangle Triangle, const struct rect Rect, struct triangle *Out)
{
    i32 TriangleCount = 0;
    Out[TriangleCount++] = Triangle;

    i32 T = TriangleCount;
    for (i32 i = 0; i < T; ++i)
    {
        f32 x = Rect.x;
        f32 ax = x - Out[i].a.x;
        f32 bx = x - Out[i].b.x;
        f32 cx = x - Out[i].c.x;
        u32 X = ((ax > 0) ? 1 : 0) | ((bx > 0) ? 2 : 0) | ((cx > 0) ? 4 : 0);

        switch (X)
        {
            case 0: continue;
            
            case 4: Out[i] = (struct triangle) { .a = Out[i].b, .b = Out[i].c, .c = Out[i].a };
            case 2: Out[i] = (struct triangle) { .a = Out[i].b, .b = Out[i].c, .c = Out[i].a };
            case 1:
            {
                struct point ab = { .x = x, .y = Lerp(Out[i].a.y, Out[i].b.y, (x - Out[i].a.x) / (Out[i].b.x - Out[i].a.x)) };
                struct point ac = { .x = x, .y = Lerp(Out[i].a.y, Out[i].c.y, (x - Out[i].a.x) / (Out[i].c.x - Out[i].a.x)) };
                Out[TriangleCount++] = (struct triangle) { .a = Out[i].c, .b = ac, .c = ab };
                Out[i].a = ab;
            } break;
            
            case 6: Out[i] = (struct triangle) { .a = Out[i].c, .b = Out[i].a, .c = Out[i].b };
            case 5: Out[i] = (struct triangle) { .a = Out[i].c, .b = Out[i].a, .c = Out[i].b };
            case 3:
            {
                Out[i].a = (struct point) { .x = x, .y = Lerp(Out[i].a.y, Out[i].c.y, (x - Out[i].a.x) / (Out[i].c.x - Out[i].a.x)) };
                Out[i].b = (struct point) { .x = x, .y = Lerp(Out[i].b.y, Out[i].c.y, (x - Out[i].b.x) / (Out[i].c.x - Out[i].b.x)) };
            } break;
            
            case 7: Out[i] = Out[--TriangleCount]; continue;
        }
    }

    T = TriangleCount;
    for (i32 i = 0; i < T; ++i)
    {
        f32 x = Rect.x + Rect.w;
        f32 ax = x - Out[i].a.x;
        f32 bx = x - Out[i].b.x;
        f32 cx = x - Out[i].c.x;
        u32 X = ((ax < 0) ? 1 : 0) | ((bx < 0) ? 2 : 0) | ((cx < 0) ? 4 : 0);

        switch (X)
        {
            case 0: continue;
            
            case 4: Out[i] = (struct triangle) { .a = Out[i].b, .b = Out[i].c, .c = Out[i].a };
            case 2: Out[i] = (struct triangle) { .a = Out[i].b, .b = Out[i].c, .c = Out[i].a };
            case 1:
            {
                struct point ab = { .x = x, .y = Lerp(Out[i].a.y, Out[i].b.y, (x - Out[i].a.x) / (Out[i].b.x - Out[i].a.x)) };
                struct point ac = { .x = x, .y = Lerp(Out[i].a.y, Out[i].c.y, (x - Out[i].a.x) / (Out[i].c.x - Out[i].a.x)) };
                Out[TriangleCount++] = (struct triangle) { .a = Out[i].c, .b = ac, .c = ab };
                Out[i].a = ab;
            } break;
            
            case 6: Out[i] = (struct triangle) { .a = Out[i].c, .b = Out[i].a, .c = Out[i].b };
            case 5: Out[i] = (struct triangle) { .a = Out[i].c, .b = Out[i].a, .c = Out[i].b };
            case 3:
            {
                Out[i].a = (struct point) { .x = x, .y = Lerp(Out[i].a.y, Out[i].c.y, (x - Out[i].a.x) / (Out[i].c.x - Out[i].a.x)) };
                Out[i].b = (struct point) { .x = x, .y = Lerp(Out[i].b.y, Out[i].c.y, (x - Out[i].b.x) / (Out[i].c.x - Out[i].b.x)) };
            } break;
            
            case 7: Out[i] = Out[--TriangleCount]; continue;
        }
    }

    T = TriangleCount;
    for (i32 i = 0; i < T; ++i)
    {
        f32 y = Rect.y;
        f32 ay = y - Out[i].a.y;
        f32 by = y - Out[i].b.y;
        f32 cy = y - Out[i].c.y;
        u32 Y = ((ay > 0) ? 1 : 0) | ((by > 0) ? 2 : 0) | ((cy > 0) ? 4 : 0);

        switch (Y)
        {
            case 0: continue;
            
            case 4: Out[i] = (struct triangle) { .a = Out[i].b, .b = Out[i].c, .c = Out[i].a };
            case 2: Out[i] = (struct triangle) { .a = Out[i].b, .b = Out[i].c, .c = Out[i].a };
            case 1:
            {
                struct point ab = { .x = Lerp(Out[i].a.x, Out[i].b.x, (y - Out[i].a.y) / (Out[i].b.y - Out[i].a.y)), .y = y };
                struct point ac = { .x = Lerp(Out[i].a.x, Out[i].c.x, (y - Out[i].a.y) / (Out[i].c.y - Out[i].a.y)), .y = y };
                Out[TriangleCount++] = (struct triangle) { .a = Out[i].c, .b = ac, .c = ab };
                Out[i].a = ab;
            } break;
            
            case 6: Out[i] = (struct triangle) { .a = Out[i].c, .b = Out[i].a, .c = Out[i].b };
            case 5: Out[i] = (struct triangle) { .a = Out[i].c, .b = Out[i].a, .c = Out[i].b };
            case 3:
            {
                Out[i].a = (struct point) { .x = Lerp(Out[i].a.x, Out[i].c.x, (y - Out[i].a.y) / (Out[i].c.y - Out[i].a.y)), .y = y };
                Out[i].b = (struct point) { .x = Lerp(Out[i].b.x, Out[i].c.x, (y - Out[i].b.y) / (Out[i].c.y - Out[i].b.y)), .y = y };
            } break;
            
            case 7: Out[i] = Out[--TriangleCount]; continue;
        }
    }

    T = TriangleCount;
    for (i32 i = 0; i < T; ++i)
    {
        f32 y = Rect.y + Rect.h;
        f32 ay = y - Out[i].a.y;
        f32 by = y - Out[i].b.y;
        f32 cy = y - Out[i].c.y;
        u32 Y = ((ay < 0) ? 1 : 0) | ((by < 0) ? 2 : 0) | ((cy < 0) ? 4 : 0);

        switch (Y)
        {
            case 0: continue;
            
            case 4: Out[i] = (struct triangle) { .a = Out[i].b, .b = Out[i].c, .c = Out[i].a };
            case 2: Out[i] = (struct triangle) { .a = Out[i].b, .b = Out[i].c, .c = Out[i].a };
            case 1:
            {
                struct point ab = { .x = Lerp(Out[i].a.x, Out[i].b.x, (y - Out[i].a.y) / (Out[i].b.y - Out[i].a.y)), .y = y };
                struct point ac = { .x = Lerp(Out[i].a.x, Out[i].c.x, (y - Out[i].a.y) / (Out[i].c.y - Out[i].a.y)), .y = y };
                Out[TriangleCount++] = (struct triangle) { .a = Out[i].c, .b = ac, .c = ab };
                Out[i].a = ab;
            } break;
            
            case 6: Out[i] = (struct triangle) { .a = Out[i].c, .b = Out[i].a, .c = Out[i].b };
            case 5: Out[i] = (struct triangle) { .a = Out[i].c, .b = Out[i].a, .c = Out[i].b };
            case 3:
            {
                Out[i].a = (struct point) { .x = Lerp(Out[i].a.x, Out[i].c.x, (y - Out[i].a.y) / (Out[i].c.y - Out[i].a.y)), .y = y };
                Out[i].b = (struct point) { .x = Lerp(Out[i].b.x, Out[i].c.x, (y - Out[i].b.y) / (Out[i].c.y - Out[i].b.y)), .y = y };
            } break;
            
            case 7: Out[i] = Out[--TriangleCount]; continue;
        }
    }

    return TriangleCount;
}
