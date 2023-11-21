#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

#include <stdexcept>
#include <vector>
#include <sstream>
#include <iomanip>

class Encryption {

    std::vector<uint8_t> GenerateRandomBytes( size_t size )
    {
        std::vector<uint8_t> randomBytes( size );
        RAND_bytes( randomBytes.data( ), static_cast< int >( size ) );
        return randomBytes;
    }

    std::vector<uint8_t> Key;
    bool InitCalled = false;
public:

    enum ErrorCode : uint8_t {
        InitNotCalled = 0x0
    };

    void Init( std::vector<uint8_t> K ) {
        Key = K;
        InitCalled = true;
    }

    std::vector<uint8_t> GenerateIV( )
    {
        const size_t IVSize = AES_BLOCK_SIZE;
        return GenerateRandomBytes( IVSize );
    }

    std::vector<uint8_t> GenerateKey( )
    {
        const size_t KeySize = 32;
        return GenerateRandomBytes( KeySize );
    }

    std::vector<uint8_t> Encrypt( std::vector<uint8_t>& input )
    {
        std::vector<uint8_t> encrypted;

        if ( !InitCalled ) {
            throw std::exception( "Encryption::Init has not been called." );
            return { ErrorCode::InitNotCalled };
        }

        AES_KEY aesKey;
        AES_set_encrypt_key( Key.data( ), 256, &aesKey );

        encrypted.resize( input.size( ) );

        for ( size_t i = 0; i < input.size( ); i += AES_BLOCK_SIZE ) {

            // IMPORTANT: Pad end so that we avoid writing to nullptr and crashing.
            if ( i + AES_BLOCK_SIZE > input.size( ) )
                encrypted.resize( input.size( ) + ( input.size( ) - i ) );

            AES_encrypt( input.data( ) + i, encrypted.data( ) + i, &aesKey );
        }

        return encrypted;
    }

    std::vector<uint8_t> Decrypt( std::vector<uint8_t>& encrypted )
    {
        std::vector<uint8_t> decrypted;

        if ( !InitCalled ) {
            throw std::exception( "Encryption::Init has not been called." );
            return { ErrorCode::InitNotCalled };
        }

        AES_KEY aesKey;
        AES_set_decrypt_key( Key.data( ), 256, &aesKey );

        decrypted.resize( encrypted.size( ) );

        for ( size_t i = 0; i < encrypted.size( ); i += AES_BLOCK_SIZE ) {

            // IMPORTANT: Pad end so that we avoid writing to nullptr and crashing.
            if ( i + AES_BLOCK_SIZE > encrypted.size( ) )
                decrypted.resize( encrypted.size( ) + ( encrypted.size( ) - i ) );

            AES_decrypt( encrypted.data( ) + i, decrypted.data( ) + i, &aesKey );
        }

        decrypted.resize( encrypted.size( ) );

        return decrypted;
    }

    std::vector<uint8_t> Hash( std::vector<uint8_t>& in )
    {
        std::vector<uint8_t> hash;

        SHA256_CTX sha256;
        SHA256_Init( &sha256 );
        SHA256_Update( &sha256, in.data( ), in.size( ) );
        SHA256_Final( hash.data( ), &sha256 );

        return hash;
    }
    
};