
enum msg_type
{
    /* UNIVERSAL MESSAGES */
    MSG_DISCONNECT,
    MSG_PLAYER_POSITION,
    MSG_VIEW_POSITION,
    
    /* SERVER MESSAGES */
    MSG_CHUNK_DATA,
    MSG_SET_BLOCK,
    
    /* CLIENT MESSAGES */
    MSG_PLACE_BLOCK,
    MSG_BREAK_BLOCK,
};

typedef struct msg_header
{
    u32 Type;
    u32 Size;
} msg_header;

/* UNIVERSAL MESSAGES */

typedef struct msg_disconnect
{
    u8 Type;
} msg_disconnect;

typedef struct msg_player_position
{
    vec3 Position;
} msg_player_position;

typedef struct msg_view_position
{
    ivec2 Position;
} msg_view_position;

/* SERVER MESSAGES */

typedef struct msg_chunk_data
{
    ivec2 Position;
    u8 Blocks[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_WIDTH];
} msg_chunk_data;

typedef struct msg_set_block
{
    ivec3 Position;
    block Block;
} msg_set_block;

/* CLIENT MESSAGES */

typedef struct msg_place_block
{
    ivec3 Position;
    u32 BlockFace;
} msg_place_block;

typedef struct msg_break_block
{
    ivec3 Position;
    u32 BlockFace;
} msg_break_block;

/* */

typedef struct msg
{
    msg_header Header;

    union
    {
        msg_disconnect Disconnect;
        msg_player_position PlayerPosition;
        msg_view_position ViewPosition;
        
        msg_chunk_data ChunkData;
        msg_set_block SetBlock;

        msg_place_block PlaceBlock;
        msg_break_block BreakBlock;
    };
} msg;

u64 Message_GetSize(msg *Message)
{
    switch (Message->Header.Type)
    {
        case MSG_DISCONNECT:      return sizeof(msg_header) + sizeof(msg_disconnect);
        case MSG_PLACE_BLOCK:     return sizeof(msg_header) + sizeof(msg_place_block);
        case MSG_BREAK_BLOCK:     return sizeof(msg_header) + sizeof(msg_break_block);
        case MSG_PLAYER_POSITION: return sizeof(msg_header) + sizeof(msg_player_position);
        case MSG_VIEW_POSITION:   return sizeof(msg_header) + sizeof(msg_view_position);
        case MSG_CHUNK_DATA:      return sizeof(msg_header) + sizeof(msg_chunk_data);
        default: return 0;
    }
}

/* UNIVERSAL MESSAGES */

void Message_Disconnect(msg *Message, u8 Type)
{
    Message->Header.Type = MSG_DISCONNECT;
    Message->Header.Size = sizeof(msg_header) + sizeof(msg_disconnect);
    
    Message->Disconnect.Type = Type;
}

void Message_PlayerPosition(msg *Message, vec3 PlayerPosition)
{
    Message->Header.Type = MSG_PLAYER_POSITION;
    Message->Header.Size = sizeof(msg_header) + sizeof(msg_player_position);

    Message->PlayerPosition.Position = PlayerPosition;
}

void Message_ViewPosition(msg *Message, ivec2 ViewPosition)
{
    Message->Header.Type = MSG_VIEW_POSITION;
    Message->Header.Size = sizeof(msg_header) + sizeof(msg_view_position);

    Message->ViewPosition.Position = ViewPosition;
}

/* SERVER MESSAGES */

void Message_ChunkData(msg *Message, chunk *Chunk)
{
    Message->Header.Type = MSG_CHUNK_DATA;
    Message->Header.Size = sizeof(msg_header) + sizeof(msg_chunk_data);

    Message->ChunkData.Position = Chunk->Position;

    for (u32 z = 0; z < CHUNK_HEIGHT; ++z)
    for (u32 y = 0; y < CHUNK_WIDTH; ++y)
    for (u32 x = 0; x < CHUNK_WIDTH; ++x)
    {
        Message->ChunkData.Blocks[z][y][x] = Chunk->Blocks[z][y][x].Id;
    }
}

void Message_SetBlock(msg *Message, ivec3 Position, block Block)
{
    Message->Header.Type = MSG_SET_BLOCK;
    Message->Header.Size = sizeof(msg_header) + sizeof(msg_set_block);
    
    Message->SetBlock.Position = Position;
    Message->SetBlock.Block = Block;
}

/* CLIENT MESSAGES */

void Message_PlaceBlock(msg *Message, ivec3 Position, u32 BlockFace)
{
    Message->Header.Type = MSG_PLACE_BLOCK;
    Message->Header.Size = sizeof(msg_header) + sizeof(msg_place_block);
    
    Message->PlaceBlock.Position = Position;
    Message->PlaceBlock.BlockFace = BlockFace;
}

void Message_BreakBlock(msg *Message, ivec3 Position)
{
    Message->Header.Type = MSG_BREAK_BLOCK;
    Message->Header.Size = sizeof(msg_header) + sizeof(msg_break_block);

    Message->BreakBlock.Position = Position;
}
