
typedef struct trace_result
{
    block Block;
    block_face BlockFace;

    ivec3 BlockPosition;
    ivec3 FreePosition;
} trace_result;

typedef block get_block_func(void *Data, ivec3 Position);

f32 Phys_TraceRay(get_block_func *GetBlock, void *Data, vec3 RayOrigin, vec3 RayDirection, f32 RayLength, trace_result *Result)
{
    vec3 RayPosition = RayOrigin;
    vec3 RaySign = Sign(RayDirection);

    f32 t = 0;
    vec3 tDelta = Min(Inverse(Abs(RayDirection)), Vec3_Set1(0x100000));
    vec3 tInit = Fract(RayPosition);
    if (RaySign.x > 0) tInit.x = (1 - tInit.x);
    if (RaySign.y > 0) tInit.y = (1 - tInit.y);
    if (RaySign.z > 0) tInit.z = (1 - tInit.z);
    vec3 tMax = Mul(tInit, tDelta);

    block_face LastFace = BLOCK_FACE_NONE;

    for (;;)
    {
        if (t > RayLength)
            return RayLength;
        if ((RaySign.z < 0) && (RayPosition.z < 0))
            return INFINITY;
        if ((RaySign.z > 0) && (RayPosition.z > CHUNK_HEIGHT - 1))
            return INFINITY;

        ivec3 BlockPosition = Vec3_FloorToIVec3(RayPosition);
        block Block = GetBlock(Data, BlockPosition);
        if (Block_TraceRay(Block, BlockPosition, RayOrigin, RayDirection) < RayLength)
        {
            ivec3 FreePosition = Add(BlockPosition, iBlockFace_Normal[LastFace]);
            *Result = (trace_result) {
                .Block = Block,
                .BlockFace = LastFace,
                .BlockPosition = BlockPosition,
                .FreePosition = FreePosition,
            };
            return t;
        }

        if ((tMax.x <= tMax.y) && (tMax.x <= tMax.z))
        {
            t = tMax.x;
            tMax.x += tDelta.x;
            RayPosition.x += RaySign.x;
            if(RaySign.x > 0) LastFace = BLOCK_FACE_LEFT;
            else              LastFace = BLOCK_FACE_RIGHT;
        }
        else if ((tMax.y <= tMax.x) && (tMax.y <= tMax.z))
        {
            t = tMax.y;
            tMax.y += tDelta.y;
            RayPosition.y += RaySign.y;
            if (RaySign.y > 0) LastFace = BLOCK_FACE_FRONT;
            else               LastFace = BLOCK_FACE_BACK;
        }
        else if ((tMax.z <= tMax.x) && (tMax.z <= tMax.y))
        {
            t = tMax.z;
            tMax.z += tDelta.z;
            RayPosition.z += RaySign.z;
            if (RaySign.z > 0) LastFace = BLOCK_FACE_BOTTOM;
            else               LastFace = BLOCK_FACE_TOP;
        }
    }
}


box Phys_BoxIntersection(get_block_func *GetBlock, void *Data, box Box)
{
    ivec3 Min = Vec3_FloorToIVec3(Box.Min);
    ivec3 Max = Vec3_CeilToIVec3(Box.Max);

    box Intersection = BOX_EMPTY;

    for (i32 z = Min.z; z < Max.z; ++z)
    for (i32 y = Min.y; y < Max.y; ++y)
    for (i32 x = Min.x; x < Max.x; ++x)
    {
        ivec3 BlockPosition = { x, y, z };
        block Block = GetBlock(Data, BlockPosition);
        Intersection = Box_Union(Intersection, Block_BoxIntersection(Block, BlockPosition, Box));
    }

    return Intersection;
}

vec3 Phys_CheckMoveBox(get_block_func *GetBlock, void *Data, box Box, vec3 Move)
{
    {
        box MoveBox = Box;
        if (Move.x < 0) { MoveBox.Max.x = MoveBox.Min.x; MoveBox.Min.x += Move.x;}
        if (Move.x > 0) { MoveBox.Min.x = MoveBox.Max.x; MoveBox.Max.x += Move.x;}
        box CollisionBox = Phys_BoxIntersection(GetBlock, Data, MoveBox);
        if (!Box_Empty(CollisionBox))
        {
            if (Move.x < 0) Move.x = CollisionBox.Max.x - Box.Min.x;
            if (Move.x > 0) Move.x = CollisionBox.Min.x - Box.Max.x;
        }
        Box.Min.x += Move.x;
        Box.Max.x += Move.x;
    }

    {
        box MoveBox = Box;
        if (Move.y < 0) { MoveBox.Max.y = MoveBox.Min.y; MoveBox.Min.y += Move.y; }
        if (Move.y > 0) { MoveBox.Min.y = MoveBox.Max.y; MoveBox.Max.y += Move.y; }
        box CollisionBox = Phys_BoxIntersection(GetBlock, Data, MoveBox);
        if (!Box_Empty(CollisionBox))
        {
            if (Move.y < 0) Move.y = CollisionBox.Max.y - Box.Min.y;
            if (Move.y > 0) Move.y = CollisionBox.Min.y - Box.Max.y;
        }
        Box.Min.y += Move.y;
        Box.Max.y += Move.y;
    }

    {
        box MoveBox = Box;
        if (Move.z < 0) { MoveBox.Max.z = MoveBox.Min.z; MoveBox.Min.z += Move.z; }
        if (Move.z > 0) { MoveBox.Min.z = MoveBox.Max.z; MoveBox.Max.z += Move.z; }
        box CollisionBox = Phys_BoxIntersection(GetBlock, Data, MoveBox);
        if (!Box_Empty(CollisionBox))
        {
            if (Move.z < 0) Move.z = CollisionBox.Max.z - Box.Min.z;
            if (Move.z > 0) Move.z = CollisionBox.Min.z - Box.Max.z;
        }
        Box.Min.z += Move.z;
        Box.Max.z += Move.z;
    }

    return Move;
}
