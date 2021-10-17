#include "world.c"
typedef struct server_client
{
    u32 EntityId;

    // ViewPosition;
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

void Server_SendChunkMessage(server *Server, ivec2 ChunkPosition, msg *Message)
{
    for (u32 i = 1; i < SERVER_MAX_CLIENTS; ++i)
    {
        Network_ServerSendMessage(&Server->Server, i, Message);
    }
}

void Server_ClientConnect(server *Server, u32 ClientId)
{
    server_client *Client = Server_GetClient(Server, ClientId);
    if (!Client) return;

    vec3 PlayerSpawnPosition = (vec3){ 0 };
    ivec2 SpawnChunk = World_ToChunkPosition(Vec3_FloorToIVec3(PlayerSpawnPosition));
    Client->EntityId = World_SpawnPlayer(&Server->World, PlayerSpawnPosition);
    entity *Player = EntityManager_GetEntity(&Server->World.EntityManager, Client->EntityId);

    msg Message;

    // set client player position
    Message_PlayerState(&Message, PlayerSpawnPosition);
    Network_ServerSendMessage(&Server->Server, ClientId, &Message);

    // set client view position
    Message_ViewPosition(&Message, SpawnChunk);
    Network_ServerSendMessage(&Server->Server, ClientId, &Message);

    // send chunk data
    ivec2 LoadedChunkDist = iVec2_Set1(LOADED_CHUNKS_DIST);
    ivec2 MinPos = iVec2_Sub(SpawnChunk, LoadedChunkDist);
    ivec2 MaxPos = iVec2_Add(SpawnChunk, LoadedChunkDist);
    for (i32 x = MinPos.x; x < MaxPos.x; x++)
    for (i32 y = MinPos.y; y < MaxPos.y; y++)
    {
        ivec2 ChunkPos = (ivec2){ x, y };
        chunk *Chunk = World_GetChunk(&Server->World, ChunkPos);
        Message_ChunkData(&Message, Chunk);
        Network_ServerSendMessage(&Server->Server, ClientId, &Message);
    }

    // send entity data
    entity_manager *Manager = &Server->World.EntityManager;
    for (u32 i = 1; i <= Manager->Capacity; ++i)
    {
        entity *Entity = &Manager->Entities[i];
        if (Entity->Type == ENTITY_NONE)
            continue;

        ivec2 Chunk = World_ToChunkPosition(Vec3_FloorToIVec3(Entity->Position));
        Message_SetEntity(&Message, i, Entity);
        Server_SendChunkMessage(Server, Chunk, &Message);
    }
}

void Server_SetBlock(server *Server, ivec3 Position, block Block)
{
    World_SetBlock(&Server->World, Position, Block);

    msg Message;
    Message_SetBlock(&Message, Position, Block);
    Server_SendChunkMessage(Server, World_ToChunkPosition(Position), &Message);
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

    ivec2 Chunk = World_ToChunkPosition(Vec3_FloorToIVec3(Player->Position));
    Player->Position = PlayerState->Position;

    msg Message;
    Message_SetEntity(&Message, Client->EntityId, Player);
    Server_SendChunkMessage(Server, Chunk, &Message);
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
        Server_SetBlock(Server, PlaceBlock->Position, Block);
}

void Server_ClientBreakBlock(server *Server, u32 ClientId, const msg_break_block *BreakBlock)
{
    server_client *Client = Server_GetClient(Server, ClientId);
    if (!Client) return;

    block Block = (block){ .Id = BLOCK_ID_AIR };
    Server_SetBlock(Server, BreakBlock->Position, Block);
}


void Update_Entities(server *Server)
{
    entity_manager *Manager = &Server->World.EntityManager;
    for (u32 i = 0; i <= Manager->Capacity; ++i)
    {
        entity *Mob = &Manager->Entities[i];
        if (Mob->Type == ENTITY_MOB)
        {
            for (u32 j = 1; j < SERVER_MAX_CLIENTS; ++j)
            {
                server_client *Client = Server_GetClient(Server, j);
                if (!Client) return;

                entity *Player = EntityManager_GetEntity(&Server->World.EntityManager, Client->EntityId);
                if (!Player) return;

                vec3 MobToPlayer = Vec3_Sub(Player->Position, Mob->Position);
                f32 DistanceSquared = MobToPlayer.x * MobToPlayer.x + MobToPlayer.y * MobToPlayer.y + MobToPlayer.z * MobToPlayer.z;

                if(DistanceSquared < 10*10)
                {
                    f32 NewYaw = Mob->Yaw += 0.1f;

                    msg Message;
                    ivec2 Chunk = World_ToChunkPosition(Vec3_FloorToIVec3(Mob->Position));
                    Message_SetEntity(&Message, i, Mob);
                    Server_SendChunkMessage(Server, Chunk, &Message);
                }
            }
        }
    }

}

void Server_Update(server *Server)
{
    u32 NewClient = Network_ServerAcceptClient(&Server->Server);
    if (NewClient) Server_ClientConnect(Server, NewClient);

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
                default: break;
            }
        }
        
    }

    World_Update(&Server->World, Vec3_Zero());
    Update_Entities(Server);
}
