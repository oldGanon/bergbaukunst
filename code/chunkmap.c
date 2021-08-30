
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
chunk *ChunkMap_AllocateChunk(chunk_map *Map, i32 x, i32 z);
void ChunkMap_DeleteChunk(chunk_map *Map, i32 x, i32 z);
chunk *ChunkMap_GetChunk(chunk_map *Map, i32 x, i32 z);

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

inline u64 ChunkMap_FNV1a(const void *Data, size Length)
{
    u64 Hash = 14695981039346656037ULL;
    const u8 *Bytes = Data;
    while (Length--) Hash = (Hash ^ *Bytes++) * 1099511628211ULL;
    return Hash;
}

inline u64 ChunkMap_Coord(i32 x, i32 z)
{
    return ((u64)x & 0xFFFFFFFF)  | ((u64)z << 32);
}

inline u64 ChunkMap_Hash(u64 Coord)
{
    return ChunkMap_FNV1a(&Coord, 8);
}

inline index ChunkMap_HashIndex(chunk_map *Map, u64 Hash)
{
    return Hash & Map->Mask;
}

inline u64 ChunkMap_ProbeDist(chunk_map *Map, u64 Pos)
{
    return (Pos - ChunkMap_HashIndex(Map, Map->Values[Pos].Hash)) & Map->Mask;
}

inline u64 ChunkMap_GetIndex(chunk_map *Map, i32 x, i32 z)
{
    u64 Coord = ChunkMap_Coord(x, z);
    u64 Hash = ChunkMap_Hash(Coord);

    if (!Hash) Hash = 1;
    u64 Pos = ChunkMap_HashIndex(Map, Hash);
    u64 Dist = 0;
    for (;;)
    {
        if (Map->Values[Pos].Hash == 0)
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
        if (Map->Values[Next].Hash == 0)
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
        if (Map->Values[Pos].Hash == 0)
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

    u64 OldMask = Map->Mask;
    u64 OldCapacity = Map->Capacity;
    Map->Capacity <<= 1;
    Map->Mask = Map->Capacity - 1;
    Map->Chunks = realloc(Map->Chunks, sizeof(chunk) * Map->Capacity);
    Map->Values = realloc(Map->Values, sizeof(chunk_map_value) * Map->Capacity);
    // for (ez_index i = OldCapacity; i < Map->Capacity; ++i)
    //     Map->Values[i] = { 0 };
    for (index i = 0; i < OldCapacity; ++i)
    if (Map->Values[i].Hash & OldCapacity)
    {
        ChunkMap_InsertValue(Map, Map->Values[i]);
        ChunkMap_RemoveIndexMasked(Map, i--, OldMask);
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

    return Map;
}

void ChunkMap_Delete(chunk_map *Map)
{
    free(Map->Chunks);
    free(Map->Values);
}

chunk *ChunkMap_AllocateChunk(chunk_map *Map, i32 x, i32 z)
{
    assert(ChunkMap_GetIndex(Map, x, z) == SIZE_MAX);

    ChunkMap_Grow(Map);

    for (u64 i = 0; i < Map->Capacity; ++i)
    {
        chunk *Chunk = Map->Chunks + i;
        if (!Chunk->Allocated)
        {
            ChunkMap_InsertChunkId(Map, i, x, z);
            Chunk_Create(Chunk, x, z);
            return Chunk;
        }
    }

    assert(false);
    return 0;
}

void ChunkMap_DeleteChunk(chunk_map *Map, i32 x, i32 z)
{
    u64 Index = ChunkMap_GetIndex(Map, x, z);

    chunk *Chunk = Map->Chunks + Map->Values[Index].ChunkId;
    Chunk_Delete(Chunk, x, z);

    ChunkMap_RemoveIndex(Map, Index);
}

chunk *ChunkMap_GetChunk(chunk_map *Map, i32 x, i32 z)
{
    u64 Index = ChunkMap_GetIndex(Map, x, z);
    if (Index == SIZE_MAX) return 0;
    return Map->Chunks + Map->Values[Index].ChunkId;
}