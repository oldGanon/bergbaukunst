
typedef struct quad
{
    vertex Verts[4];
    vec3 Center;
} quad;

typedef struct quad_mesh
{
    quad *Quads;
    u32 Capacity;
    u32 Count;
} quad_mesh;

quad_mesh Mesh_Create(void);
void Mesh_Destroy(quad_mesh *);
void Mesh_Clear(quad_mesh *);
void Mesh_AddQuad(quad_mesh *, quad);
void Mesh_Sort(quad_mesh *, const camera, vec3);
void Mesh_Draw(bitmap, const camera, bitmap, vec3, quad_mesh *);

inline quad Quad_RotateVerts(quad Quad)
{
    vertex T = Quad.Verts[0];
    Quad.Verts[0] = Quad.Verts[1];
    Quad.Verts[1] = Quad.Verts[2];
    Quad.Verts[2] = Quad.Verts[3];
    Quad.Verts[3] = T;
    return Quad;
}

quad_mesh Mesh_Create(void)
{
    u32 Capacity = 256;
    return (quad_mesh) {
        .Quads = malloc(sizeof(quad) * Capacity),
        .Capacity = Capacity,
        .Count = 0,
    };
}

void Mesh_Destroy(quad_mesh *Mesh)
{
    free(Mesh->Quads);
    *Mesh = (quad_mesh){ 0 };
}

void Mesh_Clear(quad_mesh *Mesh)
{
    Mesh->Count = 0;
}

inline void Mesh__MakeSpace(quad_mesh *Mesh, u32 Count)
{
    while (Count > Mesh->Capacity)
    {
        Mesh->Capacity <<= 1;
        Mesh->Quads = realloc(Mesh->Quads, sizeof(quad) * Mesh->Capacity);
    }
}

void Mesh_AddQuad(quad_mesh *Mesh, quad Quad)
{
    Mesh__MakeSpace(Mesh, Mesh->Count + 1);
    Mesh->Quads[Mesh->Count++] = Quad;
}

void Mesh_Draw(bitmap Target, const camera Camera, bitmap Texture, vec3 Position, quad_mesh *Mesh)
{
    Raserizer_SetTexture(Texture);

    mat4 Transform = Mat4_Translation(Vec3_Negate(Position));
    Transform = Mat4_Mul(Camera_WorldToScreenMatrix(Camera, Target), Transform);

    __m128 C0 = LOAD_VEC4(Transform.Columns[0]);
    __m128 C1 = LOAD_VEC4(Transform.Columns[1]);
    __m128 C2 = LOAD_VEC4(Transform.Columns[2]);
    __m128 C3 = LOAD_VEC4(Transform.Columns[3]);
    C3 = _mm_and_ps(C3, _mm_castsi128_ps(_mm_set_epi32(0,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF)));
    
    for (u32 i = 0; i < Mesh->Count; i++) 
    {
        vertex V[4] = {
            Mesh->Quads[i].Verts[0],
            Mesh->Quads[i].Verts[1],
            Mesh->Quads[i].Verts[2],
            Mesh->Quads[i].Verts[3],
        };

        for (u32 j = 0; j < 4; ++j)
        {
            __m128 P = LOAD_VEC3(V[j].Position);
            __m128 P0 = _mm_mul_ps(C0, _mm_shuffle_ps(P, P, _MM_SHUFFLE(0,0,0,0)));
            __m128 P1 = _mm_mul_ps(C1, _mm_shuffle_ps(P, P, _MM_SHUFFLE(1,1,1,1)));
            __m128 P2 = _mm_mul_ps(C2, _mm_shuffle_ps(P, P, _MM_SHUFFLE(2,2,2,2)));
            __m128 P3 = C3;
            P = _mm_add_ps(_mm_add_ps(P0, P1), _mm_add_ps(P2, P3));
            STORE_VEC3(V[j].Position, P);
        }
        Raserizer_DrawQuad(V[0], V[1], V[2], V[3]);
    }
    Raserizer_Flush();
}
