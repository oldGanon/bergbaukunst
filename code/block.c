
typedef struct block
{
    u8 Id;
} block;

bool Block_BoxIntersect(block Block, ivec3 BlockPosition, box Box);
box Block_BoxIntersection(block Block, ivec3 BlockPosition, box Box);
f32 Block_TraceRay(block Block, ivec3 BlockPosition, vec3 RayOrigin, vec3 RayDirection);

/******************/
/* IMPLEMENTATION */
/******************/

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
    BLOCK_FACE_NONE,
    BLOCK_FACE_LEFT,   // -x WEST
    BLOCK_FACE_RIGHT,  // +x EAST
    BLOCK_FACE_FRONT,  // -y SOUTH
    BLOCK_FACE_BACK,   // +y NORTH
    BLOCK_FACE_BOTTOM, // +z UP
    BLOCK_FACE_TOP,    // -z DOWN
} block_face;

const vec3 BlockFace_Normal[7] = {
    [BLOCK_FACE_NONE]   = { 0, 0, 0 },
    [BLOCK_FACE_LEFT]   = {-1, 0, 0 },
    [BLOCK_FACE_RIGHT]  = {+1, 0, 0 },
    [BLOCK_FACE_FRONT]  = { 0,-1, 0 },
    [BLOCK_FACE_BACK]   = { 0,+1, 0 },
    [BLOCK_FACE_BOTTOM] = { 0, 0,-1 },
    [BLOCK_FACE_TOP]    = { 0, 0,+1 },
};

const ivec3 iBlockFace_Normal[7] = {
    [BLOCK_FACE_NONE]   = { 0, 0, 0 },
    [BLOCK_FACE_LEFT]   = {-1, 0, 0 },
    [BLOCK_FACE_RIGHT]  = {+1, 0, 0 },
    [BLOCK_FACE_FRONT]  = { 0,-1, 0 },
    [BLOCK_FACE_BACK]   = { 0,+1, 0 },
    [BLOCK_FACE_BOTTOM] = { 0, 0,-1 },
    [BLOCK_FACE_TOP]    = { 0, 0,+1 },
};



bool Block_BoxIntersect(block Block, ivec3 BlockPosition, box Box)
{
    if (!Block_Solid[Block.Id])
        return INFINITY;

    switch (Block.Id)
    {
        case BLOCK_ID_AIR: return INFINITY;

        default:
        {
            vec3 Min = iVec3_toVec3(BlockPosition);
            vec3 Max = Add(Min, Vec3_Set1(1));
            box BlockBox = { 
                .Min = Min,
                .Max = Max
            };
            return Box_Intersect(BlockBox, Box);
        } break;
    }
}

box Block_BoxIntersection(block Block, ivec3 BlockPosition, box Box)
{
    if (!Block_Solid[Block.Id])
        return Box_Empty();

    switch (Block.Id)
    {
        case BLOCK_ID_AIR: return Box_Empty();

        default:
        {
            vec3 Min = iVec3_toVec3(BlockPosition);
            vec3 Max = Add(Min, Vec3_Set1(1));
            box BlockBox = { 
                .Min = Min,
                .Max = Max
            };
            return Box_Intersection(BlockBox, Box);
        } break;
    }
}

f32 Block_TraceRay(block Block, ivec3 BlockPosition, vec3 RayOrigin, vec3 RayDirection)
{
    if (!Block_Solid[Block.Id])
        return INFINITY;

    switch (Block.Id)
    {
        case BLOCK_ID_AIR: return INFINITY;

        default:
        {
            vec3 Min = iVec3_toVec3(BlockPosition);
            vec3 Max = Add(Min, Vec3_Set1(1));
            box BlockBox = { 
                .Min = Min,
                .Max = Max
            };
            return Box_TraceRay(RayOrigin, RayDirection, BlockBox);
        } break;
    }
}
