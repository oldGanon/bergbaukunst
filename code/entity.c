
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

/******************/
/* IMPLEMENTATION */
/******************/

box Entity_Box(const entity *Entity)
{
    switch (Entity->Type)
    {
        case ENTITY_PLAYER:
            return (box){
                .Min = Vec3_Sub(Entity->Position, (vec3){ 0.25f, 0.25f, 1.85f }),
                .Max = Vec3_Add(Entity->Position, (vec3){ 0.25f, 0.25f, 0.15f }),
            };
        case ENTITY_MOB:
            return (box) {
                .Min = Vec3_Sub(Entity->Position, (vec3) { 1.0f, 1.0f, 1.0f}),
                .Max = Vec3_Add(Entity->Position, (vec3) { 1.0f, 1.0f, 1.0f}),
            };

        default: return Box_Empty();
    }
}

/*---------------------------------------------------------------------------*/

typedef struct entity_manager
{
    entity *Entities;
    u32 Capacity;
    u32 Count;
} entity_manager;

u32 Entity_Spawn(entity_manager *Manager, entity Entity);
void Entity_Destroy(entity_manager *Manager, u32 EntityId);
entity *EntityManager_GetEntity(entity_manager *Manager, u32 EntityId);
entity_manager EntityManager_Create(void);
void EntityManager_Destroy(entity_manager *Manager);

/******************/
/* IMPLEMENTATION */
/******************/

u32 Entity_Spawn(entity_manager *Manager, entity Entity)
{
    while (Manager->Count >= Manager->Capacity)
    {
        Manager->Capacity = Manager->Capacity << 1;
        Manager->Entities = realloc(Manager->Entities, (Manager->Capacity + 1) * sizeof(entity));
    }

    for (u32 i = 1; i <= Manager->Capacity; ++i)
    {
        if (Manager->Entities[i].Type != ENTITY_NONE)
            continue;

        Manager->Entities[i] = Entity;
        return i;
    }

    assert(false);
    return 0;
}

void Entity_Destroy(entity_manager *Manager, u32 EntityId)
{
    Manager->Entities[EntityId] = (entity) { 0 };
}

entity *EntityManager_GetEntity(entity_manager *Manager, u32 EntityId)
{
    if ((EntityId == 0) || (EntityId > Manager->Capacity))
        return 0;
    return &Manager->Entities[EntityId];
}

entity_manager EntityManager_Create(void)
{
    u32 Capacity = 256;
    return (entity_manager) {   
        .Count = 0,
        .Capacity = Capacity,
        .Entities = malloc((Capacity + 1) * sizeof(entity)),
    };
}

void EntityManager_Destroy(entity_manager *Manager)
{
    free(Manager->Entities);
    *Manager = (entity_manager){ 0 };
}
