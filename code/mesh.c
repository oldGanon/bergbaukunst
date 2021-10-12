
typedef struct quad
{
    vertex Verts[4];
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
void Mesh_Draw(bitmap, const camera, bitmap, vec3, quad_mesh *, box);

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

void Mesh_Draw(bitmap Target, const camera Camera, bitmap Texture, vec3 Position, quad_mesh *Mesh, box Box)
{
    mat4 Transform = Mat4_Translation(Vec3_Negate(Position));
    Transform = Mat4_Mul(Camera_WorldToScreenMatrix(Camera, Target), Transform);
    Rasterizer_DrawMesh(Mesh->Quads[0].Verts, Mesh->Count, Texture, Transform, Box);
}
