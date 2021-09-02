
enum
{
    BLOCK_ID_AIR = 0,
    BLOCK_ID_GRAS = 1,
    BLOCK_ID_WOOD = 2,
    BLOCK_ID_LEAVES = 3,
} block_id;

typedef struct block
{
    u8 Id;
    u8 Light;
} block;

typedef struct block_group
{
    block Blocks[3][3][3];
} block_group;

const u8 Block_Opaque[256] = {
    [BLOCK_ID_GRAS] = true,
    [BLOCK_ID_WOOD] = true,
};

inline quad Quad_RotateVerts(quad Quad)
{
    vertex T = Quad.Verts[0];
    Quad.Verts[0] = Quad.Verts[1];
    Quad.Verts[1] = Quad.Verts[2];
    Quad.Verts[2] = Quad.Verts[3];
    Quad.Verts[3] = T;
    return Quad;
}

void Block_AddGrasBlockToMesh(quad_mesh *Mesh, vec3 Position, const block_group *BlockGroup)
{
    if (!Block_Opaque[BlockGroup->Blocks[1][1][0].Id])
    {
        quad LeftFace = {
            .Verts = {
                [0] = {
                    .Position = Vec3_Add(Position, (vec3){0,1,0}),
                    .TexCoord = (vec2){16,0},
                },
                [1] = {
                    .Position = Position,
                    .TexCoord = (vec2){32,0},
                },
                [2] = {
                    .Position = Vec3_Add(Position, (vec3){0,0,1}),
                    .TexCoord = (vec2){32,16},
                },
                [3] = {
                    .Position = Vec3_Add(Position, (vec3){0,1,1}),
                    .TexCoord = (vec2){16,16},
                },
            },
        };

        if (Block_Opaque[BlockGroup->Blocks[0][2][0].Id] || 
            Block_Opaque[BlockGroup->Blocks[0][0][2].Id])
            LeftFace = Quad_RotateVerts(LeftFace);
        Mesh_AddQuad(Mesh, LeftFace);
    }
}
