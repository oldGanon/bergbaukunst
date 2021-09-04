
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
void Mesh_Delete(quad_mesh *);
void Mesh_Clear(quad_mesh *);
void Mesh_AddQuad(quad_mesh *, quad);
void Mesh_Sort(quad_mesh *, const camera, vec3);
void Mesh_Draw(bitmap, const camera, bitmap, vec3, quad_mesh *);

typedef struct quad_reorder_buffer
{
    quad *Quads;
    f32 *Distances;
    u32 Capacity;
    u32 Count;
} quad_reorder_buffer;

void QuadReorderBuffer_Load(quad_reorder_buffer *ReorderBuffer, const quad_mesh *Mesh);
void QuadReorderBuffer_Store(const quad_reorder_buffer *ReorderBuffer, quad_mesh *Mesh);
void QuadReorderBuffer_CalcDistances(quad_reorder_buffer *ReorderBuffer, vec3 Position);
void QuadReorderBuffer_InsertionSort(quad_reorder_buffer *ReorderBuffer);

global quad_reorder_buffer GlobalQuadReorderBuffer = { 0 };

/*******************/
/*    Quad Mesh    */
/*******************/

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

void Mesh_Delete(quad_mesh *Mesh)
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

void Mesh_Sort(quad_mesh *Mesh, const camera Camera, vec3 Position)
{
    quad_reorder_buffer *ReorderBuffer = &GlobalQuadReorderBuffer;
    
    Position = Vec3_Sub(Camera.Position, Position);

    QuadReorderBuffer_Load(ReorderBuffer, Mesh);
    QuadReorderBuffer_CalcDistances(ReorderBuffer, Position);
    QuadReorderBuffer_InsertionSort(ReorderBuffer);
    QuadReorderBuffer_Store(ReorderBuffer, Mesh);
}

void Mesh_Draw(bitmap Target, const camera Camera, bitmap Texture, vec3 Position, quad_mesh *Mesh)
{
    Mesh_Sort(Mesh, Camera, Position);

    for (u32 i = 0; i < Mesh->Count; i++) 
    {
        vertex V0 = Mesh->Quads[i].Verts[0];
        vertex V1 = Mesh->Quads[i].Verts[1];
        vertex V2 = Mesh->Quads[i].Verts[2];
        vertex V3 = Mesh->Quads[i].Verts[3];

        V0.Position = Camera_WorldToScreen(Camera, Target, Vec3_Add(V0.Position, Position));
        V1.Position = Camera_WorldToScreen(Camera, Target, Vec3_Add(V1.Position, Position));
        V2.Position = Camera_WorldToScreen(Camera, Target, Vec3_Add(V2.Position, Position));
        V3.Position = Camera_WorldToScreen(Camera, Target, Vec3_Add(V3.Position, Position));

        Draw_Quad(Target, Texture, V0, V1, V2, V3);
    }
}

/***********************/
/* Quad Reorder Buffer */
/***********************/

void QuadReorderBuffer_Load(quad_reorder_buffer *ReorderBuffer, const quad_mesh *Mesh)
{
    while (ReorderBuffer->Capacity < Mesh->Count)
    {
        if (!ReorderBuffer->Capacity)
            ReorderBuffer->Capacity = 1;
        else
            ReorderBuffer->Capacity <<= 1;
        ReorderBuffer->Quads = realloc(ReorderBuffer->Quads, sizeof(quad) * ReorderBuffer->Capacity);
        ReorderBuffer->Distances = realloc(ReorderBuffer->Distances, sizeof(f32) * ReorderBuffer->Capacity);
    }

    ReorderBuffer->Count = Mesh->Count;
    memcpy(ReorderBuffer->Quads, Mesh->Quads, sizeof(quad) * Mesh->Count);
}

void QuadReorderBuffer_Store(const quad_reorder_buffer *ReorderBuffer, quad_mesh *Mesh)
{
    Mesh__MakeSpace(Mesh, ReorderBuffer->Count);

    Mesh->Count = ReorderBuffer->Count;
    memcpy(Mesh->Quads, ReorderBuffer->Quads, sizeof(quad) * ReorderBuffer->Count);
}

void QuadReorderBuffer_CalcDistances(quad_reorder_buffer *ReorderBuffer, vec3 Position)
{
    for (u32 i = 0; i < ReorderBuffer->Count; i++)
    {
        vec3 Center = Vec3_Add(Vec3_Add(ReorderBuffer->Quads[i].Verts[0].Position, 
                                        ReorderBuffer->Quads[i].Verts[1].Position),
                               Vec3_Add(ReorderBuffer->Quads[i].Verts[2].Position, 
                                        ReorderBuffer->Quads[i].Verts[3].Position));
        Center = Vec3_Mul(Center, Vec3_Set1(0.25f));
        ReorderBuffer->Distances[i] = Vec3_LengthSq(Vec3_Sub(Center, Position));
    }
}

void QuadReorderBuffer_InsertionSort(quad_reorder_buffer *ReorderBuffer)
{
    i32 End = ReorderBuffer->Count - 1;
    i32 Cur = 0;
    while (Cur < End)
    {
        i32 B = Cur++;
        i32 A = Cur;
        while (ReorderBuffer->Distances[A] > ReorderBuffer->Distances[B])
        {
            quad TempQuad = ReorderBuffer->Quads[B];
            ReorderBuffer->Quads[B] = ReorderBuffer->Quads[A];
            ReorderBuffer->Quads[A] = TempQuad;

            f32 TempDist = ReorderBuffer->Distances[B];
            ReorderBuffer->Distances[B] = ReorderBuffer->Distances[A];
            ReorderBuffer->Distances[A] = TempDist;

            if (B == 0) break;
            A = B--;
        }
    }
}
