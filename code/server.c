
typedef struct server_client
{
    vec3 Position;
} server_client;

typedef struct server
{
    network_server Server;
    server_client Clients[SERVER_MAX_CLIENTS];
    world World;
} server;

void Server_Init(server *Server)
{
    Network_Server(&Server->Server, "4510");

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

    ivec2 CenterChunk = World_ToChunkPosition(Vec3_FloorToIVec3(Client->Position));
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

void Server_Update(server *Server)
{
    msg Message;

    u32 NewClient = Network_ServerAcceptClient(&Server->Server);
    if (NewClient)
    {
        Message_PlayerPosition(&Message, (vec3){ 0 });
        Network_ServerSendMessage(&Server->Server, NewClient, &Message);
        Message_ViewPosition(&Message, (ivec2){ 0 });
        Network_ServerSendMessage(&Server->Server, NewClient, &Message);

        Server_SendChunks(Server, NewClient, &Message);
    }

    while (Network_ServerGetMessage(&Server->Server, 1, &Message))
    {
        switch (Message.Header.Type)
        {
            case MSG_DISCONNECT: break;
            case MSG_PLACE_BLOCK: break;
            case MSG_BREAK_BLOCK: break;
            case MSG_PLAYER_POSITION: break;
        }
    }

    server_client *Client = Server_GetClient(Server, 1);
    if (Client) World_Update(&Server->World, Client->Position);
}
