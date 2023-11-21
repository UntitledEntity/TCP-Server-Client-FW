#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>

#include "structures.h"
#include "../aes.h"

// std::function
#include <functional>

// std::min
#include <algorithm>

// std::thread
#include <thread>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "kernel32.lib")

using ByteArray = std::vector<uint8_t>;

#define NO_EXPECTED_SIZE -1

class TCPClient {
    SOCKET Socket;
    Encryption Encryption;

public:

    enum ResponseCode : int
    {
        INVALID_LOGIN = 0x297,
        SUCCESS = 0x9832,
        USER_BANNED = 0x929,
        EXPIRED_SUBSCRIPTION = 0x412
    };

    bool Start( );
    void Login( );

    ByteArray RecieveBytes( int ExpectedSize = NO_EXPECTED_SIZE );
    void SendBytes( ByteArray& Bytes );
    void SendBytes( char* Bytes );

    ByteArray RecieveEncryptedBytes( int ExpectedSize = NO_EXPECTED_SIZE ) { ByteArray Raw = RecieveBytes( ExpectedSize ); return Encryption.Decrypt( Raw ); }
    void SendEncryptedBytes( ByteArray& Bytes ) { ByteArray EncryptedBytes = Encryption.Encrypt( Bytes ); SendBytes( EncryptedBytes ); }

    void SendHashedBytes( ByteArray& Bytes );

    void RecieveFile( const char* Out, bool Encrypted = true );

    void Close( ) {
        closesocket( Socket );
        WSACleanup( );
    };

    SOCKET GetSocket( ) { return Socket; }
};