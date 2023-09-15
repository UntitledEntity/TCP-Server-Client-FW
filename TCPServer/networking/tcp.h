#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <cstdio>
#include "structures.h"

// std::function
#include <functional>

// std::min
#include <algorithm>

// std::thread
#include <thread>

#include <cstring>

using ByteArray = std::vector<uint8_t>;

class TCPServer {
    SOCKET ServerSocket;

public:
    bool Start();

    void OnConnection( );
    void HandleConnection( TCPConnection* Connection );

    ByteArray RecieveBytes( TCPConnection* Connection );
    void SendBytes(ByteArray &Bytes, TCPConnection* Connection);

    ByteArray RecieveEncryptedBytes( TCPConnection* Connection );
    void SendEncryptedBytes(ByteArray &Bytes, TCPConnection* Connection);

    void SendFile( const char* FileName, TCPConnection* Connection, bool Encrypted = true);

    void Close() {

        close(ServerSocket);

    };

    SOCKET GetSocket() {
        return ServerSocket;
    }

    void CloseClient(TCPConnection* Conn) {

        if (Conn->Socket)
            close(Conn->Socket);

        std::printf("[ => ] Closing connection on client %s.\n", Conn->IpAddress);

    };
};

class ClientHandler {
public:

    bool IsClientOk( const char* IpAdress );
    bool CheckClientCredentials( Login* Creds );
    ResponseToClient HandleLoginInformation( Login* L );
    
};
