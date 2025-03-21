#ifndef SHA_LIB_H
#define SHA_LIB_H

#ifdef __cplusplus
extern "C" {
#endif


    #include "../../platformlib/include/platform.h"


	typedef struct 
	{
		uint32_t state[5];
		uint64_t count;
		byte     buffer[64];
	} 
	SHA1_CTX;

    #define SHA1_BLOCK_SIZE 20
    #define ROTL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))


	PLATFORM_API void  sha1_init(SHA1_CTX* context);
	PLATFORM_API void  sha1_update(SHA1_CTX* context, const byte* data, size_t len);
	PLATFORM_API void  sha1_final(byte digest[SHA1_BLOCK_SIZE], SHA1_CTX* context);
	PLATFORM_API void  sha1_transform(uint32_t state[5], const byte buffer[64]);
	PLATFORM_API byte* sha1(const byte* data, size_t len, byte* digest);


#ifdef __cplusplus
}
#endif

#endif /* SHA_LIB */