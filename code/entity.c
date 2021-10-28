
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
