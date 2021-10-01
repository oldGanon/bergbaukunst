
typedef struct box
{
    vec3 Min, Max;
} box;

box Box_Empty(void);
bool Box_IsEmpty(box A);
vec3 Box_Dimension(box A);
box Box_Move(box A, vec3 B);
bool Box_Intersect(box A, box B);
box Box_Union(box A, box B);
box Box_Intersection(box A, box B);
f32 Box_TraceRay(vec3 RayOrigin, vec3 RayDirection, box Box);

/******************/
/* IMPLEMENTATION */
/******************/

box Box_Empty(void)
{
    return (box){
        .Min = { INFINITY, INFINITY, INFINITY },
        .Max = {-INFINITY,-INFINITY,-INFINITY }
    };
}

bool Box_IsEmpty(box A)
{
    if (A.Min.x >= A.Max.x) return true;
    if (A.Min.y >= A.Max.y) return true;
    if (A.Min.z >= A.Max.z) return true;
    return false;
}

vec3 Box_Dimension(box A)
{
    return Vec3_Sub(A.Max, A.Min);
}

box Box_Move(box A, vec3 B)
{
    A.Min = Vec3_Add(A.Min, B);
    A.Max = Vec3_Add(A.Max, B);
    return A;
}

bool Box_Intersect(box A, box B)
{
    if (A.Min.x >= B.Max.x) return false;
    if (A.Min.y >= B.Max.y) return false;
    if (A.Min.z >= B.Max.z) return false;
    if (B.Min.x >= A.Max.x) return false;
    if (B.Min.y >= A.Max.y) return false;
    if (B.Min.z >= A.Max.z) return false;
    return true;
}

box Box_Union(box A, box B)
{
    return (box){
        .Min = Min(A.Min, B.Min),
        .Max = Max(A.Max, B.Max),
    };
}

box Box_Intersection(box A, box B)
{
    return (box){
        .Min = Max(A.Min, B.Min),
        .Max = Min(A.Max, B.Max),
    };
}

f32 Box_TraceRay(vec3 RayOrigin, vec3 RayDirection, box Box)
{
    vec3 Inverse = Inverse(RayDirection);
    vec3 t0 = Mul(Sub(Box.Min, RayOrigin), Inverse);
    vec3 t1 = Mul(Sub(Box.Max, RayOrigin), Inverse);
    vec3 tmin3 = Min(t0, t1);
    vec3 tmax3 = Max(t0, t1);
    f32 tmin = Max(Max(tmin3.x, tmin3.y), tmin3.z);
    f32 tmax = Min(Min(tmax3.x, tmax3.y), tmax3.z);
    if (tmax < 0.0f || tmax < tmin) return INFINITY;
    if (tmin < 0.0f && tmax > 0.0f) return 0.0;
    return tmin;
}
