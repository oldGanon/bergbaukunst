
#include <winsock2.h>
#include <ws2tcpip.h>

#define SERVER_MAX_CLIENTS 16
#define NETWORK_SERVER_SOCKET_COUNT (SERVER_MAX_CLIENTS + 1)

typedef struct network_server
{
    SOCKET Sockets[NETWORK_SERVER_SOCKET_COUNT];
} network_server;

void Network_ServerSendMessage(network_server *Server, u32 ClientId, msg *Message);
bool Network_ServerGetMessage(network_server *Server, u32 ClientId, msg *Message);
u32 Network_ServerAcceptClient(network_server *Server);
void Network_ServerShutdown(network_server *Server);
bool Network_ServerInit(network_server *Server, const char *Port);

typedef struct network_client
{
    SOCKET Server;
} network_client;

void Network_ClientSendMessage(network_client *Client, msg *Message);
bool Network_ClientGetMessage(network_client *Client, msg *Message);
void Network_ClientDisconnect(network_client *Client);
bool Network_ClientInit(network_client *Client, const char *Domain, const char *Port);

/******************/
/* IMPLEMENTATION */
/******************/

inline void Network_DisconnectMessage(int Error, msg *Message)
{
    switch (Error)
    {
        case 0:               Message_Disconnect(Message, 0); /* connection closed */ break;
        case WSAECONNABORTED: Message_Disconnect(Message, 0); break;
        case WSAECONNRESET:   Message_Disconnect(Message, 0); break;
        case WSAENETDOWN:     Message_Disconnect(Message, 0); break;
        case WSAENETRESET:    Message_Disconnect(Message, 0); break;
        case WSAETIMEDOUT:    Message_Disconnect(Message, 0); break;
        default:              Message_Disconnect(Message, 0); break;
    }
}

inline bool Network_GetMessage(SOCKET Socket, msg *Message)
{
    DWORD Flags = MSG_PEEK;
    DWORD BytesReceived = 0;
    WSABUF Buffer = { sizeof(msg_header), (CHAR *)&Message->Header };
    int Error = WSARecv(Socket, &Buffer, 1, &BytesReceived, &Flags, 0, 0);
    if (Error == SOCKET_ERROR)
    {
        Error = WSAGetLastError();
        if (Error == WSAEWOULDBLOCK)
            return false;

        Network_DisconnectMessage(Error, Message);
        return true;
    }

    if (BytesReceived == 0)
    {
        Network_DisconnectMessage(Error, Message);
        return true;
    }

    if (BytesReceived < sizeof(msg_header))
        return false;

    if (Message->Header.Size > sizeof(msg))
    {
        Network_DisconnectMessage(Error, Message);
        return true;
    }

    Flags = 0;
    BytesReceived = 0;
    Buffer = (WSABUF){ Message->Header.Size, (CHAR *)Message };
    Error = WSARecv(Socket, &Buffer, 1, &BytesReceived, &Flags, 0, 0);
    if (Error == SOCKET_ERROR)
    {
        Error = WSAGetLastError();
        if (Error == WSAEWOULDBLOCK)
            return false;

        Network_DisconnectMessage(Error, Message);
        return true;
    }

    return true;
}

inline void Network_SendMessage(SOCKET Socket, msg *Message)
{
    DWORD Flags = 0;
    DWORD BytesSent = 0;
    WSABUF Buffer = (WSABUF){ Message->Header.Size, (CHAR *)Message };
    for (;;)
    {
        int Error = WSASend(Socket, &Buffer, 1, &BytesSent, Flags, 0, 0);
        if (Error != SOCKET_ERROR) break;
        Error = WSAGetLastError();
        if (Error != WSAEWOULDBLOCK) break;
    }
}



inline bool Network_ServerClientConnected(network_server *Server, u32 ClientId)
{
    if ((ClientId == 0) || (ClientId > SERVER_MAX_CLIENTS))
        return false;

    return (Server->Sockets[ClientId] != INVALID_SOCKET);
}

void Network_ServerBroadcastMessage(network_server *Server, msg *Message)
{
    for (u32 i = 1; i <= SERVER_MAX_CLIENTS; ++i)
        if (Network_ServerClientConnected(Server, i))
            Network_SendMessage(Server->Sockets[i], Message);
}

void Network_ServerSendMessage(network_server *Server, u32 ClientId, msg *Message)
{
    if (Network_ServerClientConnected(Server, ClientId))
        Network_SendMessage(Server->Sockets[ClientId], Message);
}

bool Network_ServerGetMessage(network_server *Server, u32 ClientId, msg *Message)
{
    if (!Network_ServerClientConnected(Server, ClientId))
        return false;

    if(!Network_GetMessage(Server->Sockets[ClientId], Message))
        return false;

    if (Message->Header.Type == MSG_DISCONNECT)
    {
        closesocket(Server->Sockets[ClientId]);
        Server->Sockets[ClientId] = INVALID_SOCKET;
    }

    return true;
}

u32 Network_ServerFreeClientSlot(network_server *Server)
{
    for (u32 i = 1; i < SERVER_MAX_CLIENTS; ++i)
        if (Server->Sockets[i] == INVALID_SOCKET)
            return i;
    return 0;
}

u32 Network_ServerAcceptClient(network_server *Server)
{
    u32 FreeClientId = Network_ServerFreeClientSlot(Server);
    if (!FreeClientId) return 0;

    struct sockaddr ClientAddr;
    int ClientAddrSize = sizeof(ClientAddr);
    Server->Sockets[FreeClientId] = WSAAccept(Server->Sockets[0], &ClientAddr, &ClientAddrSize, 0, 0);
    if (Server->Sockets[FreeClientId] == INVALID_SOCKET)
        return 0;

    u_long NotBlocking = 1;
    int Error = ioctlsocket(Server->Sockets[FreeClientId], FIONBIO, &NotBlocking);
    if (Error == SOCKET_ERROR)
    {
        closesocket(Server->Sockets[FreeClientId]);
        Server->Sockets[FreeClientId] = INVALID_SOCKET;
        return 0;
    }

    return FreeClientId;
}

void Network_ServerShutdown(network_server *Server)
{
    for (u32 i = 0; i < NETWORK_SERVER_SOCKET_COUNT; ++i)
    {
        if (Server->Sockets[i] != INVALID_SOCKET)
        {
            shutdown(Server->Sockets[i], SD_BOTH);
            closesocket(Server->Sockets[i]);
            Server->Sockets[i] = INVALID_SOCKET;
        }
    }
}

bool Network_ServerInit(network_server *Server, const char *Port)
{
    for (u32 i = 0; i < NETWORK_SERVER_SOCKET_COUNT; ++i)
        Server->Sockets[i] = INVALID_SOCKET;

    struct addrinfo *Addr = 0;
    struct addrinfo Hints = { 0 };
    Hints.ai_flags = AI_PASSIVE;
    Hints.ai_family = AF_INET;
    Hints.ai_socktype = SOCK_STREAM;
    Hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(0, Port, &Hints, &Addr))
        return false;

    for (struct addrinfo *AddrPtr = Addr; AddrPtr != NULL; AddrPtr = AddrPtr->ai_next)
    {
        Server->Sockets[0] = WSASocketW(AddrPtr->ai_family, AddrPtr->ai_socktype, AddrPtr->ai_protocol, 0, 0, WSA_FLAG_OVERLAPPED);
        if (Server->Sockets[0] == INVALID_SOCKET) continue;

        int Error = bind(Server->Sockets[0], AddrPtr->ai_addr, (int)AddrPtr->ai_addrlen);
        if (Error != SOCKET_ERROR)
        {
            Error = listen(Server->Sockets[0], SERVER_MAX_CLIENTS);
            if (Error != SOCKET_ERROR)
            {
                u_long NotBlocking = 1;
                Error = ioctlsocket(Server->Sockets[0], FIONBIO, &NotBlocking);
                if (Error != SOCKET_ERROR)
                {
                    break;
                }
            }
        }

        closesocket(Server->Sockets[0]);
        Server->Sockets[0] = INVALID_SOCKET;
    }
    freeaddrinfo(Addr);

    if (Server->Sockets[0] == INVALID_SOCKET)
        return false;
    return true;
}



void Network_ClientSendMessage(network_client *Client, msg *Message)
{
    Network_SendMessage(Client->Server, Message);
}

bool Network_ClientGetMessage(network_client *Client, msg *Message)
{
    if(!Network_GetMessage(Client->Server, Message))
        return false;

    if (Message->Header.Type == MSG_DISCONNECT)
        Network_ClientDisconnect(Client);

    return true;
}

void Network_ClientDisconnect(network_client *Client)
{
    shutdown(Client->Server, SD_BOTH);
    closesocket(Client->Server);
    Client->Server = INVALID_SOCKET;
}

bool Network_ClientInit(network_client *Client, const char *Domain, const char *Port)
{
    Client->Server = INVALID_SOCKET;

    struct addrinfo *Addr = 0;
    struct addrinfo Hints = { 0 };
    Hints.ai_flags = AI_PASSIVE;
    Hints.ai_family = AF_INET;
    Hints.ai_socktype = SOCK_STREAM;
    Hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(Domain, Port, &Hints, &Addr))
        return false;

    for (struct addrinfo *AddrPtr = Addr; AddrPtr != NULL; AddrPtr = AddrPtr->ai_next)
    {
        Client->Server = WSASocketW(AddrPtr->ai_family, AddrPtr->ai_socktype, AddrPtr->ai_protocol, 0, 0, WSA_FLAG_OVERLAPPED);
        if (Client->Server == INVALID_SOCKET) continue;

        int Error = WSAConnect(Client->Server, AddrPtr->ai_addr, (int)AddrPtr->ai_addrlen, 0, 0, 0, 0);
        if (Error != SOCKET_ERROR)
        {
            u_long NotBlocking = 1;
            Error = ioctlsocket(Client->Server, FIONBIO, &NotBlocking);
            if (Error != SOCKET_ERROR)
            {
                break;
            }
        }

        closesocket(Client->Server);
        Client->Server = INVALID_SOCKET;
    }
    freeaddrinfo(Addr);

    if (Client->Server == INVALID_SOCKET)
        return false;
    return true;
}

#if 0

typedef struct network_address
{
    int Size;
    struct sockaddr Address;
} network_address;

bool Network_SameAdresse(network_address *A, network_address *B)
{
    if (A->Size != B->Size)
        return false;

    if (A->Address.sa_family != B->Address.sa_family)
        return false;

    if (A->Address.sa_family != AF_INET)
        return false; // unsupported address family

    struct sockaddr_in *Ain = (struct sockaddr_in *)&A->Address;
    struct sockaddr_in *Bin = (struct sockaddr_in *)&B->Address;
    if (Ain->sin_addr.s_addr != Bin->sin_addr.s_addr) return false;
    if (Ain->sin_port != Bin->sin_port) return false;

    return true;
}

typedef struct network_socket
{
    SOCKET Socket;
    network_address Address;

    network_address RecvAddress;
    WSAOVERLAPPED RecvOverlapped;
    msg RecvMessage;
} network_socket;

bool Network_GetMessage(network_socket *Socket, msg *Message, network_address *Address)
{
    DWORD Flags;
    DWORD BytesReceived;
    if (!WSAGetOverlappedResult(Socket->Socket, &Socket->RecvOverlapped, &BytesReceived, FALSE, &Flags))
        return false;

    memcpy(Message, &Socket->RecvMessage, BytesReceived);
    memcpy(Address, &Socket->RecvAddress, sizeof(network_address));

    Flags = 0;
    WSABUF WSABuffer = { sizeof(msg), (CHAR *)&Socket->RecvMessage };
    Socket->RecvAddress.Size = sizeof(struct sockaddr);
    int Error = WSARecvFrom(Socket->Socket, &WSABuffer, 1, 0, &Flags, 
                           &Socket->RecvAddress.Address, 
                           &Socket->RecvAddress.Size,
                           &Socket->RecvOverlapped, 0);
    assert(Error != SOCKET_ERROR || (WSAGetLastError() == WSA_IO_PENDING));
    return true;
}

bool Network_SendMessage(network_socket *Socket, msg *Message, network_address *Address)
{
    WSABUF WSABuffer = { sizeof(msg), (CHAR *)Message };
    int Error = WSASendTo(Socket->Socket, &WSABuffer, 1, 0, 0, 
                          &Address->Address, Address->Size, 0, 0);
    if (Error == SOCKET_ERROR) return false;
    return true;
}

bool Network_Socket(network_socket *Socket, const char *Port)
{
    struct addrinfo *Addr = 0;
    struct addrinfo Hints = { 0 };
    Hints.ai_flags = AI_PASSIVE;
    Hints.ai_family = AF_INET;
    Hints.ai_socktype = SOCK_DGRAM;
    Hints.ai_protocol = IPPROTO_UDP;

    if (!getaddrinfo("", Port, &Hints, &Addr))
    {
        Socket->Socket = WSASocketW(Addr->ai_family, Addr->ai_socktype, Addr->ai_protocol, 0, 0, WSA_FLAG_OVERLAPPED);
        Socket->Address = (network_address) {
            .Address = *Addr->ai_addr,
            .Size = (int)Addr->ai_addrlen,
        };
        freeaddrinfo(Addr);

        if (Socket->Socket != INVALID_SOCKET)
        {
            int Error = bind(Socket->Socket, &Socket->Address.Address, Socket->Address.Size);
            if (Error != SOCKET_ERROR)
            {
                WSABUF WSABuffer = { sizeof(msg), (CHAR *)&Socket->RecvMessage };
                DWORD Flags = 0;
                Socket->RecvOverlapped.hEvent = WSACreateEvent();
                Socket->RecvAddress.Size = sizeof(struct sockaddr);
                Error = WSARecvFrom(Socket->Socket, &WSABuffer, 1, 0, &Flags, 
                                    &Socket->RecvAddress.Address, 
                                    &Socket->RecvAddress.Size,
                                    &Socket->RecvOverlapped, 0);
                if (Error && (Error = WSAGetLastError()) == WSA_IO_PENDING)
                    return true;
            }
            closesocket(Socket->Socket);
            Socket->Socket = INVALID_SOCKET;
        }
    }

    return false;
}

#endif

bool Network_Init(void)
{
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2,2), &WSAData))
        return false;
    return true;
}

void Network_Destroy(void)
{
    WSACleanup();
}
