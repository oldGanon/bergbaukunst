
enum world_chunk_flags
{
    CHUNK_EMPTY     = 0,
    CHUNK_ALLOCATED = 1 << 0,
    CHUNK_GENERATED = 1 << 1,
    CHUNK_DECORATED = 1 << 2,
    CHUNK_DIRTY     = 1 << 3,

    CHUNK_COMPLETE  = (CHUNK_ALLOCATED | CHUNK_GENERATED | CHUNK_DECORATED),
};

typedef struct world_chunk
{
    chunk Base;

    ivec3 DirtyMin;
    ivec3 DirtyMax;
    u32 Flags;
} world_chunk;

void WorldChunk_MarkClean(world_chunk *Chunk)
{
    Chunk->DirtyMin = (ivec3){CHUNK_WIDTH_MASK,CHUNK_WIDTH_MASK,CHUNK_HEIGHT_MASK};
    Chunk->DirtyMax = (ivec3){0,0,0};
    Chunk->Flags &= ~CHUNK_DIRTY;
}

void WorldChunk_MarkDirty(world_chunk *Chunk)
{
    Chunk->DirtyMin = (ivec3){0,0,0};
    Chunk->DirtyMax = (ivec3){CHUNK_WIDTH_MASK,CHUNK_WIDTH_MASK,CHUNK_HEIGHT_MASK};
    Chunk->Flags |= CHUNK_DIRTY;
}

void WorldChunk_MarkBlockDirty(world_chunk *Chunk, ivec3 BlockPosition)
{
    Chunk->DirtyMin = iVec3_Min(Chunk->DirtyMin, BlockPosition);
    Chunk->DirtyMax = iVec3_Max(Chunk->DirtyMax, BlockPosition);
    Chunk->Flags |= CHUNK_DIRTY;
}

void WorldChunk_Init(world_chunk *Chunk, ivec2 Position)
{
    Chunk_Init(&Chunk->Base, Position);
    WorldChunk_MarkClean(Chunk);
    Chunk->Flags = CHUNK_ALLOCATED;
}

void WorldChunk_Clear(world_chunk *Chunk, ivec2 Position)
{
    Chunk_Clear(&Chunk->Base, Position);
    WorldChunk_MarkClean(Chunk);
    Chunk->Flags = CHUNK_EMPTY;
}

block WorldChunk_GetBlock(const world_chunk *Chunk, ivec3 WorldPosition)
{
    if (WorldPosition.z < 0)                return DEFAULT_SKY_BLOCK;
    if (WorldPosition.z > CHUNK_HEIGHT - 1) return DEFAULT_HELL_BLOCK;

    ivec3 BlockPosition = World_ToBlockPosition(WorldPosition);
    return Chunk_GetBlock(&Chunk->Base, BlockPosition);
}

void WorldChunk_SetBlock(world_chunk *Chunk, ivec3 WorldPosition, block Block)
{
    if ((WorldPosition.z < 0) || (WorldPosition.z > CHUNK_HEIGHT - 1))
        return;

    ivec3 BlockPosition = World_ToBlockPosition(WorldPosition);
    Chunk_SetBlock(&Chunk->Base, BlockPosition, Block);
    
    WorldChunk_MarkBlockDirty(Chunk, BlockPosition);
}

enum world_entity_flags
{
    ENTITY_EMPTY,
    ENTITY_DIRTY = 1 << 0,
};

typedef struct world_entity
{
    entity Base;

    u32 Flags;
} world_entity;

#include "entityman.c"
#include "chunkmap.c"
#include "worldgen.c"

#define LOADED_CHUNKS_DIM_SHIFT 5
#define LOADED_CHUNKS_DIM (1 << LOADED_CHUNKS_DIM_SHIFT)
#define LOADED_CHUNKS_DIM_MASK (LOADED_CHUNKS_DIM - 1)
#define LOADED_CHUNKS_DIST_SHIFT (LOADED_CHUNKS_DIM_SHIFT - 1)
#define LOADED_CHUNKS_DIST (1 << LOADED_CHUNKS_DIST_SHIFT)
#define LOADED_CHUNKS_DIST_MASK (LOADED_CHUNKS_DIST - 1)

typedef struct world
{
    world_chunk_map ChunkMap;
    world_entity_manager EntityManager;
    world_gen Generator;
} world;

// typedef enum world_direction
// {
//     WEST,  // -x
//     EAST,  // +x
//     SOUTH, // -y
//     NORTH, // +y
//     UP,    // +z
//     DOWN,  // -z
// } world_direction;

void Mob_Update(entity* Entity, vec3 MobToPlayer, f32 Distance, world* World);


void World_Init(world *World)
{
    World->ChunkMap = ChunkMap_Create();
    World->EntityManager = EntityManager_Create();
    World->Generator = WorldGen_Create(__rdtsc());
    
    entity Test = (entity){ .Type = ENTITY_MOB, .Position = (vec3){ 0,0,120 }, .Health = 3 };
    Entity_Spawn(&World->EntityManager, Test);
}

world_chunk *World_GetChunk(world *World, ivec2 ChunkPosition)
{
    return ChunkMap_GetChunk(&World->ChunkMap, ChunkPosition);
}

const world_chunk *World_GetConstChunk(const world *World, ivec2 ChunkPosition)
{
    return ChunkMap_GetChunk(&World->ChunkMap, ChunkPosition);
}

chunk *World_GetBaseChunk(world *World, ivec2 ChunkPosition)
{
    world_chunk *Chunk = World_GetChunk(World, ChunkPosition);
    if (!Chunk) return 0;
    return &Chunk->Base;
}

const chunk *World_GetConstBaseChunk(const world *World, ivec2 ChunkPosition)
{
    const world_chunk *Chunk = World_GetConstChunk(World, ChunkPosition);
    if (!Chunk) return 0;
    return &Chunk->Base;
}

world_chunk_group World_GetChunkGroup(world *World, ivec2 ChunkPosition)
{
    return ChunkMap_GetGroup(&World->ChunkMap, ChunkPosition);
}

void World_Update(world *World)
{
    // load chunks
    FOREACH_ENTITY_OF_TYPE(PlayerId, &World->EntityManager, ENTITY_PLAYER)
    {
        entity *Player = EntityManager_GetEntity(&World->EntityManager, PlayerId);
        ivec2 CenterChunk = World_ToChunkPosition(Vec3_FloorToIVec3(Player->Position));
        ivec2 LoadedChunkDist = iVec2_Set1(LOADED_CHUNKS_DIST);
        ivec2 MinPos = iVec2_Sub(CenterChunk, LoadedChunkDist);
        ivec2 MaxPos = iVec2_Add(CenterChunk, LoadedChunkDist);
        for (i32 x = MinPos.x; x < MaxPos.x; x++)
        for (i32 y = MinPos.y; y < MaxPos.y; y++)
        {
            ivec2 ChunkPos = (ivec2){ x, y };
            world_chunk *Chunk = ChunkMap_GetChunk(&World->ChunkMap, ChunkPos);
            if (!Chunk) Chunk = ChunkMap_AllocateChunk(&World->ChunkMap, ChunkPos);
            if (!(Chunk->Flags & CHUNK_GENERATED)) WorldGen_GenerateChunk(&World->Generator, Chunk);
            world_chunk_group ChunkGroup = World_GetChunkGroup(World, ChunkPos);
            if (!(Chunk->Flags & CHUNK_DECORATED)) WorldGen_DecorateChunk(&World->Generator, ChunkGroup, ChunkPos);
        }
    }

    // update chunks
    {

    }

    // update entities
    world_entity_manager *Manager = &World->EntityManager;
    FOREACH_ENTITY(EntityId, Manager)
    {
        entity *Entity = EntityManager_GetEntity(Manager, EntityId);
        switch (Entity->Type)
        {
            case ENTITY_MOB:
            {
                FOREACH_ENTITY_OF_TYPE(PlayerId, Manager, ENTITY_PLAYER)
                {
                    entity *Player = EntityManager_GetEntity(&World->EntityManager, PlayerId);
                    vec3 MobToPlayer = Vec3_Sub(Player->Position, Entity->Position);

                    // move Mobs
                    f32 Distance = Vec3_Length(MobToPlayer);
                    Mob_Update(Entity, MobToPlayer, Distance, World);
                    EntityManager_EntitySetDirty(&World->EntityManager, EntityId);
                }
            } break;
        }
    }
}

block World_GetBlock(const world *World, ivec3 WorldPosition)
{
    ivec2 ChunkPosition = World_ToChunkPosition(WorldPosition);
    const world_chunk *Chunk = World_GetConstChunk(World, ChunkPosition);
    if (!Chunk) return DEFAULT_BLOCK; // maybe generate chunk instead?
    return WorldChunk_GetBlock(Chunk, WorldPosition);
}

void World_SetBlock(world *World, ivec3 WorldPosition, block Block)
{
    ivec2 ChunkPosition = World_ToChunkPosition(WorldPosition);
    world_chunk *Chunk = World_GetChunk(World, ChunkPosition);
    if (!Chunk) return; // maybe generate chunk instead?
    WorldChunk_SetBlock(Chunk, WorldPosition, Block);
}



u32 World_SpawnPlayer(world *World, vec3 Position)
{
    return Entity_Spawn(&World->EntityManager, (entity) {
        .Type = ENTITY_PLAYER,
        .Position = Position,
    });
}



f32 World_TraceRay(world *World, vec3 RayOrigin, vec3 RayDirection, f32 RayLength, trace_result *Result)
{
    return Phys_TraceRay(World_GetBlock, World, RayOrigin, RayDirection, RayLength, Result);
}

vec3 World_CheckMoveBox(world *World, box Box, vec3 Move)
{
    return Phys_CheckMoveBox(World_GetBlock, World, Box, Move);
}



vec3 Entity_CheckMove(entity *Entity, world *World, vec3 Move)
{
    return World_CheckMoveBox(World, Entity_Box(Entity), Move);
}

void Entity_Move(entity *Entity, world *World, vec3 Move)
{
    vec3 CheckMove = World_CheckMoveBox(World, Entity_Box(Entity), Move);
    if (Abs(CheckMove.x) < Abs(Move.x)) Entity->Velocity.x = 0;
    if (Abs(CheckMove.y) < Abs(Move.y)) Entity->Velocity.y = 0;
    if (Abs(CheckMove.z) < Abs(Move.z)) Entity->Velocity.z = 0;
    if (CheckMove.z > Move.z) Entity->OnGround = true;
    Entity->Position = Vec3_Add(Entity->Position, CheckMove);
}

void Mob_Update(entity *Entity, vec3 MobToPlayer, f32 Distance, world *World)
{
#define GRAVITY_MOB         250.0f
#define JUMP_SPEED_MOB      26.0f
#define GROUND_SPEED_MOB    200.0f
#define AIR_SPEED_MOB       20.0f
#define GROUND_FRICTION_MOB 15.0f
#define AIR_FRICTION_MOB    2.0f

    bool PlayerNear = false;
    vec3 VecEntityWishMove = (vec3){ 0 };

    if (Distance < 15.0f)
    {
        Entity->Yaw += 0.05f;
        if (Distance < 10.0f)
        {
            VecEntityWishMove = Vec3_Normalize((vec3) { MobToPlayer.x, MobToPlayer.y, 0 });
            PlayerNear = true;
        }
    }

    // Acceleration
    Entity->Acceleration = Vec3_Zero();
    f32 WishSpeed = (Entity->OnGround) ? GROUND_SPEED_MOB : AIR_SPEED_MOB;
    Entity->Acceleration = Vec3_Mul(VecEntityWishMove, Vec3_Set1(WishSpeed));

    // Gravity
    Entity->Acceleration.z -= GRAVITY_MOB;

    if (Entity->OnGround)
    {
        // Friction
        vec3 Friction = Vec3_Mul(Entity->Velocity, Vec3_Set1(-GROUND_FRICTION_MOB));
        Entity->Acceleration = Vec3_Add(Entity->Acceleration, Friction);

        // Jump
        if (Entity->Jump && Entity->OnGround)
        {
            Entity->Jump = false;
            if (Entity->Velocity.z < JUMP_SPEED_MOB)
                Entity->Velocity.z = JUMP_SPEED_MOB;
        }
        Entity->OnGround = false;
    }
    else
    {
        // Air Friction
        vec3 Friction = Vec3_Mul(Entity->Velocity, Vec3_Set1(-AIR_FRICTION_MOB));
        Entity->Acceleration = Vec3_Add(Entity->Acceleration, Friction);
    }

    // Move
    vec3 AddVelocity = Vec3_Mul(Entity->Acceleration, Vec3_Set1(0.01f));
    Entity->Velocity = Vec3_Add(Entity->Velocity, AddVelocity);
    vec3 AddPosition = Vec3_Mul(Entity->Velocity, Vec3_Set1(0.01f));

    Entity_Move(Entity, World, AddPosition);

    if (PlayerNear && Entity->OnGround && (Entity->Velocity.x == 0 || Entity->Velocity.y == 0))
        Entity->Jump = true;
}
