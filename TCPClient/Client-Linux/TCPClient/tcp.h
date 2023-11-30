#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <vector>
#include <cstring>
#include <cstdint>

#include "structures.h"
#include "../encryption.h"

// std::function
#include <functional>

// std::min
#include <algorithm>

// std::thread
#include <thread>

using SOCKET = int;
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1

#define NO_EXPECTED_SIZE -1

class TCPClient {
    SOCKET Socket;
    Encryption Enc;

public:

    enum ResponseCode : int
    {
        INVALID_LOGIN = 0x297,
        SUCCESS = 0x9832,
        USER_BANNED = 0x929,
        EXPIRED_SUBSCRIPTION = 0x412
    };

    bool Start( const char* server_host, int server_port );
    void Login( );

    ByteArray RecieveBytes( int ExpectedSize = NO_EXPECTED_SIZE );
    void SendBytes( ByteArray& Bytes );
    void SendBytes( char* Bytes );

    ByteArray RecieveEncryptedBytes( int ExpectedSize = NO_EXPECTED_SIZE ) { ByteArray Raw = RecieveBytes( ExpectedSize ); return Enc.Decrypt( Raw ); }
    void SendEncryptedBytes( ByteArray& Bytes ) { ByteArray EncryptedBytes = Enc.Encrypt( Bytes ); SendBytes( EncryptedBytes ); }

    void SendHashedBytes( ByteArray& Bytes );

    void RecieveFile( const char* Out, bool Encrypted = true );

    void Close( ) {
        close( this->Socket );
    };

    SOCKET GetSocket( ) { return Socket; }
};