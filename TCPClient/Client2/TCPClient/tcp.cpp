#include "tcp.h"

#include <fstream>
#include <iostream>

bool TCPClient::Start( )
{
    // Initialize Winsock
    WSADATA wsaData;
    if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 ) {
        printf("[ E! ] Failed to initialize Winsock.\n");
        return false;
    }

    // Define the server host and port
    const char* server_host = "15.204.223.99";
    int server_port = 2182;

    // Create a socket
    Socket = socket( AF_INET, SOCK_STREAM, 0 );
    if ( Socket == INVALID_SOCKET ) {
        printf( "[ E! ] Failed to create socket.\n" );
        WSACleanup( );
        return false;
    }

    // Set up the server address
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons( server_port );

    if ( inet_pton( AF_INET, server_host, &( server_address.sin_addr ) ) != 1 ) {
        printf( "[ E! ] Server is down." );
        closesocket( Socket );
        WSACleanup( );
        return false;
    }

    // Connect to the server
    if ( connect( Socket, reinterpret_cast< sockaddr* >( &server_address ), sizeof( server_address ) ) == SOCKET_ERROR ) {

        int32_t ErrorCode = WSAGetLastError( );
        if ( ErrorCode != 10061 )
        {
            printf( "[ E! ] Failed to connect to the server. Error: %i\n", WSAGetLastError( ) );
            closesocket( Socket );
            WSACleanup( );
        }
        else 
            printf( "[ E! ] Server is down.\n");
            
        return false;
    }

    printf( "[ => ] Successfully connected to server.\n" );
    
    ByteArray Header { 0xAA, 0x5D, 0xD5, 0xC8 };
    SendBytes( Header );

    // Recieve encryption key
    ByteArray EncKey = RecieveBytes( );
    printf( "[ => ] Recieved Encryption Key\n" );

    Encryption.Init( EncKey );
    
    return true;
}

void TCPClient::Login( ) {

    LoginStruct Login;

    strncpy_s( Login.User, "Test", sizeof( Login.User ) );
    strncpy_s( Login.Pass, "Test", sizeof( Login.Pass ) );
    strncpy_s( Login.HWID, "Test", sizeof( Login.HWID ) );
    
    ByteArray ToSend;

    ToSend.insert( ToSend.begin( ), 
        ( uint8_t* ) &Login, 
        ( uint8_t* ) &Login + sizeof( LoginStruct ) 
    );

    SendEncryptedBytes( ToSend );

    ByteArray Raw = RecieveEncryptedBytes( );
    ResponseToClient Response = *( ResponseToClient* ) Raw.data( );

    if ( !Response.Success )
        return;

    printf( "[ => ] Successfully logged in.\n" );

    RecieveFile( "recv.txt", false );

    Close( );
}

void TCPClient::RecieveFile( const char* Out, bool Encrypted ) {

    std::ofstream Output( Out, std::ios::out );

    if ( !Output.is_open( ) ) {
        printf( "[ E! ] Failed to recieve file.\n" );
        return;
    }

    ByteArray Data = Encrypted ? RecieveEncryptedBytes( ) : RecieveBytes( );

    printf( "[ => ] Recieving file  (%zu bytes).\n", Data.size( ) );

    printf( "[ => ] Writing data to file\n" );
    Output.write( ( const char* ) Data.data( ), Data.size( ) );

    Output.close( );
}

ByteArray TCPClient::RecieveBytes( int ExpectedSize ) {

    constexpr int BufferSize = 256;

    ByteArray	Recv;
    uint8_t		RecvBuffer[ BufferSize ];

    while ( true )
    {

        int32_t Received = recv( Socket, ( char* ) RecvBuffer, BufferSize, 0 );

        if ( Received < 1 ) {
            break;
        }

        for ( int i = 0; i < Received; ++i ) {
            Recv.push_back( RecvBuffer[ i ] );
        }

        // This prevents not recieving all the data in the bytes.
        if ( ExpectedSize == NO_EXPECTED_SIZE && Received < BufferSize )
            break;
        else if ( Recv.size( ) >= ExpectedSize )
            break;
    }

    if ( ExpectedSize != NO_EXPECTED_SIZE )
        Recv.resize( ExpectedSize );

    return Recv;
}

void TCPClient::SendBytes( ByteArray& Bytes )
{
    int32_t Response = send( Socket, ( char* ) Bytes.data( ), ( int ) Bytes.size( ), 0 );
    if ( Response == INVALID_SOCKET )
        printf( "[ E! ] Failed to send %zd bytes to server.\n", Bytes.size( ) );

    printf( "[ => ] Sent %zd bytes to server.\n", Bytes.size( ) );
}

void TCPClient::SendBytes( char* Bytes )
{
    // Send data.
    int32_t Response = send( Socket, Bytes, sizeof( Bytes ), 0 );
    if ( Response == INVALID_SOCKET )
        printf( "[ E! ] Failed to send %zd bytes to server.\n", sizeof( Bytes ) );

    printf( "[ => ] Sending %zd bytes to server.\n", sizeof( Bytes ) );
}


