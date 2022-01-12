
typedef struct world_entity_manager
{
    world_entity *Entities;
    u32 Capacity;
    u32 Count;
} world_entity_manager;

world_entity_manager EntityManager_Create(void);
void EntityManager_Destroy(world_entity_manager *Manager);
u32 Entity_Spawn(world_entity_manager *Manager, entity Entity);
void Entity_Destroy(world_entity_manager *Manager, u32 EntityId);
entity *EntityManager_GetEntity(world_entity_manager *Manager, u32 EntityId);
void EntityManager_EntitySetDirty(world_entity_manager *Manager, u32 EntityId);

/******************/
/* IMPLEMENTATION */
/******************/

world_entity_manager EntityManager_Create(void)
{
    u32 Capacity = 256;
    return (world_entity_manager) {   
        .Count = 0,
        .Capacity = Capacity,
        .Entities = malloc((Capacity + 1) * sizeof(world_entity)),
    };
}

void EntityManager_Destroy(world_entity_manager *Manager)
{
    free(Manager->Entities);
    *Manager = (world_entity_manager){ 0 };
}

u32 Entity_Spawn(world_entity_manager *Manager, entity Entity)
{
    while (Manager->Count >= Manager->Capacity)
    {
        Manager->Capacity = Manager->Capacity << 1;
        Manager->Entities = realloc(Manager->Entities, (Manager->Capacity + 1) * sizeof(world_entity));
    }

    for (u32 i = 1; i <= Manager->Capacity; ++i)
    {
        if (Manager->Entities[i].Base.Type != ENTITY_NONE)
            continue;

        Manager->Entities[i] =  (world_entity) { .Base = Entity };
        return i;
    }

    assert(false);
    return 0;
}

entity *EntityManager_GetEntity(world_entity_manager *Manager, u32 EntityId)
{
    if ((EntityId == 0) || (EntityId > Manager->Capacity)) return 0;
    return &Manager->Entities[EntityId].Base;
}

void EntityManager_EntitySetDirty(world_entity_manager *Manager, u32 EntityId)
{
    if ((EntityId == 0) || (EntityId > Manager->Capacity)) return;
    world_entity *Entity = &Manager->Entities[EntityId];
    if (Entity->Base.Type == ENTITY_NONE) return;
    Entity->Flags |= ENTITY_DIRTY;
}

void Entity_Destroy(world_entity_manager *Manager, u32 EntityId)
{
    if ((EntityId == 0) || (EntityId > Manager->Capacity)) return;
    Manager->Entities[EntityId] = (world_entity) { .Flags = ENTITY_DIRTY };
}

u32 EntityManager_First(world_entity_manager *Manager)
{
    for (u32 i = 1; i <= Manager->Capacity; ++i)
        if (Manager->Entities[i].Base.Type != ENTITY_NONE)
            return i;
    return Manager->Capacity + 1;
}

u32 EntityManager_Next(world_entity_manager *Manager, u32 EntityId)
{
    for (u32 i = EntityId + 1; i <= Manager->Capacity; ++i)
        if (Manager->Entities[i].Base.Type != ENTITY_NONE)
            return i;
    return Manager->Capacity + 1;
}

u32 EntityManager_Last(world_entity_manager *Manager)
{
    return Manager->Capacity;
}

u32 EntityManager_FirstType(world_entity_manager *Manager, u32 EntityType)
{
    for (u32 i = 1; i <= Manager->Capacity; ++i)
        if (Manager->Entities[i].Base.Type == EntityType)
            return i;
    return Manager->Capacity + 1;
}

u32 EntityManager_NextType(world_entity_manager *Manager, u32 EntityId, u32 EntityType)
{
    for (u32 i = EntityId + 1; i <= Manager->Capacity; ++i)
        if (Manager->Entities[i].Base.Type == EntityType)
            return i;
    return Manager->Capacity + 1;
}

#define FOREACH_ENTITY(ENTITY_ID, MANAGER) \
    for (u32 ENTITY_ID = EntityManager_First((MANAGER)); \
         ENTITY_ID <= EntityManager_Last((MANAGER)); \
         ENTITY_ID = EntityManager_Next((MANAGER), (ENTITY_ID)))

#define FOREACH_ENTITY_OF_TYPE(ENTITY_ID, MANAGER, ENTITY_TYPE) \
    for (u32 ENTITY_ID = EntityManager_FirstType((MANAGER), (ENTITY_TYPE)); \
         ENTITY_ID <= EntityManager_Last((MANAGER)); \
         ENTITY_ID = EntityManager_NextType((MANAGER), (ENTITY_ID), (ENTITY_TYPE)))
