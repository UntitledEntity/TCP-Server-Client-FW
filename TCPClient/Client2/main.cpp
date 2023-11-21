#include "TCPClient\tcp.h"

int main( ) {

    TCPClient TCP;
    
    if ( !TCP.Start( ) )
        return 1;

    TCP.Login( );
    
    printf( "[ => ] Finished TCP connection. Pausing application." );
    Sleep( -1 );

    return 0;
}