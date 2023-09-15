#include "tcp.h"

// For now, these functions are just here to support the framework but ideally you could implement a JSON-based system or just add MySQL

bool ClientHandler::IsClientOk( const char* IpAdress ) {

    /* Check for IP blacklist */
    return true;

}

bool ClientHandler::CheckClientCredentials( Login* Creds ) {

    if ( !strstr( Creds->User, "Test" )|| !strstr( Creds->Pass, "Test" ) )
        return false;

    return true;

}

ResponseToClient ClientHandler::HandleLoginInformation( Login* LoginInfo ) {

    bool CorrectLogin = CheckClientCredentials(LoginInfo);

    ResponseToClient Ret;

    if (!CorrectLogin) {
        std::printf("[ => ] Failed login attempt from %s.\n", LoginInfo->User);

        strcpy( Ret.User, LoginInfo->User );
        Ret.Success = false;
        
        return Ret;
    }

    std::printf("[ => ] Successfull login attempt from %s.\n", LoginInfo->User);

    strcpy( Ret.User, LoginInfo->User );
    Ret.Success = true;
    Ret.Level = 2;
        
    return Ret;

}