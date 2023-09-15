#include "tcp.h"
#include "../encryption/encryption.h"

#include <iostream>
#include <fstream>
#include <cstdint>
#include <iterator>

constexpr int INVALID_SOCKET = -1;

bool TCPServer::Start( ) {
 
    // Create a socket
    ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ServerSocket == INVALID_SOCKET) {
        std::printf("Failed to create socket.\n");
        return false;
    }

    // Set up the address
    struct sockaddr_in ServerAddress;
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    ServerAddress.sin_port = htons(2182);

    // Bind the socket to the port
    int32_t Bind = bind(ServerSocket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress));
        
    if (Bind == INVALID_SOCKET) {
        std::printf("Failed to bind socket.\n");
        return false;
    }

    std::printf("Server listening on port %d.\n", 2182);

    // Listen for incoming connections
    if (listen(ServerSocket, 1) == INVALID_SOCKET) {
        std::printf("Failed to listen for connections.\n");
        return false;
    }

    std::printf("[ \033[0;32m!\033[0m ] Server started. Waiting for incoming connections\n");
    return true;
}

/*
Handle Incoming connections
Notes: Possibly hash the headers when being sent?
*/
void TCPServer::OnConnection() {
    
    sockaddr_in IncomingConnection;
    socklen_t AdressSize = sizeof(IncomingConnection);

    SOCKET IncomingSocket = accept(ServerSocket, (struct sockaddr*)&IncomingConnection, &AdressSize);

    if (IncomingSocket == INVALID_SOCKET)
        return;

    const char* IpAddress = inet_ntoa(IncomingConnection.sin_addr);
    std::printf("[ => ] %s connected.\n", IpAddress);
    
    std::thread thread([&] {

        TCPConnection Connection = TCPConnection(IncomingSocket, IpAddress);
        std::printf("[ => ] Handling connection from %s.\n", IpAddress);

        ByteArray Header = RecieveBytes( &Connection );

        // Define the headers here to make it easier to read.
        ByteArray ClientHeader { 0xAA, 0x5D, 0xD5, 0xC8 };

        if ( Header == ClientHeader ) {
            printf("[ => ] Client header recieved from %s.\n", IpAddress);
            HandleConnection( &Connection );
        }
        
    }); thread.detach();

}

ByteArray TCPServer::RecieveBytes(TCPConnection* Connection) {
    
    constexpr int BufferSize = 256;

    ByteArray	Recv;
	uint8_t		RecvBuffer[BufferSize];

    while(true)
	{
	    int32_t Received = recv(Connection->Socket, (char*)RecvBuffer, BufferSize, 0);

		// No more bytes
		if( !Received )
			break;

		// Emplace all bytes recieved in this cycle.
		for(int i = 0; i < Received; ++i)
		{
			Recv.push_back(RecvBuffer[i]);
		}

		// No more bytes left to receive.
		if (Received < BufferSize)
			break;
	}

	return Recv;
}

void TCPServer::SendBytes(ByteArray &Bytes, TCPConnection* Connection)
{
	// Send data.
	int32_t Response = send(Connection->Socket, (char *)Bytes.data(), (int)Bytes.size(), 0);
    
    if ( Response == INVALID_SOCKET )
        std::printf("[ E! ] Failed to send %zd bytes to %s.\n", Bytes.size(), Connection->IpAddress);

	std::printf("[ => ] Sending %zd bytes to %s.\n", Bytes.size(), Connection->IpAddress);
}

void TCPServer::SendEncryptedBytes(ByteArray &Raw, TCPConnection* Connection) {

    Encryption Enc;

    ByteArray EncryptedBytes = Enc.Encrypt(Raw, Connection->EncryptionKey);

    SendBytes(EncryptedBytes, Connection);
}

ByteArray TCPServer::RecieveEncryptedBytes(TCPConnection* Connection) {

    ByteArray Raw = RecieveBytes(Connection);

    Encryption Enc;
    ByteArray Decrypted = Enc.Decrypt(Raw, Connection->EncryptionKey);

    return Decrypted;
}

void TCPServer::SendFile(const char* FileName, TCPConnection* Connection, bool Encrypted) {

    std::ifstream File(FileName, std::ios::in | std::ios::binary);
    if (!File) 
        return;

    File.seekg(0, std::ios::end);
    std::streamsize FileSize = File.tellg();
    File.seekg(0, std::ios::beg);

    ByteArray RawFileData(FileSize);

    File.unsetf(std::ios::skipws);

    if (!File.read(reinterpret_cast<char*>(RawFileData.data()), FileSize))
        return;

    File.close();

    if (Encrypted)
        SendEncryptedBytes(RawFileData, Connection);
    else
        SendBytes(RawFileData, Connection);

}

void TCPServer::HandleConnection(TCPConnection* Connection) {
    
    ClientHandler Handler;

    /* Client IP is blacklisted, refuse connection */
    if (!Handler.IsClientOk(Connection->IpAddress)) {
        return;
    }

    /* Generate and send encryption key */
    Encryption Enc;

    ByteArray Key = Enc.GenerateKey();
    SendBytes(Key, Connection); 

    Connection->EncryptionKey = Key;

    ByteArray LoginHeader = RecieveEncryptedBytes( Connection );
    if ( LoginHeader.size( ) < sizeof( Login ) ) { 
        std::printf("[ => ] Failed to recieve login header from %s.\n", Connection->IpAddress);
        return;
    }

    Login LoginInfo = *( Login* )LoginHeader.data( );

    std::printf("[ => ] Recieved login attempt from %s.\n", LoginInfo.User);

    ResponseToClient RespStruct = Handler.HandleLoginInformation(&LoginInfo);
 
    ByteArray ToSend;
    ToSend.insert( ToSend.begin( ), 
        ( uint8_t* ) &RespStruct, 
        ( uint8_t* ) &RespStruct + sizeof( RespStruct ) 
    );

    SendEncryptedBytes( ToSend, Connection );

    SendFile("tosend.txt", Connection, false);
    
    CloseClient(Connection);
}