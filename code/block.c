
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
    if (A.Min.x > B.Max.x) return false;
    if (A.Min.y > B.Max.y) return false;
    if (A.Min.z > B.Max.z) return false;
    if (B.Min.x > A.Max.x) return false;
    if (B.Min.y > A.Max.y) return false;
    if (B.Min.z > A.Max.z) return false;
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

//
//
//

quad Block_FaceQuad(vec3 Position, vec3 Right, vec3 Up, vec2 UV, vec2 U, vec2 V, f32 *Shadow)
{
    return (quad){
        .Verts = {
            [0] = {
                .Position = Position,
                .TexCoord = UV,
                .Shadow = Shadow[0],
            },
            [1] = {
                .Position = Vec3_Add(Position, Right),
                .TexCoord = Vec2_Add(UV, U),
                .Shadow = Shadow[1],
            },
            [2] = {
                .Position = Vec3_Add(Position, Vec3_Add(Right, Up)),
                .TexCoord = Vec2_Add(UV, Vec2_Add(U, V)),
                .Shadow = Shadow[2],
            },
            [3] = {
                .Position = Vec3_Add(Position, Up),
                .TexCoord = Vec2_Add(UV, V),
                .Shadow = Shadow[3],
            },
        },
    };
}

quad Block_LeftFace(vec3 Position, u32 Index, const block_group *BlockGroup)
{
    f32 ShadowValues[3][3] = {
        { BlockGroup->Blocks[0][2][0].Shadow, BlockGroup->Blocks[0][1][0].Shadow, BlockGroup->Blocks[0][0][0].Shadow, },
        { BlockGroup->Blocks[1][2][0].Shadow, BlockGroup->Blocks[1][1][0].Shadow, BlockGroup->Blocks[1][0][0].Shadow, },
        { BlockGroup->Blocks[2][2][0].Shadow, BlockGroup->Blocks[2][1][0].Shadow, BlockGroup->Blocks[2][0][0].Shadow, },
    };
    f32 Shadow[4] = {
        (ShadowValues[0][0] + ShadowValues[0][1] + ShadowValues[1][0] + ShadowValues[1][1]) * (0.25f / 15.0f),
        (ShadowValues[0][1] + ShadowValues[0][2] + ShadowValues[1][1] + ShadowValues[1][2]) * (0.25f / 15.0f),
        (ShadowValues[1][1] + ShadowValues[1][2] + ShadowValues[2][1] + ShadowValues[2][2]) * (0.25f / 15.0f),
        (ShadowValues[1][0] + ShadowValues[1][1] + ShadowValues[2][0] + ShadowValues[2][1]) * (0.25f / 15.0f),
    };
    vec3 Pos = Vec3_Add(Position, (vec3){0,1,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){0,-1,0}, (vec3){0,0,1}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    if ((ShadowValues[0][0] + ShadowValues[2][2]) > (ShadowValues[0][2] + ShadowValues[2][0]))
        Face = Quad_RotateVerts(Face);
    return Face;
}

quad Block_RightFace(vec3 Position, u32 Index, const block_group *BlockGroup)
{
    f32 ShadowValues[3][3] = {
        { BlockGroup->Blocks[0][0][2].Shadow, BlockGroup->Blocks[0][1][2].Shadow, BlockGroup->Blocks[0][2][2].Shadow, },
        { BlockGroup->Blocks[1][0][2].Shadow, BlockGroup->Blocks[1][1][2].Shadow, BlockGroup->Blocks[1][2][2].Shadow, },
        { BlockGroup->Blocks[2][0][2].Shadow, BlockGroup->Blocks[2][1][2].Shadow, BlockGroup->Blocks[2][2][2].Shadow, },
    };
    f32 Shadow[4] = {
        (ShadowValues[0][0] + ShadowValues[0][1] + ShadowValues[1][0] + ShadowValues[1][1]) * (0.25f / 15.0f),
        (ShadowValues[0][1] + ShadowValues[0][2] + ShadowValues[1][1] + ShadowValues[1][2]) * (0.25f / 15.0f),
        (ShadowValues[1][1] + ShadowValues[1][2] + ShadowValues[2][1] + ShadowValues[2][2]) * (0.25f / 15.0f),
        (ShadowValues[1][0] + ShadowValues[1][1] + ShadowValues[2][0] + ShadowValues[2][1]) * (0.25f / 15.0f),
    };
    vec3 Pos = Vec3_Add(Position, (vec3){1,0,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){0,1,0}, (vec3){0,0,1}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    if ((ShadowValues[0][0] + ShadowValues[2][2]) > (ShadowValues[0][2] + ShadowValues[2][0]))
        Face = Quad_RotateVerts(Face);
    return Face;
}

quad Block_FrontFace(vec3 Position, u32 Index, const block_group *BlockGroup)
{
    f32 ShadowValues[3][3] = {
        { BlockGroup->Blocks[0][0][0].Shadow, BlockGroup->Blocks[0][0][1].Shadow, BlockGroup->Blocks[0][0][2].Shadow, },
        { BlockGroup->Blocks[1][0][0].Shadow, BlockGroup->Blocks[1][0][1].Shadow, BlockGroup->Blocks[1][0][2].Shadow, },
        { BlockGroup->Blocks[2][0][0].Shadow, BlockGroup->Blocks[2][0][1].Shadow, BlockGroup->Blocks[2][0][2].Shadow, },
    };
    f32 Shadow[4] = {
        (ShadowValues[0][0] + ShadowValues[0][1] + ShadowValues[1][0] + ShadowValues[1][1]) * (0.25f / 15.0f),
        (ShadowValues[0][1] + ShadowValues[0][2] + ShadowValues[1][1] + ShadowValues[1][2]) * (0.25f / 15.0f),
        (ShadowValues[1][1] + ShadowValues[1][2] + ShadowValues[2][1] + ShadowValues[2][2]) * (0.25f / 15.0f),
        (ShadowValues[1][0] + ShadowValues[1][1] + ShadowValues[2][0] + ShadowValues[2][1]) * (0.25f / 15.0f),
    };
    vec3 Pos = Vec3_Add(Position, (vec3){0,0,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){1,0,0}, (vec3){0,0,1}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    if ((ShadowValues[0][0] + ShadowValues[2][2]) > (ShadowValues[0][2] + ShadowValues[2][0]))
        Face = Quad_RotateVerts(Face);
    return Face;
}

quad Block_BackFace(vec3 Position, u32 Index, const block_group *BlockGroup)
{
    f32 ShadowValues[3][3] = {
        { BlockGroup->Blocks[0][2][2].Shadow, BlockGroup->Blocks[0][2][1].Shadow, BlockGroup->Blocks[0][2][0].Shadow, },
        { BlockGroup->Blocks[1][2][2].Shadow, BlockGroup->Blocks[1][2][1].Shadow, BlockGroup->Blocks[1][2][0].Shadow, },
        { BlockGroup->Blocks[2][2][2].Shadow, BlockGroup->Blocks[2][2][1].Shadow, BlockGroup->Blocks[2][2][0].Shadow, },
    };
    f32 Shadow[4] = {
        (ShadowValues[0][0] + ShadowValues[0][1] + ShadowValues[1][0] + ShadowValues[1][1]) * (0.25f / 15.0f),
        (ShadowValues[0][1] + ShadowValues[0][2] + ShadowValues[1][1] + ShadowValues[1][2]) * (0.25f / 15.0f),
        (ShadowValues[1][1] + ShadowValues[1][2] + ShadowValues[2][1] + ShadowValues[2][2]) * (0.25f / 15.0f),
        (ShadowValues[1][0] + ShadowValues[1][1] + ShadowValues[2][0] + ShadowValues[2][1]) * (0.25f / 15.0f),
    };
    vec3 Pos = Vec3_Add(Position, (vec3){1,1,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){-1,0,0}, (vec3){0,0,1}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    if ((ShadowValues[0][0] + ShadowValues[2][2]) > (ShadowValues[0][2] + ShadowValues[2][0]))
        Face = Quad_RotateVerts(Face);
    return Face;
}

quad Block_BottomFace(vec3 Position, u32 Index, const block_group *BlockGroup)
{
    f32 ShadowValues[3][3] = {
        { BlockGroup->Blocks[0][2][0].Shadow, BlockGroup->Blocks[0][2][1].Shadow, BlockGroup->Blocks[0][2][2].Shadow, },
        { BlockGroup->Blocks[0][1][0].Shadow, BlockGroup->Blocks[0][1][1].Shadow, BlockGroup->Blocks[0][1][2].Shadow, },
        { BlockGroup->Blocks[0][0][0].Shadow, BlockGroup->Blocks[0][0][1].Shadow, BlockGroup->Blocks[0][0][2].Shadow, },
    };
    f32 Shadow[4] = {
        (ShadowValues[0][0] + ShadowValues[0][1] + ShadowValues[1][0] + ShadowValues[1][1]) * (0.25f / 15.0f),
        (ShadowValues[0][1] + ShadowValues[0][2] + ShadowValues[1][1] + ShadowValues[1][2]) * (0.25f / 15.0f),
        (ShadowValues[1][1] + ShadowValues[1][2] + ShadowValues[2][1] + ShadowValues[2][2]) * (0.25f / 15.0f),
        (ShadowValues[1][0] + ShadowValues[1][1] + ShadowValues[2][0] + ShadowValues[2][1]) * (0.25f / 15.0f),
    };
    vec3 Pos = Vec3_Add(Position, (vec3){0,1,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){1,0,0}, (vec3){0,-1,0}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    if ((ShadowValues[0][0] + ShadowValues[2][2]) > (ShadowValues[0][2] + ShadowValues[2][0]))
        Face = Quad_RotateVerts(Face);
    return Face;
}

quad Block_TopFace(vec3 Position, u32 Index, const block_group *BlockGroup)
{
    f32 ShadowValues[3][3] = {
        { BlockGroup->Blocks[2][0][0].Shadow, BlockGroup->Blocks[2][0][1].Shadow, BlockGroup->Blocks[2][0][2].Shadow, },
        { BlockGroup->Blocks[2][1][0].Shadow, BlockGroup->Blocks[2][1][1].Shadow, BlockGroup->Blocks[2][1][2].Shadow, },
        { BlockGroup->Blocks[2][2][0].Shadow, BlockGroup->Blocks[2][2][1].Shadow, BlockGroup->Blocks[2][2][2].Shadow, },
    };
    f32 Shadow[4] = {
        (ShadowValues[0][0] + ShadowValues[0][1] + ShadowValues[1][0] + ShadowValues[1][1]) * (0.25f / 15.0f),
        (ShadowValues[0][1] + ShadowValues[0][2] + ShadowValues[1][1] + ShadowValues[1][2]) * (0.25f / 15.0f),
        (ShadowValues[1][1] + ShadowValues[1][2] + ShadowValues[2][1] + ShadowValues[2][2]) * (0.25f / 15.0f),
        (ShadowValues[1][0] + ShadowValues[1][1] + ShadowValues[2][0] + ShadowValues[2][1]) * (0.25f / 15.0f),
    };
    vec3 Pos = Vec3_Add(Position, (vec3){0,0,1});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){1,0,0}, (vec3){0,1,0}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    if ((ShadowValues[0][0] + ShadowValues[2][2]) > (ShadowValues[0][2] + ShadowValues[2][0]))
        Face = Quad_RotateVerts(Face);
    return Face;
}

void Block_AddQuadsToMesh(quad_mesh *Mesh, vec3 Position, const block_group *BlockGroup, 
                          u32 Left, u32 Right, u32 Front, u32 Back, u32 Bottom, u32 Top)
{
    // Left
    if (!Block_Opaque[BlockGroup->Blocks[1][1][0].Id])
    {
        Mesh_AddQuad(Mesh, Block_LeftFace(Position, Left, BlockGroup));
    }

    // Right
    if (!Block_Opaque[BlockGroup->Blocks[1][1][2].Id])
    {
        Mesh_AddQuad(Mesh, Block_RightFace(Position, Right, BlockGroup));
    }

    // Front
    if (!Block_Opaque[BlockGroup->Blocks[1][0][1].Id])
    {
        Mesh_AddQuad(Mesh, Block_FrontFace(Position, Front, BlockGroup));
    }

    // Back
    if (!Block_Opaque[BlockGroup->Blocks[1][2][1].Id])
    {
        Mesh_AddQuad(Mesh, Block_BackFace(Position, Back, BlockGroup));
    }

    // Bottom
    if (!Block_Opaque[BlockGroup->Blocks[0][1][1].Id])
    {
        Mesh_AddQuad(Mesh, Block_BottomFace(Position, Bottom, BlockGroup));
    }

    // Top
    if (!Block_Opaque[BlockGroup->Blocks[2][1][1].Id])
    {
        Mesh_AddQuad(Mesh, Block_TopFace(Position, Top, BlockGroup));
    }
}
