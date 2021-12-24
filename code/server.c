
#include "world.c"

typedef struct server_client
{
    u32 EntityId;
    ivec2 ViewPosition;

    // ViewDistance;
    // LookYaw;
    // LookPitch;
} server_client;

typedef struct server
{
    network_server Server;
    server_client Clients[SERVER_MAX_CLIENTS];
    world World;
} server;

void Server_Init(server *Server)
{
    Network_ServerInit(&Server->Server, "4510");

    World_Init(&Server->World);
}

server_client *Server_GetClient(server *Server, u32 ClientId)
{
    if ((ClientId == 0) || (ClientId >= SERVER_MAX_CLIENTS))
        return 0;
    return Server->Clients + ClientId - 1;
}

void Server_BroadcastMessage(server *Server, msg *Message)
{
    Network_ServerBroadcastMessage(&Server->Server, Message);
}

bool ChunkInView(ivec2 ViewPosition, ivec2 ChunkPosition)
{
    if (ChunkPosition.x >= (ViewPosition.x - LOADED_CHUNKS_DIST) &&
        ChunkPosition.y >= (ViewPosition.y - LOADED_CHUNKS_DIST) &&
        ChunkPosition.x < (ViewPosition.x + LOADED_CHUNKS_DIST) &&
        ChunkPosition.y < (ViewPosition.y + LOADED_CHUNKS_DIST))
        return true;
    return false;
}

void Server_SendChunkMessage(server *Server, ivec2 ChunkPosition, msg *Message)
{
    for (u32 i = 1; i < SERVER_MAX_CLIENTS; ++i)
    {
        server_client *Client = Server_GetClient(Server, i);
        if (ChunkInView(Client->ViewPosition, ChunkPosition))
            Network_ServerSendMessage(&Server->Server, i, Message);
    }
}

void Server_ClientConnect(server *Server, u32 ClientId)
{
    server_client *Client = Server_GetClient(Server, ClientId);
    if (!Client) return;

    vec3 PlayerSpawnPosition = (vec3){ 0 };

    *Client = (server_client){ 0 };
    Client->EntityId = World_SpawnPlayer(&Server->World, PlayerSpawnPosition);
    Client->ViewPosition = World_ToChunkPosition(Vec3_FloorToIVec3(PlayerSpawnPosition));

    entity *Player = EntityManager_GetEntity(&Server->World.EntityManager, Client->EntityId);

    msg Message;

    // set client player position
    Message_PlayerState(&Message, PlayerSpawnPosition);
    Network_ServerSendMessage(&Server->Server, ClientId, &Message);

    // set client view position
    Message_ViewPosition(&Message, Client->ViewPosition);
    Network_ServerSendMessage(&Server->Server, ClientId, &Message);

    // send chunk data
    ivec2 LoadedChunkDist = iVec2_Set1(LOADED_CHUNKS_DIST);
    ivec2 MinPos = iVec2_Sub(Client->ViewPosition, LoadedChunkDist);
    ivec2 MaxPos = iVec2_Add(Client->ViewPosition, LoadedChunkDist);
    for (i32 x = MinPos.x; x < MaxPos.x; x++)
    for (i32 y = MinPos.y; y < MaxPos.y; y++)
    {
        ivec2 ChunkPos = (ivec2){ x, y };
        world_chunk *Chunk = World_GetChunk(&Server->World, ChunkPos);
        if (!Chunk) continue;
        if ((Chunk->Flags & CHUNK_COMPLETE) != (CHUNK_COMPLETE)) continue;

        Message_ChunkData(&Message, &Chunk->Base, (ivec3){0,0,0}, (ivec3){CHUNK_WIDTH_MASK,CHUNK_WIDTH_MASK,CHUNK_HEIGHT_MASK});
        Network_ServerSendMessage(&Server->Server, ClientId, &Message);
    }

    // send entity data
    world_entity_manager *Manager = &Server->World.EntityManager;
    FOREACH_ENTITY(EntityId, Manager)
    {
        entity *Entity = EntityManager_GetEntity(Manager, EntityId);
        if (Entity->Type == ENTITY_NONE) continue;

        ivec2 Chunk = World_ToChunkPosition(Vec3_FloorToIVec3(Entity->Position));
        Message_SetEntity(&Message, EntityId, Entity);
        Server_SendChunkMessage(Server, Chunk, &Message);
    }
}

void Server_ClientDisconnect(server *Server, u32 ClientId, const msg_disconnect *Disconnect)
{
    server_client *Client = Server_GetClient(Server, ClientId);
    if (!Client) return;

    entity *Player = EntityManager_GetEntity(&Server->World.EntityManager, Client->EntityId);
    if (!Player) return;

    ivec2 Chunk = World_ToChunkPosition(Vec3_FloorToIVec3(Player->Position));
    *Player = (entity){ 0 };

    msg Message;
    Message_SetEntity(&Message, Client->EntityId, Player);
    Server_SendChunkMessage(Server, Chunk, &Message);
}

void Server_ClientUpdatePlayerState(server *Server, u32 ClientId, const msg_player_state *PlayerState)
{
    server_client *Client = Server_GetClient(Server, ClientId);
    if (!Client) return;

    entity *Player = EntityManager_GetEntity(&Server->World.EntityManager, Client->EntityId);
    if (!Player) return;

    ivec2 PlayerChunk = World_ToChunkPosition(Vec3_FloorToIVec3(Player->Position));

    msg Message;

    // update client view
    ivec2 NewViewPosition = PlayerChunk;
    ivec2 OldViewPosition = Client->ViewPosition;
    if ((NewViewPosition.x != OldViewPosition.x) ||
        (NewViewPosition.y != OldViewPosition.y))
    {
        Client->ViewPosition = NewViewPosition;
        Message_ViewPosition(&Message, Client->ViewPosition);
        Network_ServerSendMessage(&Server->Server, ClientId, &Message);

        i32 MinX = Min(NewViewPosition.x, OldViewPosition.x) - LOADED_CHUNKS_DIST;
        i32 MaxX = Max(NewViewPosition.x, OldViewPosition.x) + LOADED_CHUNKS_DIST;
        i32 MinY = Min(NewViewPosition.y, OldViewPosition.y) - LOADED_CHUNKS_DIST;
        i32 MaxY = Max(NewViewPosition.y, OldViewPosition.y) + LOADED_CHUNKS_DIST;
        for (i32 y = MinY; y < MaxY; ++y)
        for (i32 x = MinX; x < MaxX; ++x)
        {
            ivec2 ChunkPos = (ivec2){ x, y };
            if (ChunkInView(OldViewPosition, ChunkPos))
                continue;

            world_chunk *Chunk = World_GetChunk(&Server->World, ChunkPos);
            if (!Chunk) continue;
            if ((Chunk->Flags & CHUNK_COMPLETE) != (CHUNK_COMPLETE)) continue;

            Message_ChunkData(&Message, &Chunk->Base, (ivec3){0,0,0}, (ivec3){CHUNK_WIDTH_MASK,CHUNK_WIDTH_MASK,CHUNK_HEIGHT_MASK});
            Network_ServerSendMessage(&Server->Server, ClientId, &Message);
        }
    }

    // update player entity
    Player->Position = PlayerState->Position;
    Message_SetEntity(&Message, Client->EntityId, Player);
    Server_SendChunkMessage(Server, PlayerChunk, &Message);
}

void Server_ClientPlaceBlock(server *Server, u32 ClientId, const msg_place_block *PlaceBlock)
{
    server_client *Client = Server_GetClient(Server, ClientId);
    if (!Client) return;

    entity *Player = EntityManager_GetEntity(&Server->World.EntityManager, Client->EntityId);
    if (!Player) return;

    block Block = (block){ .Id = BLOCK_ID_GRAS };
    box ClientPlayerBox = Entity_Box(Player);
    if (!Block_BoxIntersect(Block, PlaceBlock->Position, ClientPlayerBox))
        World_SetBlock(&Server->World, PlaceBlock->Position, Block);
}

void Server_ClientBreakBlock(server *Server, u32 ClientId, const msg_break_block *BreakBlock)
{
    server_client *Client = Server_GetClient(Server, ClientId);
    if (!Client) return;

    block Block = (block){ .Id = BLOCK_ID_AIR };
    World_SetBlock(&Server->World, BreakBlock->Position, Block);
}
void Server_ClientEntityHurt(server* Server, u32 ClientId, const msg_entity_hurt *EntityHurt)
{
    world_entity_manager *Manager = &Server->World.EntityManager;
    entity* Entity = EntityManager_GetEntity(Manager, EntityHurt->ID);
    if(Entity->Leben == 1)
    {
        Entity_DestroySetDirty(Manager, EntityHurt->ID);
    }
    else
    {
        Entity->Velocity = EntityHurt->Entity.Velocity;
        Entity->Leben -= 1;
    }
    EntityManager_EntityDirty(Manager, EntityHurt->ID);

}

void Server_Update(server *Server)
{
    // receive messages
    msg Message;
    for (u32 i = 1; i < SERVER_MAX_CLIENTS; ++i)
    {
        while (Network_ServerGetMessage(&Server->Server, i, &Message))
        {
            switch (Message.Header.Type)
            {
                case MSG_DISCONNECT:   Server_ClientDisconnect(Server, i, &Message.Disconnect); break;
                case MSG_PLAYER_STATE: Server_ClientUpdatePlayerState(Server, i, &Message.PlayerState); break;

                case MSG_PLACE_BLOCK:  Server_ClientPlaceBlock(Server, i, &Message.PlaceBlock); break;
                case MSG_BREAK_BLOCK:  Server_ClientBreakBlock(Server, i, &Message.BreakBlock); break;
                case MSG_ENTITY_HURT:  Server_ClientEntityHurt(Server, i, &Message.EntityHurt); break;
                default: break;
            }
        }
    }

    // update
    World_Update(&Server->World);

    // send chunks
    world_chunk_map *ChunkMap = &Server->World.ChunkMap;
    for (u32 i = 1; i <= ChunkMap->Capacity; ++i)
    {
        world_chunk *Chunk = ChunkMap->Chunks + i;
        if ((Chunk->Flags & (CHUNK_COMPLETE | CHUNK_DIRTY)) == (CHUNK_COMPLETE | CHUNK_DIRTY))
        {
            Chunk->Flags &= ~CHUNK_DIRTY;
            Message_ChunkData(&Message, &Chunk->Base, Chunk->DirtyMin, Chunk->DirtyMax);
            Server_SendChunkMessage(Server, Chunk->Base.Position, &Message);
            Chunk->DirtyMin = (ivec3){CHUNK_WIDTH_MASK,CHUNK_WIDTH_MASK,CHUNK_HEIGHT_MASK};
            Chunk->DirtyMax = (ivec3){0,0,0};
        }
    }

    // send entities
    world_entity_manager *Manager = &Server->World.EntityManager;
    for (u32 i = 1; i <= Manager->Capacity; ++i)
    {
        world_entity *Entity = &Manager->Entities[i];
        if (Entity->Flags & ENTITY_DIRTY)
        {
            Entity->Flags &= ~ENTITY_DIRTY;
            ivec2 Chunk = World_ToChunkPosition(Vec3_FloorToIVec3(Entity->Base.Position));
            Message_SetEntity(&Message, i, &Entity->Base);
            Server_SendChunkMessage(Server, Chunk, &Message);
        }
    }




    // handle new connections
    u32 NewClient = Network_ServerAcceptClient(&Server->Server);
    if (NewClient) Server_ClientConnect(Server, NewClient);
}
