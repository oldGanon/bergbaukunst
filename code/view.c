
#define DRAW_DISTANCE 8

typedef struct view_chunk
{
	chunk Base;

	quad_mesh Mesh;
	bool Dirty;
} view_chunk;

typedef struct view_entity
{
    entity Base;

    quad_mesh Mesh;
    quad_mesh RotatedMesh;
} view_entity;

typedef struct view_entity_table
{
    u32 Count;
    u32 Capacity;
    view_entity *Entities;
} view_entity_table;

typedef struct view
{
	ivec2 Position;
	view_chunk Chunks[LOADED_CHUNKS_DIM][LOADED_CHUNKS_DIM];

    view_entity_table EntityTable;
} view;

void View_DrawLineBox(bitmap Target, camera Camera, box Box)
{
    vec3 Corners[8] = {
        Camera_WorldToScreen(Camera, Target, Box.Min),
        Camera_WorldToScreen(Camera, Target, Vec3_Lerp3(Box.Min, Box.Max, (vec3) { 1, 0, 0 })),
        Camera_WorldToScreen(Camera, Target, Vec3_Lerp3(Box.Min, Box.Max, (vec3) { 0, 1, 0 })),
        Camera_WorldToScreen(Camera, Target, Vec3_Lerp3(Box.Min, Box.Max, (vec3) { 1, 1, 0 })),
        Camera_WorldToScreen(Camera, Target, Vec3_Lerp3(Box.Min, Box.Max, (vec3) { 0, 0, 1 })),
        Camera_WorldToScreen(Camera, Target, Vec3_Lerp3(Box.Min, Box.Max, (vec3) { 1, 0, 1 })),
        Camera_WorldToScreen(Camera, Target, Vec3_Lerp3(Box.Min, Box.Max, (vec3) { 0, 1, 1 })),
        Camera_WorldToScreen(Camera, Target, Box.Max),
    };

    Draw_Line(Target, COLOR_WHITE, Corners[0], Corners[1]);
    Draw_Line(Target, COLOR_WHITE, Corners[2], Corners[3]);
    Draw_Line(Target, COLOR_WHITE, Corners[4], Corners[5]);
    Draw_Line(Target, COLOR_WHITE, Corners[6], Corners[7]);

    Draw_Line(Target, COLOR_WHITE, Corners[0], Corners[2]);
    Draw_Line(Target, COLOR_WHITE, Corners[1], Corners[3]);
    Draw_Line(Target, COLOR_WHITE, Corners[4], Corners[6]);
    Draw_Line(Target, COLOR_WHITE, Corners[5], Corners[7]);

    Draw_Line(Target, COLOR_WHITE, Corners[0], Corners[4]);
    Draw_Line(Target, COLOR_WHITE, Corners[1], Corners[5]);
    Draw_Line(Target, COLOR_WHITE, Corners[2], Corners[6]);
    Draw_Line(Target, COLOR_WHITE, Corners[3], Corners[7]);
}

void Block_HighlightFace(bitmap Buffer, camera Camera, ivec3 iBlockPosition, u32 BlockFace)
{
    vec3 BlockPosition = iVec3_toVec3(iBlockPosition);
    
    vec3 Corners[8] = {
        Camera_WorldToScreen(Camera, Buffer, BlockPosition),
        Camera_WorldToScreen(Camera, Buffer, Vec3_Add(BlockPosition, (vec3) { 1, 0, 0 })),
        Camera_WorldToScreen(Camera, Buffer, Vec3_Add(BlockPosition, (vec3) { 0, 1, 0 })),
        Camera_WorldToScreen(Camera, Buffer, Vec3_Add(BlockPosition, (vec3) { 1, 1, 0 })),
        Camera_WorldToScreen(Camera, Buffer, Vec3_Add(BlockPosition, (vec3) { 0, 0, 1 })),
        Camera_WorldToScreen(Camera, Buffer, Vec3_Add(BlockPosition, (vec3) { 1, 0, 1 })),
        Camera_WorldToScreen(Camera, Buffer, Vec3_Add(BlockPosition, (vec3) { 0, 1, 1 })),
        Camera_WorldToScreen(Camera, Buffer, Vec3_Add(BlockPosition, (vec3) { 1, 1, 1 })),
    };

    switch (BlockFace)
    {
        case BLOCK_FACE_NONE:
        {
            Draw_Line(Buffer, COLOR_WHITE, Corners[0], Corners[1]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[2], Corners[3]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[4], Corners[5]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[6], Corners[7]);

            Draw_Line(Buffer, COLOR_WHITE, Corners[0], Corners[2]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[1], Corners[3]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[4], Corners[6]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[5], Corners[7]);

            Draw_Line(Buffer, COLOR_WHITE, Corners[0], Corners[4]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[1], Corners[5]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[2], Corners[6]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[3], Corners[7]);
        }break;
        case BLOCK_FACE_LEFT:
        {
            Draw_Line(Buffer, COLOR_WHITE, Corners[0], Corners[2]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[2], Corners[6]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[6], Corners[4]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[4], Corners[0]);
        } break;
        case BLOCK_FACE_RIGHT:
        {
            Draw_Line(Buffer, COLOR_WHITE, Corners[1], Corners[3]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[3], Corners[7]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[7], Corners[5]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[5], Corners[1]);
        } break;
        case BLOCK_FACE_FRONT:
        {
            Draw_Line(Buffer, COLOR_WHITE, Corners[0], Corners[1]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[1], Corners[5]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[5], Corners[4]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[4], Corners[0]);
        } break;
        case BLOCK_FACE_BACK:
        {
            Draw_Line(Buffer, COLOR_WHITE, Corners[2], Corners[3]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[3], Corners[7]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[7], Corners[6]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[6], Corners[2]);
        } break;
        case BLOCK_FACE_BOTTOM:
        {
            Draw_Line(Buffer, COLOR_WHITE, Corners[0], Corners[1]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[1], Corners[3]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[3], Corners[2]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[2], Corners[0]);
        } break;
        case BLOCK_FACE_TOP:
        {
            Draw_Line(Buffer, COLOR_WHITE, Corners[4], Corners[5]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[5], Corners[7]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[7], Corners[6]);
            Draw_Line(Buffer, COLOR_WHITE, Corners[6], Corners[4]);
        } break;
    }
}



bool View_ChunkIsLoaded(const view *View, ivec2 ChunkPosition)
{
    if (ChunkPosition.x < (View->Position.x - LOADED_CHUNKS_DIST) ||
        ChunkPosition.y < (View->Position.y - LOADED_CHUNKS_DIST) ||
        ChunkPosition.x >= (View->Position.x + LOADED_CHUNKS_DIST) ||
        ChunkPosition.y >= (View->Position.y + LOADED_CHUNKS_DIST))
        return false;
    return true;
}

view_chunk *View_GetViewChunk(view *View, ivec2 ChunkPosition)
{
    if (!View_ChunkIsLoaded(View, ChunkPosition))
        return 0;

    ivec2 ViewPosition = iVec2_And(ChunkPosition, iVec2_Set1(LOADED_CHUNKS_DIM_MASK));
	return &View->Chunks[ViewPosition.y][ViewPosition.x];
}

const view_chunk *View_GetConstViewChunk(const view *View, ivec2 ChunkPosition)
{
    if (!View_ChunkIsLoaded(View, ChunkPosition))
        return 0;

    ivec2 ViewPosition = iVec2_And(ChunkPosition, iVec2_Set1(LOADED_CHUNKS_DIM_MASK));
    return &View->Chunks[ViewPosition.y][ViewPosition.x];
}

chunk *View_GetChunk(view *View, ivec2 ChunkPosition)
{
    view_chunk *ViewChunk = View_GetViewChunk(View, ChunkPosition);
    if (ViewChunk) return &ViewChunk->Base;
    return 0;
}

const chunk *View_GetConstChunk(const view *View, ivec2 ChunkPosition)
{
    const view_chunk *ViewChunk = View_GetConstViewChunk(View, ChunkPosition);
    if (ViewChunk) return &ViewChunk->Base;
    return 0;
}

void View_MarkChunkDirty(view *View, ivec2 ChunkPosition)
{
	view_chunk *Chunk = View_GetViewChunk(View, ChunkPosition);
    if (Chunk) Chunk->Dirty = true;
}

block View_GetBlock(const view *View, ivec3 WorldPosition)
{
    if (WorldPosition.z < 0)                return DEFAULT_SKY_BLOCK;
    if (WorldPosition.z > CHUNK_HEIGHT - 1) return DEFAULT_HELL_BLOCK;

    ivec2 ChunkPosition = World_ToChunkPosition(WorldPosition);
    const chunk *Chunk = View_GetConstChunk(View, ChunkPosition);
    if (!Chunk) return DEFAULT_BLOCK;
    return Chunk_GetBlock(Chunk, WorldPosition);
}

void View_SetBlock(view *View, ivec3 WorldPosition, block Block)
{
    if ((WorldPosition.z < 0) || (WorldPosition.z > CHUNK_HEIGHT - 1))
        return;

    ivec2 ChunkPosition = World_ToChunkPosition(WorldPosition);
    chunk *Chunk = View_GetChunk(View, ChunkPosition);
    if (!Chunk) return;
    Chunk_SetBlock(Chunk, WorldPosition, Block);

    Chunk_CalcLight(View, View_GetChunk, ChunkPosition);
    
    // mark chunks dirty
    for (i32 y = -1; y <= 1; ++y)
    for (i32 x = -1; x <= 1; ++x)
    {
        ivec2 Offset = { x, y };
        ivec2 ChunkPos = iVec2_Add(ChunkPosition, Offset);
        View_MarkChunkDirty(View, ChunkPos);
    }
}

#include "meshgen.c"

void View_DrawChunk(view *View, ivec2 ChunkPosition, const bitmap Target, bitmap TerrainTexture, const camera Camera)
{
	view_chunk *Chunk = View_GetViewChunk(View, ChunkPosition);
	if (!Chunk) return;
    if (Chunk->Dirty) View_GenerateChunkMesh(View, ChunkPosition);

    box ChunkBox = Chunk_Box(&Chunk->Base);
    if (!Camera_BoxVisible(Camera, Target, ChunkBox)) return;
    vec3 ChunkPos = ChunkBox.Min;
    Mesh_Draw(Target, Camera, TerrainTexture, ChunkPos, &Chunk->Mesh);
}


void View_DrawEntity(view_entity *Entity, const bitmap Target, bitmap TerrainTexture, const camera Camera)
{
    if (Entity->Mesh.Count == 0) return;

    Mesh_Rotate(Entity->Base.Yaw, &Entity->Mesh, &Entity->RotatedMesh);

    Mesh_Draw(Target, Camera, TerrainTexture, Entity->Base.Position, &Entity->RotatedMesh);
}

void View_Draw(view *View, const bitmap Target, bitmap TerrainTexture, const camera Camera)
{
    // draw chunks
    ivec3 iWorldPosition = Vec3_FloorToIVec3(Camera.Position);
    ivec2 Center = World_ToChunkPosition(iWorldPosition);
    View_DrawChunk(View, Center, Target, TerrainTexture, Camera);
    for (i32 i = 1; i < DRAW_DISTANCE; ++i)
    {
        for (i32 j = -i; j < i; ++j)
        {
            ivec2 OffsetX = (ivec2){ i, j };
            View_DrawChunk(View, iVec2_Add(Center, OffsetX), Target, TerrainTexture, Camera);
            View_DrawChunk(View, iVec2_Sub(Center, OffsetX), Target, TerrainTexture, Camera);

            ivec2 OffsetY = (ivec2){ -j, i };
            View_DrawChunk(View, iVec2_Add(Center, OffsetY), Target, TerrainTexture, Camera);
            View_DrawChunk(View, iVec2_Sub(Center, OffsetY), Target, TerrainTexture, Camera);
        }
    }

    // draw entity
    for (u32 i = 0; i < View->EntityTable.Capacity; ++i)
    {
        view_entity *Entity = &View->EntityTable.Entities[i];
        if (Entity->Base.Type == ENTITY_NONE) continue;
        View_DrawEntity(Entity, Target, TerrainTexture, Camera);
    }
}

void View_DrawEntityBoxes(view *View, const bitmap Target, const camera Camera)
{
    for (u32 i = 0; i < View->EntityTable.Capacity; ++i)
    {
        entity *Entity = &View->EntityTable.Entities[i].Base;
        if (Entity->Type == ENTITY_NONE) continue;
        box EntityBox = Entity_Box(Entity);
        View_DrawLineBox(Target, Camera, EntityBox);
    }
}

void View_SetPosition(view *View, ivec2 Position)
{
    View->Position = Position;
    for (i32 y = 0; y < LOADED_CHUNKS_DIM; ++y)
    for (i32 x = 0; x < LOADED_CHUNKS_DIM; ++x)
    {
        view_chunk *Chunk = &View->Chunks[y][x];
        if (View_ChunkIsLoaded(View, Chunk->Base.Position)) continue;

        ivec2 ChunkPosition = { Position.x + x - LOADED_CHUNKS_DIST, Position.y + y - LOADED_CHUNKS_DIST };
        // ivec2 ViewPosition = iVec2_And(ChunkPosition, iVec2_Set1(LOADED_CHUNKS_DIM_MASK));
        Chunk_Init(&Chunk->Base, ChunkPosition);
        Mesh_Clear(&Chunk->Mesh);
    }
}

void View_SetChunk(view *View, const msg_chunk_data *ChunkData)
{
    // copy data
    ivec2 ChunkPosition = ChunkData->Position;
    chunk *Chunk = View_GetChunk(View, ChunkPosition);
    if (!Chunk) return;

    Chunk->Position = ChunkPosition;

    ivec3 Min = (ivec3){ 0, 0, 0 };
    ivec3 Max = (ivec3){ CHUNK_WIDTH_MASK, CHUNK_WIDTH_MASK, CHUNK_HEIGHT_MASK };
    Min = iVec3_Max(Min, iVec3_Min(ChunkData->MinBlock, ChunkData->MaxBlock));
    Max = iVec3_Min(Max, iVec3_Max(ChunkData->MinBlock, ChunkData->MaxBlock));
    const u8 *BlockPtr = ChunkData->Blocks;
    for (i32 z = Min.z; z <= Max.z; ++z)
    for (i32 y = Min.y; y <= Max.y; ++y)
    for (i32 x = Min.x; x <= Max.x; ++x)
        Chunk->Blocks[z][y][x].Id = *BlockPtr++;

    Chunk_CalcLight(View, View_GetChunk, ChunkPosition);

    // mark chunk dirty
    for (i32 y = -1; y <= 1; ++y)
    for (i32 x = -1; x <= 1; ++x)
    {
        ivec2 Offset = { x, y };
        ivec2 ChunkPos = iVec2_Add(ChunkPosition, Offset);
        View_MarkChunkDirty(View, ChunkPos);
    }
}

void View_SetEntityTableCapacity(view_entity_table *EntityTable, u32 NewCapacity)
{
    if (NewCapacity <= EntityTable->Capacity)
        return;

    u32 OldCapacity = EntityTable->Capacity;
    EntityTable->Capacity = NewCapacity;
    EntityTable->Entities = realloc(EntityTable->Entities, NewCapacity * sizeof(view_entity));
    for (u32 i = OldCapacity; i < NewCapacity; ++i)
    {
        EntityTable->Entities[i].Mesh = Mesh_Create();
        EntityTable->Entities[i].RotatedMesh = Mesh_Create();
    }    
}

void View_SetEntity(view *View, const msg_set_entity *SetEntity)
{
    while (SetEntity->Id >= View->EntityTable.Capacity)
    {
        u32 NewCapacity = View->EntityTable.Capacity << 1;
        View_SetEntityTableCapacity(&View->EntityTable, NewCapacity);
    }

    view_entity *ViewEntity = &View->EntityTable.Entities[SetEntity->Id];
    ViewEntity->Base = SetEntity->Entity;
    View_GenerateMobMesh(ViewEntity);
}

void View_Init(view *View)
{
    View->Position = (ivec2){ 0 };
    for (i32 y = 0; y < LOADED_CHUNKS_DIM; ++y)
    for (i32 x = 0; x < LOADED_CHUNKS_DIM; ++x)
    {
        ivec2 ChunkPosition = { x - LOADED_CHUNKS_DIST, y - LOADED_CHUNKS_DIST };
        ivec2 ViewPosition = iVec2_And(ChunkPosition, iVec2_Set1(LOADED_CHUNKS_DIM_MASK));
        view_chunk *Chunk = &View->Chunks[ViewPosition.y][ViewPosition.x];
        Chunk_Init(&Chunk->Base, ChunkPosition);
        Chunk->Mesh = Mesh_Create();
    }

    View_SetEntityTableCapacity(&View->EntityTable, 256);
}

f32 View_TraceRay(view *View, vec3 RayOrigin, vec3 RayDirection, f32 RayLength, trace_result *Result)
{
    return Phys_TraceRay(View_GetBlock, View, RayOrigin, RayDirection, RayLength, Result);
}

vec3 View_CheckMoveBox(const view *View, box Box, vec3 Move)
{
    return Phys_CheckMoveBox(View_GetBlock, View, Box, Move);
}
