
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
    vec3 Velocity;
    vec3 Acceleration;
    bool OnGround;
    bool Jump;
    f32 Yaw, Pitch;
    i32 Leben;

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
                .Min = (vec3) {-0.5f,-0.5f,-0.5f},
                .Max = (vec3) { 0.5f, 0.5f, 0.5f},
            };
        } break;

        default: Box = Box_Empty(); break;
    }

    Box = Box_Move(Box, Entity->Position);

    return Box;
}
