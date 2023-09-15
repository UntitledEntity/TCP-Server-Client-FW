#include "networking/tcp.h"

// std::atexit
#include <cstdlib>

static SOCKET ServerSocket = -1;

int main() {

    TCPServer Server;

    bool Resp = Server.Start( );

    if (!Resp)
        return 1;
        
    ServerSocket = Server.GetSocket();

    while (true) {
        Server.OnConnection();
    }
    
    Server.Close();
    return 0;
}
 