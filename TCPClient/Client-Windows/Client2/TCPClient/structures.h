#include <vector>

struct LoginStruct {

    char User[ 255 ];
    char Pass[ 255 ];
    char HWID[ 255 ];
    char Pad[ 1 ];
};

struct ResponseToClient {

    char User[ 255 ];
    bool Success;
    int Level;
    char Pad[ 1 ];

};

struct FileInfo {
    int size;
    char name[ 255 ];
};

struct TCPConnection {

    TCPConnection( SOCKET C, const char* IP ) {
        Socket = C;
        IpAddress = IP;
    };

    SOCKET Socket;
    const char* IpAddress;
    std::vector<uint8_t> EncryptionKey;
};