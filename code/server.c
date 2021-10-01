
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

void Server_SendChunks(server *Server, u32 ClientId, msg *Message)
{
    server_client *Client = Server_GetClient(Server, ClientId);

    entity *Player = EntityManager_GetEntity(&Server->World.EntityManager, Client->EntityId);

    ivec2 CenterChunk = World_ToChunkPosition(Vec3_FloorToIVec3(Player->Position));
    ivec2 LoadedChunkDist = iVec2_Set1(LOADED_CHUNKS_DIST);
    ivec2 MinPos = iVec2_Sub(CenterChunk, LoadedChunkDist);
    ivec2 MaxPos = iVec2_Add(CenterChunk, LoadedChunkDist);

    for (i32 x = MinPos.x; x < MaxPos.x; x++)
    for (i32 y = MinPos.y; y < MaxPos.y; y++)
    {
        ivec2 ChunkPos = (ivec2){ x, y };
        chunk *Chunk = World_GetChunk(&Server->World, ChunkPos);
        Message_ChunkData(Message, Chunk);
        Network_ServerSendMessage(&Server->Server, ClientId, Message);
    }
}

void Server_ClientUpdatePosition(server *Server, u32 ClientId, const msg_player_position *PlayerPosition)
{
    server_client *Client = Server_GetClient(Server, ClientId);
    if (!Client) return;

    entity *Player = EntityManager_GetEntity(&Server->World.EntityManager, Client->EntityId);
    if (!Player) return;

    Player->Position = PlayerPosition->Position;
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
    {
        World_SetBlock(&Server->World, PlaceBlock->Position, Block);

        msg Message;
        Message_SetBlock(&Message, PlaceBlock->Position, Block);
        Network_ServerSendMessage(&Server->Server, ClientId, &Message);
    }

}

void Server_ClientBreakBlock(server *Server, u32 ClientId, const msg_break_block *BreakBlock)
{
    server_client *Client = Server_GetClient(Server, ClientId);
    if (!Client) return;

    block Block = (block){ .Id = BLOCK_ID_AIR };
    World_SetBlock(&Server->World, BreakBlock->Position, Block);

    msg Message;
    Message_SetBlock(&Message, BreakBlock->Position, Block);
    Network_ServerSendMessage(&Server->Server, ClientId, &Message);
}

void Server_Update(server *Server)
{
    msg Message;

    u32 NewClient = Network_ServerAcceptClient(&Server->Server);
    if (NewClient)
    {
        server_client *Client = Server_GetClient(Server, NewClient);

        vec3 PlayerSpawnPosition = (vec3){ 0 };
        Client->EntityId = World_SpawnPlayer(&Server->World, PlayerSpawnPosition);

        Message_PlayerPosition(&Message, PlayerSpawnPosition);
        Network_ServerSendMessage(&Server->Server, NewClient, &Message);

        ivec2 ViewPosition = World_ToChunkPosition(Vec3_FloorToIVec3(PlayerSpawnPosition));
        Message_ViewPosition(&Message, ViewPosition);
        Network_ServerSendMessage(&Server->Server, NewClient, &Message);

        Server_SendChunks(Server, NewClient, &Message);
    }

    for (u32 i = 1; i < 16; ++i)
    {
        while (Network_ServerGetMessage(&Server->Server, i, &Message))
        {
            switch (Message.Header.Type)
            {
                case MSG_DISCONNECT: break;
                case MSG_PLAYER_POSITION: Server_ClientUpdatePosition(Server, i, &Message.PlayerPosition); break;
                case MSG_PLACE_BLOCK:     Server_ClientPlaceBlock(Server, i, &Message.PlaceBlock); break;
                case MSG_BREAK_BLOCK:     Server_ClientBreakBlock(Server, i, &Message.BreakBlock); break;
                default: break;
            }
        }
    }

    World_Update(&Server->World, Vec3_Zero());
}
