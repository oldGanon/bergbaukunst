
#define DRAW_DISTANCE 8

typedef struct view_chunk
{
	chunk Chunk;
	quad_mesh Mesh;
	bool MeshDirty;
} view_chunk;

typedef struct view_entity_map
{
    u32 Count;
    u32 Capacity;
    entity *Entities;
} view_entity_map;

typedef struct view
{
	ivec2 Position;
	view_chunk Chunks[LOADED_CHUNKS_DIM][LOADED_CHUNKS_DIM];

    view_entity_map EntityMap;
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



bool View_ChunkisLoaded(const view *View, ivec2 ChunkPosition)
{
    if (ChunkPosition.x < (View->Position.x - LOADED_CHUNKS_DIST) ||
        ChunkPosition.y < (View->Position.y - LOADED_CHUNKS_DIST) ||
        ChunkPosition.x >= (View->Position.x + LOADED_CHUNKS_DIST) ||
        ChunkPosition.y >= (View->Position.y + LOADED_CHUNKS_DIST))
        return false;
    return true;
}

view_chunk *View_GetChunk(view *View, ivec2 ChunkPosition)
{
    if (!View_ChunkisLoaded(View, ChunkPosition))
        return 0;

    ivec2 ViewPosition = iVec2_And(ChunkPosition, iVec2_Set1(LOADED_CHUNKS_DIM_MASK));
	return &View->Chunks[ViewPosition.y][ViewPosition.x];
}

const view_chunk *View_GetConstChunk(const view *View, ivec2 ChunkPosition)
{
    if (!View_ChunkisLoaded(View, ChunkPosition))
        return 0;

    ivec2 ViewPosition = iVec2_And(ChunkPosition, iVec2_Set1(LOADED_CHUNKS_DIM_MASK));
    return &View->Chunks[ViewPosition.y][ViewPosition.x];
}

void View_MarkChunkDirty(view *View, ivec2 ChunkPosition)
{
	view_chunk *Chunk = View_GetChunk(View, ChunkPosition);
    if (Chunk) Chunk->MeshDirty = true;
}

block View_GetBlock(const view *View, ivec3 WorldPosition)
{
    if (WorldPosition.z < 0)                return DEFAULT_SKY_BLOCK;
    if (WorldPosition.z > CHUNK_HEIGHT - 1) return DEFAULT_HELL_BLOCK;

    ivec2 ChunkPosition = World_ToChunkPosition(WorldPosition);
    const view_chunk *Chunk = View_GetConstChunk(View, ChunkPosition);
    if (!Chunk) return DEFAULT_BLOCK;
    return Chunk_GetBlock(&Chunk->Chunk, WorldPosition);
}

void View_SetBlock(view *View, ivec3 WorldPosition, block Block)
{
    if ((WorldPosition.z < 0) || (WorldPosition.z > CHUNK_HEIGHT - 1))
        return;

    ivec2 ChunkPosition = World_ToChunkPosition(WorldPosition);
    view_chunk *Chunk = View_GetChunk(View, ChunkPosition);
    if (!Chunk) return;
    Chunk_SetBlock(&Chunk->Chunk, WorldPosition, Block);

    View_MarkChunkDirty(View, ChunkPosition);
    ivec3 BlockPosition = World_ToBlockPosition(WorldPosition);
    if ((BlockPosition.x == 0))
    	View_MarkChunkDirty(View, iVec2_Add(ChunkPosition, (ivec2){-1,0}));
    if ((BlockPosition.y == 0))
    	View_MarkChunkDirty(View, iVec2_Add(ChunkPosition, (ivec2){0,-1}));
    if ((BlockPosition.x == CHUNK_WIDTH - 1))
    	View_MarkChunkDirty(View, iVec2_Add(ChunkPosition, (ivec2){+1,0}));
    if ((BlockPosition.y == CHUNK_WIDTH - 1))
    	View_MarkChunkDirty(View, iVec2_Add(ChunkPosition, (ivec2){0,+1}));
    if ((BlockPosition.x == 0) && (BlockPosition.y == 0))
    	View_MarkChunkDirty(View, iVec2_Add(ChunkPosition, (ivec2){-1,-1}));
    if ((BlockPosition.x == 0) && (BlockPosition.y == CHUNK_WIDTH - 1))
    	View_MarkChunkDirty(View, iVec2_Add(ChunkPosition, (ivec2){-1,+1}));
    if ((BlockPosition.x == CHUNK_WIDTH - 1) && (BlockPosition.y == 0))
    	View_MarkChunkDirty(View, iVec2_Add(ChunkPosition, (ivec2){+1,-1}));
    if ((BlockPosition.x == CHUNK_WIDTH - 1) && (BlockPosition.y == CHUNK_WIDTH - 1))
    	View_MarkChunkDirty(View, iVec2_Add(ChunkPosition, (ivec2){+1,+1}));
}

#include "meshgen.c"

void View_DrawChunk(view *View, ivec2 ChunkPosition, const bitmap Target, bitmap TerrainTexture, const camera Camera)
{
	view_chunk *Chunk = View_GetChunk(View, ChunkPosition);
	if (!Chunk) return;
    if (Chunk->MeshDirty)
    {
        View_GenerateChunkMesh(View, ChunkPosition);
    }

    box ChunkBox = Chunk_Box(&Chunk->Chunk);
    if (!Camera_BoxVisible(Camera, Target, ChunkBox)) return;
    Mesh_Draw(Target, Camera, TerrainTexture, ChunkBox.Min, &Chunk->Mesh);
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
}

void View_DrawEntityBoxes(view *View, const bitmap Target, const camera Camera)
{
    for (u32 i = 0; i < View->EntityMap.Capacity; ++i)
    {
        entity *Entity = &View->EntityMap.Entities[i];
        if (Entity->Type == ENTITY_NONE) continue;
        box EntityBox = Entity_Box(Entity);
        View_DrawLineBox(Target, Camera, EntityBox);
    }
}

void View_SetPosition(view *View, ivec2 Position)
{
    ivec2 Min = iVec2_Sub(Position, iVec2_Set1(DRAW_DISTANCE));
    ivec2 Max = iVec2_Add(Position, iVec2_Set1(DRAW_DISTANCE));

    for (i32 y = Min.y; y <= Max.y; ++y)
    for (i32 x = Min.x; x <= Max.x; ++x)
    {
        ivec2 ChunkPosition = (ivec2){ x, y };
        ivec2 ViewPosition = iVec2_And(ChunkPosition, iVec2_Set1(LOADED_CHUNKS_DIM_MASK));
        view_chunk *Chunk = &View->Chunks[ViewPosition.y][ViewPosition.x];
        if (Chunk->Chunk.Position.x == ChunkPosition.x ||
            Chunk->Chunk.Position.y == ChunkPosition.y)
            continue;
        Chunk_Init(&Chunk->Chunk, ViewPosition);
        Mesh_Clear(&Chunk->Mesh);
    }
}

void View_SetChunk(view *View, const msg_chunk_data *ChunkData)
{
    ivec2 ChunkPosition = ChunkData->Position;
    view_chunk *Chunk = View_GetChunk(View, ChunkPosition);
    if (!Chunk) return;
    for (u32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (u32 y = 0; y < CHUNK_WIDTH; ++y)
    for (u32 x = 0; x < CHUNK_WIDTH; ++x)
    {
        Chunk->Chunk.Blocks[z][y][x].Id = ChunkData->Blocks[z][y][x];
    }
    Chunk->Chunk.Position = ChunkPosition;
    Chunk_CalcSkyLight(&Chunk->Chunk);

    for (i32 y = -1; y <= 1; ++y)
    for (i32 x = -1; x <= 1; ++x)
    {
        ivec2 Offset = { x, y };
        Chunk = View_GetChunk(View, iVec2_Add(ChunkPosition, Offset));
        if (!Chunk) continue;
        Chunk->MeshDirty = true;
    }
}

void View_SetEntity(view *View, const msg_set_entity *SetEntity)
{
    while (SetEntity->Id >= View->EntityMap.Capacity)
    {
        View->EntityMap.Capacity <<= 1;
        View->EntityMap.Entities = realloc(View->EntityMap.Entities, View->EntityMap.Capacity * sizeof(entity));
    }

    View->EntityMap.Entities[SetEntity->Id] = SetEntity->Entity;
}

void View_Init(view *View)
{
    View_SetPosition(View, (ivec2){ 0 });

    for (i32 y = 0; y < LOADED_CHUNKS_DIM; ++y)
    for (i32 x = 0; x < LOADED_CHUNKS_DIM; ++x)
    {
        View->Chunks[y][x].Mesh = Mesh_Create();
    }

    View->EntityMap.Count = 0;
    View->EntityMap.Capacity = 256;
    View->EntityMap.Entities = malloc(View->EntityMap.Capacity * sizeof(entity));
}

f32 View_TraceRay(view *View, vec3 RayOrigin, vec3 RayDirection, f32 RayLength, trace_result *Result)
{
    return Phys_TraceRay(View_GetBlock, View, RayOrigin, RayDirection, RayLength, Result);
}

vec3 View_CheckMoveBox(const view *View, box Box, vec3 Move)
{
    return Phys_CheckMoveBox(View_GetBlock, View, Box, Move);
}
