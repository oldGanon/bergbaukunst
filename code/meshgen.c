
quad Block_FaceQuad(vec3 Position, vec3 Right, vec3 Up, vec2 UV, vec2 U, vec2 V, vec4 Shadow)
{
    quad Face = {
        .Verts = {
            [0] = {
                .Position = Position,
                .TexCoord = UV,
                .Shadow = Shadow.E[0],
            },
            [1] = {
                .Position = Vec3_Add(Position, Right),
                .TexCoord = Vec2_Add(UV, U),
                .Shadow = Shadow.E[1],
            },
            [2] = {
                .Position = Vec3_Add(Position, Vec3_Add(Right, Up)),
                .TexCoord = Vec2_Add(UV, Vec2_Add(U, V)),
                .Shadow = Shadow.E[2],
            },
            [3] = {
                .Position = Vec3_Add(Position, Up),
                .TexCoord = Vec2_Add(UV, V),
                .Shadow = Shadow.E[3],
            },
        },
    };

    if ((Shadow.E[0] + Shadow.E[2]) > (Shadow.E[1] + Shadow.E[3]))
        Face = Quad_RotateVerts(Face);

    return Face;
}

void Block_AddLeftFaceToMesh(quad_mesh *Mesh, ivec3 Position, u8 LeftId, u8 BlockId, vec4 Shadow)
{
    u8 Index = 0;
    switch (BlockId)
    {
        case BLOCK_ID_DIRT: Index = 2; break;
        case BLOCK_ID_GRAS: Index = 1; break;
        case BLOCK_ID_STONE: Index = 5; break;
        case BLOCK_ID_COBBLE: Index = 4; break;
        case BLOCK_ID_WOOD: Index = 7; break;
        case BLOCK_ID_LEAVES: Index = 3; break;
        case BLOCK_ID_SAND: Index = 6; break;
        case BLOCK_ID_WATER:
        {
            Index = 8;
            if (LeftId == BLOCK_ID_WATER)
                return;
        } break;
    }
    vec3 Pos = Vec3_Add(iVec3_toVec3(Position), (vec3){0,1,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){0,-1,0}, (vec3){0,0,1}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    Mesh_AddQuad(Mesh, Face);
}

void Block_AddRightFaceToMesh(quad_mesh *Mesh, ivec3 Position, u8 RightId, u8 BlockId, vec4 Shadow)
{
    u8 Index = 0;
    switch (BlockId)
    {
        case BLOCK_ID_DIRT: Index = 2; break;
        case BLOCK_ID_GRAS: Index = 1; break;
        case BLOCK_ID_STONE: Index = 5; break;
        case BLOCK_ID_COBBLE: Index = 4; break;
        case BLOCK_ID_WOOD: Index = 7; break;
        case BLOCK_ID_LEAVES: Index = 3; break;
        case BLOCK_ID_SAND: Index = 6; break;
        case BLOCK_ID_WATER:
        {
            Index = 8;
            if (RightId == BLOCK_ID_WATER)
                return;
        } break;
    }
    vec3 Pos = Vec3_Add(iVec3_toVec3(Position), (vec3){1,0,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){0,1,0}, (vec3){0,0,1}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    Mesh_AddQuad(Mesh, Face);
}

void Block_AddFrontFaceToMesh(quad_mesh *Mesh, ivec3 Position, u8 FrontId, u8 BlockId, vec4 Shadow)
{
    u8 Index = 0;
    switch (BlockId)
    {
        case BLOCK_ID_DIRT: Index = 2; break;
        case BLOCK_ID_GRAS: Index = 1; break;
        case BLOCK_ID_STONE: Index = 5; break;
        case BLOCK_ID_COBBLE: Index = 4; break;
        case BLOCK_ID_WOOD: Index = 7; break;
        case BLOCK_ID_LEAVES: Index = 3; break;
        case BLOCK_ID_SAND: Index = 6; break;
        case BLOCK_ID_WATER:
        {
            Index = 8;
            if (FrontId == BLOCK_ID_WATER)
                return;
        } break;
    }
    vec3 Pos = Vec3_Add(iVec3_toVec3(Position), (vec3){0,0,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){1,0,0}, (vec3){0,0,1}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    Mesh_AddQuad(Mesh, Face);
}

void Block_AddBackFaceToMesh(quad_mesh *Mesh, ivec3 Position, u8 BackId, u8 BlockId, vec4 Shadow)
{
    u8 Index = 0;
    switch (BlockId)
    {
        case BLOCK_ID_DIRT: Index = 2; break;
        case BLOCK_ID_GRAS: Index = 1; break;
        case BLOCK_ID_STONE: Index = 5; break;
        case BLOCK_ID_COBBLE: Index = 4; break;
        case BLOCK_ID_WOOD: Index = 7; break;
        case BLOCK_ID_LEAVES: Index = 3; break;
        case BLOCK_ID_SAND: Index = 6; break;
        case BLOCK_ID_WATER:
        {
            Index = 8;
            if (BackId == BLOCK_ID_WATER)
                return;
        } break;
    }
    vec3 Pos = Vec3_Add(iVec3_toVec3(Position), (vec3){1,1,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){-1,0,0}, (vec3){0,0,1}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    Mesh_AddQuad(Mesh, Face);
}

void Block_AddBottomFaceToMesh(quad_mesh *Mesh, ivec3 Position, u8 BottomId, u8 BlockId, vec4 Shadow)
{
    u8 Index = 0;
    switch (BlockId)
    {
        case BLOCK_ID_DIRT: Index = 2; break;
        case BLOCK_ID_GRAS: Index = 2; break;
        case BLOCK_ID_STONE: Index = 5; break;
        case BLOCK_ID_COBBLE: Index = 4; break;
        case BLOCK_ID_WOOD: Index = 15; break;
        case BLOCK_ID_LEAVES: Index = 3; break;
        case BLOCK_ID_SAND: Index = 6; break;
        case BLOCK_ID_WATER:
        {
            Index = 8;
            if (BottomId == BLOCK_ID_WATER)
                return;
        } break;
    }
    vec3 Pos = Vec3_Add(iVec3_toVec3(Position), (vec3){0,1,0});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){1,0,0}, (vec3){0,-1,0}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    Mesh_AddQuad(Mesh, Face);
}

void Block_AddTopFaceToMesh(quad_mesh *Mesh, ivec3 Position, u8 TopId, u8 BlockId, vec4 Shadow)
{
    u8 Index = 0;
    switch (BlockId)
    {
        case BLOCK_ID_DIRT: Index = 2; break;
        case BLOCK_ID_GRAS: Index = 0; break;
        case BLOCK_ID_STONE: Index = 5; break;
        case BLOCK_ID_COBBLE: Index = 4; break;
        case BLOCK_ID_WOOD: Index = 15; break;
        case BLOCK_ID_LEAVES: Index = 3; break;
        case BLOCK_ID_SAND: Index = 6; break;
        case BLOCK_ID_WATER:
        {
            Index = 8;
            if (TopId == BLOCK_ID_WATER)
                return;
        } break;
    }
    vec3 Pos = Vec3_Add(iVec3_toVec3(Position), (vec3){0,0,1});
    vec2 UV = { (Index & 7) * 16.0f, (Index >> 3) * 16.0f };
    quad Face = Block_FaceQuad(Pos, (vec3){1,0,0}, (vec3){0,1,0}, UV, (vec2){16,0}, (vec2){0,16}, Shadow);
    Mesh_AddQuad(Mesh, Face);
}

void View_GenerateChunkMesh(view *View, ivec2 ChunkPosition)
{
    // get chunks
    chunk *Chunks[3][3];
    for (u32 y = 0; y < 3; ++y)
    for (u32 x = 0; x < 3; ++x)
    {
        ivec2 Offset = { x - 1, y - 1 };
        ivec2 ChunkPos = iVec2_Add(ChunkPosition, Offset);
        Chunks[y][x] = View_GetChunk(View, ChunkPos);
    }

    // read block values
    u8 Ids[CHUNK_HEIGHT+2][CHUNK_WIDTH * 3][CHUNK_WIDTH * 3] = { 0 };
    u8 Shades[CHUNK_HEIGHT+2][CHUNK_WIDTH * 3][CHUNK_WIDTH * 3] = { 0 };
    for (u32 cy = 0; cy < 3; ++cy)
    for (u32 cx = 0; cx < 3; ++cx)
    {
        chunk *Chunk = Chunks[cy][cx];
        if (!Chunk) continue;
        for (u32 bz = 0; bz < CHUNK_HEIGHT; ++bz)
        for (u32 by = 0; by < CHUNK_WIDTH; ++by)
        {
            __m128i Id = _mm_loadu_si128((__m128i *)&Chunk->Blocks[bz][by][0].Id);
            _mm_storeu_si128((__m128i*)&Ids[bz + 1][cy * CHUNK_WIDTH + by][cx * CHUNK_WIDTH], Id);

            __m128i S = _mm_loadu_si128((__m128i *)&Chunk->Shades[bz][by][0]);
            _mm_storeu_si128((__m128i*)&Shades[bz + 1][cy * CHUNK_WIDTH + by][cx * CHUNK_WIDTH], S);
        }
    }

    // clear chunk mesh
    view_chunk *Chunk = View_GetViewChunk(View, ChunkPosition);
    Mesh_Clear(&Chunk->Mesh);

    // generate quads
    for (i32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (i32 y = 0; y < CHUNK_WIDTH; ++y)
    {
        __m128i Id = _mm_loadu_si128((__m128i *)&Ids[z + 1][CHUNK_WIDTH + y][CHUNK_WIDTH]);
        __m128i Air = _mm_xor_si128(_mm_cmpeq_epi8(Id, _mm_setzero_si128()), _mm_set1_epi8(0xFF));
        if (_mm_testz_si128(Air, Air)) continue;

        __m128i Left = _mm_loadu_si128((__m128i *)&Ids[z + 1][CHUNK_WIDTH + y][CHUNK_WIDTH - 1]);
        __m128i LeftOpaque = _mm_cmplt_epi8(Left, _mm_setzero_si128());
        if (!_mm_testc_si128(LeftOpaque, Air))
        {
            __m128i ShadowBytes[3][3];
            for (i32 zz = 0; zz < 3; ++zz)
            for (i32 yy = 0; yy < 3; ++yy)
                ShadowBytes[zz][yy] = _mm_loadu_si128((__m128i *)&Shades[z + zz][CHUNK_WIDTH - 1 + y + yy][CHUNK_WIDTH - 1]);

            f32 Shadow[2][2][16];
            for (i32 zz = 0; zz < 2; ++zz)
            for (i32 yy = 0; yy < 2; ++yy)
            {
                __m256 S = _mm256_set1_ps(3.0f / 225.0f);
                __m256 A = _mm256_set1_ps(3.0f /  15.0f);
                __m128i iS = _mm_adds_epu8(_mm_adds_epu8(ShadowBytes[zz + 0][yy + 0], ShadowBytes[zz + 0][yy + 1]), 
                                           _mm_adds_epu8(ShadowBytes[zz + 1][yy + 0], ShadowBytes[zz + 1][yy + 1]));
                _mm256_storeu_ps(&Shadow[zz][yy][ 0], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(iS)), S), A));
                _mm256_storeu_ps(&Shadow[zz][yy][ 4], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS,  4))), S), A));
                _mm256_storeu_ps(&Shadow[zz][yy][ 8], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS,  8))), S), A));
                _mm256_storeu_ps(&Shadow[zz][yy][12], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS, 12))), S), A));
            }

            for (i32 x = 0; x < 16; ++x)
            {
                u8 BlockId = Ids[z + 1][CHUNK_WIDTH + y][CHUNK_WIDTH + x];
                if (BlockId == BLOCK_ID_AIR) continue;
                u8 LeftId = Ids[z + 1][CHUNK_WIDTH + y][CHUNK_WIDTH - 1 + x];
                if (LeftId & BLOCK_OPAQUE) continue;
                ivec3 iBlockPosition = (ivec3){ x, y, z };
                vec4 Shadow4 = (vec4){ Shadow[0][1][x], Shadow[0][0][x], Shadow[1][0][x], Shadow[1][1][x] };
                Block_AddLeftFaceToMesh(&Chunk->Mesh, iBlockPosition, LeftId, BlockId, Shadow4);
            }
        }

        __m128i Right = _mm_loadu_si128((__m128i *)&Ids[z + 1][CHUNK_WIDTH + y][CHUNK_WIDTH + 1]);
        __m128i RightOpaque = _mm_cmplt_epi8(Right, _mm_setzero_si128());
        if (!_mm_testc_si128(RightOpaque, Air))
        {
            __m128i ShadowBytes[3][3];
            for (i32 zz = 0; zz < 3; ++zz)
            for (i32 yy = 0; yy < 3; ++yy)
                ShadowBytes[zz][yy] = _mm_loadu_si128((__m128i *)&Shades[z + zz][CHUNK_WIDTH - 1 + y + yy][CHUNK_WIDTH + 1]);

            f32 Shadow[2][2][16];
            for (i32 zz = 0; zz < 2; ++zz)
            for (i32 yy = 0; yy < 2; ++yy)
            {
                __m256 S = _mm256_set1_ps(3.0f / 225.0f);
                __m256 A = _mm256_set1_ps(3.0f /  15.0f);
                __m128i iS = _mm_adds_epu8(_mm_adds_epu8(ShadowBytes[zz + 0][yy + 0], ShadowBytes[zz + 0][yy + 1]), 
                                           _mm_adds_epu8(ShadowBytes[zz + 1][yy + 0], ShadowBytes[zz + 1][yy + 1]));
                _mm256_storeu_ps(&Shadow[zz][yy][ 0], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(iS)), S), A));
                _mm256_storeu_ps(&Shadow[zz][yy][ 4], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS,  4))), S), A));
                _mm256_storeu_ps(&Shadow[zz][yy][ 8], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS,  8))), S), A));
                _mm256_storeu_ps(&Shadow[zz][yy][12], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS, 12))), S), A));
            }

            for (i32 x = 0; x < 16; ++x)
            {
                u8 BlockId = Ids[z + 1][CHUNK_WIDTH + y][CHUNK_WIDTH + x];
                if (BlockId == BLOCK_ID_AIR) continue;
                u8 RightId = Ids[z + 1][CHUNK_WIDTH + y][CHUNK_WIDTH + 1 + x];
                if (RightId & BLOCK_OPAQUE) continue;
                ivec3 iBlockPosition = (ivec3){ x, y, z };
                vec4 Shadow4 = (vec4){ Shadow[0][0][x], Shadow[0][1][x], Shadow[1][1][x], Shadow[1][0][x] };
                Block_AddRightFaceToMesh(&Chunk->Mesh, iBlockPosition, RightId, BlockId, Shadow4);
            }
        }

        __m128i Front = _mm_loadu_si128((__m128i *)&Ids[z + 1][CHUNK_WIDTH + y - 1][CHUNK_WIDTH]);
        __m128i FrontOpaque = _mm_cmplt_epi8(Front, _mm_setzero_si128());
        if (!_mm_testc_si128(FrontOpaque, Air))
        {
            __m128i ShadowBytes[3][3];
            for (i32 zz = 0; zz < 3; ++zz)
            for (i32 xx = 0; xx < 3; ++xx)
                ShadowBytes[zz][xx] = _mm_loadu_si128((__m128i *)&Shades[z + zz][CHUNK_WIDTH - 1 + y][CHUNK_WIDTH - 1 + xx]);

            f32 Shadow[2][2][16];
            for (i32 zz = 0; zz < 2; ++zz)
            for (i32 xx = 0; xx < 2; ++xx)
            {
                __m256 S = _mm256_set1_ps(2.25f / 225.0f);
                __m256 A = _mm256_set1_ps(6.00f /  15.0f);
                __m128i iS = _mm_adds_epu8(_mm_adds_epu8(ShadowBytes[zz + 0][xx + 0], ShadowBytes[zz + 0][xx + 1]), 
                                           _mm_adds_epu8(ShadowBytes[zz + 1][xx + 0], ShadowBytes[zz + 1][xx + 1]));
                _mm256_storeu_ps(&Shadow[zz][xx][ 0], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(iS)), S), A));
                _mm256_storeu_ps(&Shadow[zz][xx][ 4], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS,  4))), S), A));
                _mm256_storeu_ps(&Shadow[zz][xx][ 8], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS,  8))), S), A));
                _mm256_storeu_ps(&Shadow[zz][xx][12], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS, 12))), S), A));
            }

            for (i32 x = 0; x < 16; ++x)
            {
                u8 BlockId = Ids[z + 1][CHUNK_WIDTH + y][CHUNK_WIDTH + x];
                if (BlockId == BLOCK_ID_AIR) continue;
                u8 FrontId = Ids[z + 1][CHUNK_WIDTH - 1 + y][CHUNK_WIDTH + x];
                if (FrontId & BLOCK_OPAQUE) continue;
                ivec3 iBlockPosition = (ivec3){ x, y, z };
                vec4 Shadow4 = (vec4){ Shadow[0][0][x], Shadow[0][1][x], Shadow[1][1][x], Shadow[1][0][x] };
                Block_AddFrontFaceToMesh(&Chunk->Mesh, iBlockPosition, FrontId, BlockId, Shadow4);
            }
        }

        __m128i Back = _mm_loadu_si128((__m128i *)&Ids[z + 1][CHUNK_WIDTH + y + 1][CHUNK_WIDTH]);
        __m128i BackOpaque = _mm_cmplt_epi8(Back, _mm_setzero_si128());
        if (!_mm_testc_si128(BackOpaque, Air))
        {
            __m128i ShadowBytes[3][3];
            for (i32 zz = 0; zz < 3; ++zz)
            for (i32 xx = 0; xx < 3; ++xx)
                ShadowBytes[zz][xx] = _mm_loadu_si128((__m128i *)&Shades[z + zz][CHUNK_WIDTH + 1 + y][CHUNK_WIDTH - 1 + xx]);

            f32 Shadow[2][2][16];
            for (i32 zz = 0; zz < 2; ++zz)
            for (i32 xx = 0; xx < 2; ++xx)
            {
                __m256 S = _mm256_set1_ps(2.25f / 225.0f);
                __m256 A = _mm256_set1_ps(6.00f /  15.0f);
                __m128i iS = _mm_adds_epu8(_mm_adds_epu8(ShadowBytes[zz + 0][xx + 0], ShadowBytes[zz + 0][xx + 1]), 
                                           _mm_adds_epu8(ShadowBytes[zz + 1][xx + 0], ShadowBytes[zz + 1][xx + 1]));
                _mm256_storeu_ps(&Shadow[zz][xx][ 0], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(iS)), S), A));
                _mm256_storeu_ps(&Shadow[zz][xx][ 4], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS,  4))), S), A));
                _mm256_storeu_ps(&Shadow[zz][xx][ 8], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS,  8))), S), A));
                _mm256_storeu_ps(&Shadow[zz][xx][12], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS, 12))), S), A));
            }

            for (i32 x = 0; x < 16; ++x)
            {
                u8 BlockId = Ids[z + 1][CHUNK_WIDTH + y][CHUNK_WIDTH + x];
                if (BlockId == BLOCK_ID_AIR) continue;
                u8 BackId = Ids[z + 1][CHUNK_WIDTH + 1 + y][CHUNK_WIDTH + x];
                if (BackId & BLOCK_OPAQUE) continue;
                ivec3 iBlockPosition = (ivec3){ x, y, z };
                vec4 Shadow4 = (vec4){ Shadow[0][1][x], Shadow[0][0][x], Shadow[1][0][x], Shadow[1][1][x] };
                Block_AddBackFaceToMesh(&Chunk->Mesh, iBlockPosition, BackId, BlockId, Shadow4);
            }
        }

        __m128i Bottom = _mm_loadu_si128((__m128i *)&Ids[z][CHUNK_WIDTH + y][CHUNK_WIDTH]);
        __m128i BottomOpaque = _mm_cmplt_epi8(Bottom, _mm_setzero_si128());
        if (!_mm_testc_si128(BottomOpaque, Air))
        {
            __m128i ShadowBytes[3][3];
            for (i32 yy = 0; yy < 3; ++yy)
            for (i32 xx = 0; xx < 3; ++xx)
                ShadowBytes[yy][xx] = _mm_loadu_si128((__m128i *)&Shades[z][CHUNK_WIDTH - 1 + y + yy][CHUNK_WIDTH - 1 + xx]);

            f32 Shadow[2][2][16];
            for (i32 yy = 0; yy < 2; ++yy)
            for (i32 xx = 0; xx < 2; ++xx)
            {
                __m256 S = _mm256_set1_ps(1.5f / 225.0f);
                __m256 A = _mm256_set1_ps(9.0f /  15.0f);
                __m128i iS = _mm_adds_epu8(_mm_adds_epu8(ShadowBytes[yy + 0][xx + 0], ShadowBytes[yy + 0][xx + 1]), 
                                           _mm_adds_epu8(ShadowBytes[yy + 1][xx + 0], ShadowBytes[yy + 1][xx + 1]));
                _mm256_storeu_ps(&Shadow[yy][xx][ 0], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(iS)), S), A));
                _mm256_storeu_ps(&Shadow[yy][xx][ 4], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS,  4))), S), A));
                _mm256_storeu_ps(&Shadow[yy][xx][ 8], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS,  8))), S), A));
                _mm256_storeu_ps(&Shadow[yy][xx][12], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS, 12))), S), A));
            }

            for (i32 x = 0; x < 16; ++x)
            {
                u8 BlockId = Ids[z + 1][CHUNK_WIDTH + y][CHUNK_WIDTH + x];
                if (BlockId == BLOCK_ID_AIR) continue;
                u8 BottomId = Ids[z][CHUNK_WIDTH + y][CHUNK_WIDTH + x];
                if (BottomId & BLOCK_OPAQUE) continue;
                ivec3 iBlockPosition = (ivec3){ x, y, z };
                vec4 Shadow4 = (vec4){ Shadow[1][0][x], Shadow[1][1][x], Shadow[0][1][x], Shadow[0][0][x] };
                Block_AddBottomFaceToMesh(&Chunk->Mesh, iBlockPosition, BottomId, BlockId, Shadow4);
            }
        }

        __m128i Top = _mm_loadu_si128((__m128i *)&Ids[z + 2][CHUNK_WIDTH + y][CHUNK_WIDTH]);
        __m128i TopOpaque = _mm_cmplt_epi8(Top, _mm_setzero_si128());
        if (!_mm_testc_si128(TopOpaque, Air))
        {
            __m128i ShadowBytes[3][3];
            for (i32 yy = 0; yy < 3; ++yy)
            for (i32 xx = 0; xx < 3; ++xx)
                ShadowBytes[yy][xx] = _mm_loadu_si128((__m128i *)&Shades[z + 2][CHUNK_WIDTH - 1 + y + yy][CHUNK_WIDTH - 1 + xx]);

            f32 Shadow[2][2][16];
            for (i32 yy = 0; yy < 2; ++yy)
            for (i32 xx = 0; xx < 2; ++xx)
            {
                __m256 S = _mm256_set1_ps(0.25f / 15.0f);
                __m256 A = _mm256_set1_ps(0.00f);
                __m128i iS = _mm_adds_epu8(_mm_adds_epu8(ShadowBytes[yy + 0][xx + 0], ShadowBytes[yy + 0][xx + 1]), 
                                           _mm_adds_epu8(ShadowBytes[yy + 1][xx + 0], ShadowBytes[yy + 1][xx + 1]));
                _mm256_storeu_ps(&Shadow[yy][xx][ 0], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(iS)), S), A));
                _mm256_storeu_ps(&Shadow[yy][xx][ 4], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS,  4))), S), A));
                _mm256_storeu_ps(&Shadow[yy][xx][ 8], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS,  8))), S), A));
                _mm256_storeu_ps(&Shadow[yy][xx][12], _mm256_add_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepi8_epi32(_mm_srli_si128(iS, 12))), S), A));
            }

            for (i32 x = 0; x < 16; ++x)
            {
                u8 BlockId = Ids[z + 1][CHUNK_WIDTH + y][CHUNK_WIDTH + x];
                if (BlockId == BLOCK_ID_AIR) continue;
                u8 TopId = Ids[z + 2][CHUNK_WIDTH + y][CHUNK_WIDTH + x];
                if (TopId & BLOCK_OPAQUE) continue;
                ivec3 iBlockPosition = (ivec3){ x, y, z };
                vec4 Shadow4 = (vec4){ Shadow[0][0][x], Shadow[0][1][x], Shadow[1][1][x], Shadow[1][0][x] };
                Block_AddTopFaceToMesh(&Chunk->Mesh, iBlockPosition, TopId, BlockId, Shadow4);
            }
        }
    }
    
    Chunk->Dirty = false;
}

void Mob_AddBlockMesh(quad_mesh *Mesh, f32 Height, f32 Width)
{
    vec2 UV = { 32.0f, 0.0f };
    vec4 Shadow = (vec4){ 0, 0, 0, 0 };
    
    //Left
    vec3 Pos = (vec3) { -Width / 2, Width / 2, -Height / 2 };
    quad Face = Block_FaceQuad(Pos, (vec3) { 0, -Width, 0 }, (vec3) { 0, 0, Height }, UV, (vec2) { 16, 0 }, (vec2) { 0, 16 }, Shadow);
    Mesh_AddQuad(Mesh, Face);
    //Right
    Pos = (vec3) { Width / 2, Width / 2, -Height / 2 };
    Face = Block_FaceQuad(Pos, (vec3) { 0, 0, Height }, (vec3){ 0, -Width, 0 }, UV, (vec2) { 16, 0 }, (vec2) { 0, 16 }, Shadow);
    Mesh_AddQuad(Mesh, Face);
    //Front
    Pos = (vec3) { -Width / 2, -Width / 2, -Height / 2 };
    Face = Block_FaceQuad(Pos, (vec3) { Width, 0, 0 }, (vec3) { 0, 0, Height }, UV, (vec2) { 16, 0 }, (vec2) { 0, 16 }, Shadow);
    Mesh_AddQuad(Mesh, Face);
    //Back
    Pos = (vec3) { -Width / 2, Width / 2, -Height / 2 };
    Face = Block_FaceQuad(Pos, (vec3) { 0, 0, Height }, (vec3) { Width, 0, 0 }, UV, (vec2) { 16, 0 }, (vec2) { 0, 16 }, Shadow);
    Mesh_AddQuad(Mesh, Face);
    //Bottom
    Pos = (vec3) { -Width / 2, -Width / 2, -Height / 2 };
    Face = Block_FaceQuad(Pos, (vec3) { 0, Width, 0 }, (vec3) { Width, 0, 0 }, UV, (vec2) { 16, 0 }, (vec2) { 0, 16 }, Shadow);
    Mesh_AddQuad(Mesh, Face);
    //Top
    Pos = (vec3) { -Width / 2, -Width / 2, Height / 2 };
    Face = Block_FaceQuad(Pos, (vec3) { Width, 0, 0 }, (vec3) { 0, Width, 0 }, UV, (vec2) { 16, 0 }, (vec2) { 0, 16 }, Shadow);
    Mesh_AddQuad(Mesh, Face);
}

void Mob_AddMesh(quad_mesh *Mesh)
{
    f32 Height = 5.0f;
    f32 Width = 5.0f;
    Mesh_Clear(Mesh);
    Mob_AddBlockMesh(Mesh, Height, Width);
}

void View_GenerateMobMesh(view_entity *Entity)
{
    if (ENTITY_NONE == Entity->Base.Type) return;

    switch (Entity->Base.Type)
    {
        case(ENTITY_MOB): Mob_AddMesh(&Entity->Mesh); break;
    }
}
