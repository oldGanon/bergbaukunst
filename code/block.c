
enum
{
    BLOCK_ID_AIR = 0,
    BLOCK_ID_DIRT = 1,
    BLOCK_ID_GRAS = 2,
    BLOCK_ID_STONE = 3,
    BLOCK_ID_COBBLE = 4,
    BLOCK_ID_WOOD = 5,
    BLOCK_ID_LEAVES = 6,
    BLOCK_ID_SAND = 7,
} block_id;

typedef struct block
{
    u8 Id;
    u8 Shadow;
} block;

typedef struct block_group
{
    block Blocks[3][3][3];
} block_group;

const u8 Block_Solid[256] = {
    [BLOCK_ID_DIRT] = true,
    [BLOCK_ID_GRAS] = true,
    [BLOCK_ID_STONE] = true,
    [BLOCK_ID_COBBLE] = true,
    [BLOCK_ID_WOOD] = true,
    [BLOCK_ID_LEAVES] = true,
    [BLOCK_ID_SAND] = true,
};

const u8 Block_Opaque[256] = {
    [BLOCK_ID_DIRT] = true,
    [BLOCK_ID_GRAS] = true,
    [BLOCK_ID_STONE] = true,
    [BLOCK_ID_COBBLE] = true,
    [BLOCK_ID_WOOD] = true,
    [BLOCK_ID_SAND] = true,
};

typedef enum block_face
{
    BLOCK_FACE_LEFT,   // -x WEST
    BLOCK_FACE_RIGHT,  // +x EAST
    BLOCK_FACE_FRONT,  // -y SOUTH
    BLOCK_FACE_BACK,   // +y NORTH
    BLOCK_FACE_BOTTOM, // +z UP
    BLOCK_FACE_TOP,    // -z DOWN
} block_face;

const vec3 BlockFace_Normal[6] = {
    [BLOCK_FACE_LEFT]   = {-1, 0, 0 },
    [BLOCK_FACE_RIGHT]  = {+1, 0, 0 },
    [BLOCK_FACE_FRONT]  = { 0,-1, 0 },
    [BLOCK_FACE_BACK]   = { 0,+1, 0 },
    [BLOCK_FACE_BOTTOM] = { 0, 0,-1 },
    [BLOCK_FACE_TOP]    = { 0, 0,+1 },
};

typedef struct box
{
    vec3 Min, Max;
} box;

#define BOX_EMPTY (box){.Min = {INFINITY,INFINITY,INFINITY},.Max = {-INFINITY,-INFINITY,-INFINITY}}

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

f32 Block_TraceRay(block Block, vec3 BlockPosition, vec3 RayOrigin, vec3 RayDirection)
{
    if (!Block_Solid[Block.Id])
        return INFINITY;

    switch (Block.Id)
    {
        case BLOCK_ID_AIR: return INFINITY;

        default:
        {
            box BlockBox = {
                .Min = BlockPosition,
                .Max = Add(BlockPosition, Vec3_Set1(1))
            };
            return Box_TraceRay(RayOrigin, RayDirection, BlockBox);
        } break;
    }
}

bool Box_Empty(box A)
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

bool Block_BoxIntersect(block Block, vec3 BlockPosition, box Box)
{
    if (!Block_Solid[Block.Id])
        return INFINITY;

    switch (Block.Id)
    {
        case BLOCK_ID_AIR: return INFINITY;

        default:
        {
            box BlockBox = {
                .Min = BlockPosition,
                .Max = Add(BlockPosition, Vec3_Set1(1))
            };
            return Box_Intersect(BlockBox, Box);
        } break;
    }
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

box Block_BoxIntersection(block Block, vec3 BlockPosition, box Box)
{
    if (!Block_Solid[Block.Id])
        return BOX_EMPTY;

    switch (Block.Id)
    {
        case BLOCK_ID_AIR: return BOX_EMPTY;

        default:
        {
            box BlockBox = {
                .Min = BlockPosition,
                .Max = Add(BlockPosition, Vec3_Set1(1))
            };
            return Box_Intersection(BlockBox, Box);
        } break;
    }
}
