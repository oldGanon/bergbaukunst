
typedef struct chunk_map
{
    struct chunk *Chunks;
    struct chunk_map_value *Values;
    u64 Capacity;
    u64 Count;
    u64 Mask;  
} chunk_map;

chunk_map ChunkMap_Create(void);
void ChunkMap_Delete(chunk_map *Map);
chunk *ChunkMap_AllocateChunk(chunk_map *Map, i32 x, i32 y);
void ChunkMap_DeleteChunk(chunk_map *Map, i32 x, i32 y);
chunk *ChunkMap_GetChunkById(const chunk_map *Map, u64 ChunkId);
chunk *ChunkMap_GetChunk(const chunk_map *Map, i32 x, i32 y);
u64 ChunkMap_GetChunkId(const chunk_map *Map, i32 x, i32 y);

//
//
//

typedef struct chunk_map_value
{
    u64 ChunkId;
    u64 Hash;
    u64 Coord;
} chunk_map_value;

#define CHUNK_MAP_MAXLOAD_PERCENT 90

inline u64 ChunkMap_Coord(i32 x, i32 y)
{
    return ((u64)x & 0xFFFFFFFFULL)  | ((u64)y << 32);
}

inline u64 ChunkMap_Hash(u64 Coord)
{
    u64 Hash = Hash_U64(Coord);
    return Hash;
}

inline index ChunkMap_HashIndex(const chunk_map *Map, u64 Hash)
{
    return Hash & Map->Mask;
}

inline u64 ChunkMap_ProbeDist(const chunk_map *Map, u64 Pos)
{
    return (Pos - ChunkMap_HashIndex(Map, Map->Values[Pos].Hash)) & Map->Mask;
}

inline u64 ChunkMap_GetIndex(const chunk_map *Map, i32 x, i32 y)
{
    u64 Coord = ChunkMap_Coord(x, y);
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

inline void ChunkMap_RemoveIndexMasked(chunk_map *Map, u64 Index, u64 Mask)
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

    Map->Values[Index] = (chunk_map_value){ 0 };
    --Map->Count;
}

inline void ChunkMap_RemoveIndex(chunk_map *Map, u64 Index)
{
    ChunkMap_RemoveIndexMasked(Map, Index, Map->Mask);
}

inline void ChunkMap_InsertValue(chunk_map *Map, chunk_map_value Value)
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
            chunk_map_value T = Map->Values[Pos];
            Map->Values[Pos] = Value;
            Value = T;
            
            Dist = Probe;
        }

        Pos = (Pos + 1) & Map->Mask;
        ++Dist;
    }
}

inline void ChunkMap_InsertChunkId(chunk_map *Map, u64 ChunkId, i32 x, i32 z)
{
    u64 Coord = ChunkMap_Coord(x, z);
    u64 Hash = ChunkMap_Hash(Coord);
    ChunkMap_InsertValue(Map, (chunk_map_value){ ChunkId, Hash, Coord });
}

inline void ChunkMap_Grow(chunk_map *Map)
{
    if ((((Map->Count * 100) / CHUNK_MAP_MAXLOAD_PERCENT) < Map->Capacity))
        return;

    u64 OldCapacity = Map->Capacity;
    u64 OldMask = Map->Mask;
    chunk_map_value *OldValues = Map->Values;

    Map->Count = 0;
    Map->Capacity <<= 1;
    Map->Mask = Map->Capacity - 1;
    Map->Chunks = realloc(Map->Chunks, sizeof(chunk) * Map->Capacity);
    Map->Values = malloc(sizeof(chunk_map_value) * Map->Capacity);

    for (index i = 0; i < OldCapacity; ++i)
        if (OldValues[i].ChunkId)
            ChunkMap_InsertValue(Map, OldValues[i]);

    free(OldValues);
}

inline void ChunkMap__ConnectChunk(chunk_map *Map, chunk *Chunk, u64 ChunkId)
{
    for (i32 y = -1; y <= 1; ++y)
    for (i32 x = -1; x <= 1; ++x)
    {
        u64 NeighborId =  ChunkMap_GetChunkId(Map, Chunk->x + x, Chunk->y + y);
        chunk *Neighbor = ChunkMap_GetChunkById(Map, NeighborId);
        if (Neighbor)
        {
            Chunk->Neighbors[1+y][1+x] = NeighborId;
            Neighbor->Neighbors[1-y][1-x] = ChunkId;
            Neighbor->MeshDirty = true;
        }
    }
}

inline void ChunkMap__DisonnectChunk(chunk_map *Map, chunk *Chunk)
{
    for (i32 y = -1; y <= 1; ++y)
    for (i32 x = -1; x <= 1; ++x)
    {
        u64 NeighborId = Chunk->Neighbors[1+y][1+x];
        chunk *Neighbor = ChunkMap_GetChunkById(Map, NeighborId);
        if (Neighbor)
        {
            Chunk->Neighbors[1+y][1+x] = 0;
            Neighbor->Neighbors[1-y][1-x] = 0;
        }
    }
}

//
//
//

chunk_map ChunkMap_Create(void)
{
    u64 Capacity = 16 * 16 * 4;
    chunk_map Map = {
        .Chunks = malloc(sizeof(chunk) * Capacity),
        .Values = malloc(sizeof(chunk_map_value) * Capacity),
        .Capacity = Capacity,
        .Count = 0,
        .Mask = Capacity - 1,
    };

    Map.Chunks[0].Allocated = true;

    return Map;
}

void ChunkMap_Delete(chunk_map *Map)
{
    free(Map->Chunks);
    free(Map->Values);
}


chunk *ChunkMap_AllocateChunk(chunk_map *Map, i32 x, i32 y)
{
    assert(ChunkMap_GetIndex(Map, x, y) == SIZE_MAX);

    ChunkMap_Grow(Map);

    for (u64 i = 0; i < Map->Capacity; ++i)
    {
        chunk *Chunk = Map->Chunks + i;
        if (!Chunk->Allocated)
        {
            ChunkMap_InsertChunkId(Map, i, x, y);
            Chunk_Create(Chunk, x, y);
            ChunkMap__ConnectChunk(Map, Chunk, i);
            return Chunk;
        }
    }

    assert(false);
    return 0;
}

void ChunkMap_DeleteChunk(chunk_map *Map, i32 x, i32 y)
{
    u64 Index = ChunkMap_GetIndex(Map, x, y);

    chunk *Chunk = Map->Chunks + Map->Values[Index].ChunkId;
    ChunkMap__DisonnectChunk(Map, Chunk);
    Chunk_Delete(Chunk, x, y);

    ChunkMap_RemoveIndex(Map, Index);
}

chunk *ChunkMap_GetChunkById(const chunk_map *Map, u64 ChunkId)
{
    return Map->Chunks + ChunkId;
}

u64 ChunkMap_GetChunkId(const chunk_map *Map, i32 x, i32 y)
{
    u64 Index = ChunkMap_GetIndex(Map, x, y);
    if (Index == SIZE_MAX) return 0;
    return Map->Values[Index].ChunkId;
}

chunk *ChunkMap_GetChunk(const chunk_map *Map, i32 x, i32 y)
{
    u64 ChunkId = ChunkMap_GetChunkId(Map, x, y);
    if (!ChunkId) return 0;
    return ChunkMap_GetChunkById(Map, ChunkId);
}
