#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

class Encryption {

    std::vector<uint8_t> GenerateRandomBytes(size_t size)
    {
        std::vector<uint8_t> randomBytes(size);
        RAND_bytes(randomBytes.data(), static_cast<int>(size));
        return randomBytes;
    }

public:

    std::vector<uint8_t> GenerateIV()
    {
        // IV size for AES CBC mode is the same as the block size
        const size_t IVSize = AES_BLOCK_SIZE;
        return GenerateRandomBytes(IVSize);
    }

    std::vector<uint8_t> GenerateKey()
    {
        // Key size for AES-256 is 256 bits (32 bytes)
        const size_t KeySize = 32;
        return GenerateRandomBytes(KeySize);
    }
            
    std::vector<uint8_t> Encrypt(std::vector<uint8_t>& input, std::vector<uint8_t>& key)
    {
        std::vector<uint8_t> encrypted;

        AES_KEY aesKey;
        AES_set_encrypt_key(key.data(), 256, &aesKey);

        // Set up output buffer
        encrypted.resize(input.size());

        for ( size_t i = 0; i < input.size( ); i += AES_BLOCK_SIZE ) {

            // IMPORTANT: Pad end so that we avoid writing to nullptr and crashing.
            if ( i + AES_BLOCK_SIZE > input.size( ) ) { 
                //printf("[ T! ] Resizing by %d bytes. \n", ( input.size( ) - i ));
                encrypted.resize( input.size( ) + ( input.size( ) - i ) );
            }

            //printf("[ T! ] Cycle: %d \n", c );

            AES_encrypt( input.data( ) + i, encrypted.data( ) + i, &aesKey );
        }

        return encrypted;
    }

    std::vector<uint8_t> Decrypt(std::vector<uint8_t>& encrypted, std::vector<uint8_t>& key)
    {
        std::vector<uint8_t> decrypted;

        AES_KEY aesKey;
        AES_set_decrypt_key(key.data(), 256, &aesKey);

        // Set up output buffer
        decrypted.resize(encrypted.size());

        for ( size_t i = 0; i < encrypted.size( ); i += AES_BLOCK_SIZE ) {

            // IMPORTANT: Pad end so that we avoid writing to nullptr and crashing.
            if ( i + AES_BLOCK_SIZE > encrypted.size( ) ) { 
                //printf("[ T! ] Resizing by %d bytes. \n", ( encrypted.size( ) - i ));
                decrypted.resize( encrypted.size( ) + ( encrypted.size( ) - i ) );
            }

            AES_decrypt( encrypted.data( ) + i, decrypted.data( ) + i, &aesKey );
        }

        return decrypted;
    }
    
    /*std::string Hash( const std::string Input ) 
    {
        unsigned char hash[ SHA256_DIGEST_LENGTH ];
        SHA256_CTX sha256;
        SHA256_Init( &sha256 );
        SHA256_Update( &sha256, Input.c_str( ), Input.size( ) );
        SHA256_Final( hash, &sha256 );

        std::stringstream ss;
        for ( int i = 0; i < SHA256_DIGEST_LENGTH; i++ )
        {
            ss << std::hex << std::setw( 2 ) << std::setfill( '0' ) << ( int ) hash[ i ];
        }
        return ss.str( );
    }*/

};