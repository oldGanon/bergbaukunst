
enum entity_type
{
    ENTITY_NONE = 0,
    ENTITY_PLAYER,
    ENTITY_MOB,
};

typedef struct entity
{
    u32 Type;
    vec3 Position;
    f32 Yaw, Pitch;
} entity;

enum server_entity_flags
{
    ENTITY_NO_FLAGS,
    ENTITY_CHANGED = 1 << 0,
};

typedef struct server_entity
{
    entity Entity;

    u32 Flags;
} server_entity;

/******************/
/* IMPLEMENTATION */
/******************/

box Entity_Box(const entity *Entity)
{
    box Box;
    switch (Entity->Type)
    {
        case ENTITY_PLAYER:
        {
            Box = (box) {
                .Min = (vec3){-0.25f,-0.25f,-1.75f },
                .Max = (vec3){ 0.25f, 0.25f, 0.15f },
            };
        } break;

        case ENTITY_MOB:
        {
            Box = (box) {
                .Min = (vec3) {-1.0f,-1.0f,-1.0f},
                .Max = (vec3) { 1.0f, 1.0f, 1.0f},
            };
        } break;

        default: Box = Box_Empty(); break;
    }

    Box = Box_Move(Box, Entity->Position);

    return Box;
}

/*---------------------------------------------------------------------------*/

typedef struct entity_manager
{
    server_entity *Entities;
    u32 Capacity;
    u32 Count;
} entity_manager;

entity_manager EntityManager_Create(void);
void EntityManager_Destroy(entity_manager *Manager);
u32 Entity_Spawn(entity_manager *Manager, entity Entity);
void Entity_Destroy(entity_manager *Manager, u32 EntityId);
entity *EntityManager_GetEntity(entity_manager *Manager, u32 EntityId);
void EntityManager_EntityChanged(entity_manager *Manager, u32 EntityId);

/******************/
/* IMPLEMENTATION */
/******************/

entity_manager EntityManager_Create(void)
{
    u32 Capacity = 256;
    return (entity_manager) {   
        .Count = 0,
        .Capacity = Capacity,
        .Entities = malloc((Capacity + 1) * sizeof(server_entity)),
    };
}

void EntityManager_Destroy(entity_manager *Manager)
{
    free(Manager->Entities);
    *Manager = (entity_manager){ 0 };
}

u32 Entity_Spawn(entity_manager *Manager, entity Entity)
{
    while (Manager->Count >= Manager->Capacity)
    {
        Manager->Capacity = Manager->Capacity << 1;
        Manager->Entities = realloc(Manager->Entities, (Manager->Capacity + 1) * sizeof(server_entity));
    }

    for (u32 i = 1; i <= Manager->Capacity; ++i)
    {
        if (Manager->Entities[i].Entity.Type != ENTITY_NONE)
            continue;

        Manager->Entities[i] =  (server_entity) { .Entity = Entity };
        return i;
    }

    assert(false);
    return 0;
}

void Entity_Destroy(entity_manager *Manager, u32 EntityId)
{
    Manager->Entities[EntityId] = (server_entity) { 0 };
}

u32 EntityManager_First(entity_manager *Manager)
{
    for (u32 i = 1; i <= Manager->Capacity; ++i)
        if (Manager->Entities[i].Entity.Type != ENTITY_NONE)
            return i;
    return Manager->Capacity + 1;
}

u32 EntityManager_Next(entity_manager *Manager, u32 EntityId)
{
    for (u32 i = EntityId + 1; i <= Manager->Capacity; ++i)
        if (Manager->Entities[i].Entity.Type != ENTITY_NONE)
            return i;
    return Manager->Capacity + 1;
}

u32 EntityManager_Last(entity_manager *Manager)
{
    return Manager->Capacity;
}

#define FOREACH_ENTITY(ENTITY_ID, MANAGER) \
    for (u32 ENTITY_ID = EntityManager_First(MANAGER); \
         ENTITY_ID <= EntityManager_Last(MANAGER); \
         ENTITY_ID = EntityManager_Next(MANAGER, ENTITY_ID))


entity *EntityManager_GetEntity(entity_manager *Manager, u32 EntityId)
{
    if ((EntityId == 0) || (EntityId > Manager->Capacity)) return 0;
    return &Manager->Entities[EntityId].Entity;
}

void EntityManager_EntityChanged(entity_manager *Manager, u32 EntityId)
{
    if ((EntityId == 0) || (EntityId > Manager->Capacity)) return;
    server_entity *Entity = &Manager->Entities[EntityId];
    if (Entity->Entity.Type == ENTITY_NONE) return;
    Entity->Flags |= ENTITY_CHANGED;
}
