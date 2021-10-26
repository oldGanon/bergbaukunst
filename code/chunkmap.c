
typedef struct world_chunk_map
{
    struct world_chunk *Chunks;
    struct world_chunk_map_value *Values;
    u64 Capacity;
    u64 Count;
    u64 Mask;  
} world_chunk_map;

world_chunk_map ChunkMap_Create(void);
void ChunkMap_Destroy(world_chunk_map *Map);
world_chunk *ChunkMap_AllocateChunk(world_chunk_map *Map, ivec2 Position);
void ChunkMap_DestroyChunk(world_chunk_map *Map, ivec2 Position);
world_chunk *ChunkMap_GetChunkById(const world_chunk_map *Map, u64 ChunkId);
world_chunk *ChunkMap_GetChunk(const world_chunk_map *Map, ivec2 Position);
u64 ChunkMap_GetChunkId(const world_chunk_map *Map, ivec2 Position);

/******************/
/* IMPLEMENTATION */
/******************/

typedef struct world_chunk_map_value
{
    u64 ChunkId;
    u64 Hash;
    u64 Coord;
} world_chunk_map_value;

#define CHUNK_MAP_MAXLOAD_PERCENT 90

inline u64 ChunkMap_Coord(ivec2 Position)
{
    return ((u64)Position.x << 32) | ((u64)Position.y & 0xFFFFFFFFULL);
}

inline u64 ChunkMap_Hash(u64 Coord)
{
    u64 Hash = Hash_U64(Coord);
    return Hash;
}

inline index ChunkMap_HashIndex(const world_chunk_map *Map, u64 Hash)
{
    return Hash & Map->Mask;
}

inline u64 ChunkMap_ProbeDist(const world_chunk_map *Map, u64 Pos)
{
    return (Pos - ChunkMap_HashIndex(Map, Map->Values[Pos].Hash)) & Map->Mask;
}

inline u64 ChunkMap_GetIndex(const world_chunk_map *Map, ivec2 Position)
{
    u64 Coord = ChunkMap_Coord(Position);
    u64 Hash = ChunkMap_Hash(Coord);

    u64 Pos = ChunkMap_HashIndex(Map, Hash);
    u64 Dist = 0;
    for (;;)
    {
        if (Map->Values[Pos].ChunkId == 0)
            return SIZE_MAX;
        if (Dist > ChunkMap_ProbeDist(Map, Pos))
            return SIZE_MAX;
        if ((Map->Values[Pos].Hash == Hash) &&
            (Map->Values[Pos].Coord == Coord))
            return Pos;

        Pos = (Pos + 1) & Map->Mask;
        ++Dist;
    }
}

inline void ChunkMap_RemoveIndexMasked(world_chunk_map *Map, u64 Index, u64 Mask)
{
    for (;;)
    {
        u64 Next = (Index + 1) & Mask;
        if (Map->Values[Next].ChunkId == 0)
            break;
        if (ChunkMap_ProbeDist(Map, Next) == 0)
            break;

        Map->Values[Index] = Map->Values[Next];
        Index = Next;
    }

    Map->Values[Index] = (world_chunk_map_value){ 0 };
    --Map->Count;
}

inline void ChunkMap_RemoveIndex(world_chunk_map *Map, u64 Index)
{
    ChunkMap_RemoveIndexMasked(Map, Index, Map->Mask);
}

inline void ChunkMap_InsertValue(world_chunk_map *Map, world_chunk_map_value Value)
{
    u64 Pos = ChunkMap_HashIndex(Map, Value.Hash);
    u64 Dist = 0;
    for (;;)
    {
        if (Map->Values[Pos].ChunkId == 0)
        {
            Map->Count++;
            Map->Values[Pos] = Value;
            return;
        }

        u64 Probe = ChunkMap_ProbeDist(Map, Pos);
        if (Probe < Dist)
        {
            world_chunk_map_value T = Map->Values[Pos];
            Map->Values[Pos] = Value;
            Value = T;
            
            Dist = Probe;
        }

        Pos = (Pos + 1) & Map->Mask;
        ++Dist;
    }
}

inline void ChunkMap_InsertChunkId(world_chunk_map *Map, u64 ChunkId, ivec2 Position)
{
    u64 Coord = ChunkMap_Coord(Position);
    u64 Hash = ChunkMap_Hash(Coord);
    ChunkMap_InsertValue(Map, (world_chunk_map_value){ ChunkId, Hash, Coord });
}

inline void ChunkMap_Grow(world_chunk_map *Map)
{
    if ((((Map->Count * 100) / CHUNK_MAP_MAXLOAD_PERCENT) < Map->Capacity))
        return;

    u64 OldCapacity = Map->Capacity;
    u64 OldMask = Map->Mask;
    world_chunk_map_value *OldValues = Map->Values;

    Map->Count = 0;
    Map->Capacity <<= 1;
    Map->Mask = Map->Capacity - 1;
    Map->Chunks = realloc(Map->Chunks, sizeof(world_chunk) * (Map->Capacity + 1));
    Map->Values = malloc(sizeof(world_chunk_map_value) * Map->Capacity);

    for (index i = 0; i < OldCapacity; ++i)
        if (OldValues[i].ChunkId)
            ChunkMap_InsertValue(Map, OldValues[i]);

    free(OldValues);
}

//
//
//

world_chunk_map ChunkMap_Create(void)
{
    u64 Capacity = 16 * 16 * 4;
    world_chunk_map Map = {
        .Chunks = malloc(sizeof(world_chunk) * (Capacity + 1)),
        .Values = malloc(sizeof(world_chunk_map_value) * Capacity),
        .Capacity = Capacity,
        .Count = 0,
        .Mask = Capacity - 1,
    };

    Map.Chunks[0].Flags = CHUNK_ALLOCATED;

    return Map;
}

void ChunkMap_Destroy(world_chunk_map *Map)
{
    free(Map->Chunks);
    free(Map->Values);
}

world_chunk *ChunkMap_AllocateChunk(world_chunk_map *Map, ivec2 Position)
{
    assert(ChunkMap_GetIndex(Map, Position) == SIZE_MAX);

    ChunkMap_Grow(Map);

    for (u64 i = 0; i < Map->Capacity; ++i)
    {
        world_chunk *Chunk = Map->Chunks + i;
        if (Chunk->Flags & CHUNK_ALLOCATED)
            continue;

        ChunkMap_InsertChunkId(Map, i, Position);
        WorldChunk_Init(Chunk, Position);
        return Chunk;
    }

    assert(false);
    return 0;
}

void ChunkMap_DestroyChunk(world_chunk_map *Map, ivec2 Position)
{
    u64 Index = ChunkMap_GetIndex(Map, Position);

    world_chunk *Chunk = Map->Chunks + Map->Values[Index].ChunkId;
    WorldChunk_Clear(Chunk, Position);

    ChunkMap_RemoveIndex(Map, Index);
}

world_chunk *ChunkMap_GetChunkById(const world_chunk_map *Map, u64 ChunkId)
{
    if (!ChunkId) return 0;
    return Map->Chunks + ChunkId;
}

u64 ChunkMap_GetChunkId(const world_chunk_map *Map, ivec2 Position)
{
    u64 Index = ChunkMap_GetIndex(Map, Position);
    if (Index == SIZE_MAX) return 0;
    return Map->Values[Index].ChunkId;
}

world_chunk *ChunkMap_GetChunk(const world_chunk_map *Map, ivec2 Position)
{
    u64 ChunkId = ChunkMap_GetChunkId(Map, Position);
    if (!ChunkId) return 0;
    return ChunkMap_GetChunkById(Map, ChunkId);
}

/*********************/
/* WORLD CHUNK GROUP */
/*********************/

typedef struct world_chunk_group
{
    ivec2 Position;
    world_chunk *Chunks[3][3];
} world_chunk_group;

world_chunk_group ChunkMap_GetGroup(world_chunk_map *Map, ivec2 ChunkPosition)
{
    world_chunk_group ChunkGroup = { .Position = ChunkPosition };
    for (u32 y = 0; y < 3; ++y)
    for (u32 x = 0; x < 3; ++x)
    {
        ivec2 Offset = { x - 1, y - 1 };
        ivec2 ChunkPos = iVec2_Add(ChunkPosition, Offset);
        ChunkGroup.Chunks[y][x] = ChunkMap_GetChunk(Map, ChunkPos);
    }
    return ChunkGroup;
}

bool ChunkGroup_Complete(const world_chunk_group *ChunkGroup)
{
    for (u32 y = 0; y < 3; ++y)
    for (u32 x = 0; x < 3; ++x)
        if (!ChunkGroup->Chunks[y][x])
            return false;
    return true;
}

block ChunkGroup_GetBlock(const world_chunk_group *ChunkGroup, ivec3 WorldPosition)
{
    ivec2 GroupPosition = iVec2_Sub(iVec2_ShiftRight(WorldPosition.xy, CHUNK_WIDTH_SHIFT), ChunkGroup->Position);
    if (GroupPosition.x < -1 || GroupPosition.x > 1 || 
        GroupPosition.y < -1 || GroupPosition.y > 1)
        return DEFAULT_BLOCK;

    world_chunk *Chunk = ChunkGroup->Chunks[GroupPosition.y + 1][GroupPosition.x + 1];
    return WorldChunk_GetBlock(Chunk, WorldPosition);
}

void ChunkGroup_SetBlock(const world_chunk_group *ChunkGroup, ivec3 WorldPosition, block Block)
{
    ivec2 GroupPosition = iVec2_Sub(iVec2_ShiftRight(WorldPosition.xy, CHUNK_WIDTH_SHIFT), ChunkGroup->Position);
    if (GroupPosition.x < -1 || GroupPosition.x > 1 || 
        GroupPosition.y < -1 || GroupPosition.y > 1)
        return;

    world_chunk *Chunk = ChunkGroup->Chunks[GroupPosition.y + 1][GroupPosition.x + 1];
    WorldChunk_SetBlock(Chunk, WorldPosition, Block);
}
