#include <sys/socket.h>
#include <vector>

using SOCKET = int;

struct Login {

    char User[ 255 ];
    char Pass[ 255 ];
    char HWID[ 255 ];
    char Pad[ 1 ];
    
};

/* If you don't pad it, the last variable does not write properly? */
struct ResponseToClient {

    char User[ 255 ];
    bool Success;
    int Level;
    char Pad[ 1 ];

};

struct FileInfo { 
    int size;
    char name[255];
};

struct TCPConnection {

    TCPConnection( SOCKET C, const char* IP ) {
        Socket = C;
        IpAddress = IP;
    };

    SOCKET Socket;
    const char* IpAddress;
    std::vector<unsigned char> EncryptionKey;
};