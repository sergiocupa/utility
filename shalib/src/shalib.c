#include "../include/shalib.h"


void sha1_init(SHA1_CTX* context)
{
    context->count = 0;
    // Valores iniciais conforme o padrão (FIPS PUB 180-1)
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
}

void sha1_update(SHA1_CTX* context, const byte* data, size_t len)
{
    size_t i, j;
    j = (context->count >> 3) & 63;  // Quantos bytes já estão armazenados no buffer
    context->count += (uint64_t)len << 3; // Atualiza contagem de bits

    // Se houver dados suficientes para completar um bloco, processa-os
    if ((j + len) > 63) 
    {
        memcpy(&context->buffer[j], data, 64 - j);

        sha1_transform(context->state, context->buffer);

        for (i = 64 - j; i + 63 < len; i += 64) 
        {
            sha1_transform(context->state, data + i);
        }
        j = 0;
    }
    else {
        i = 0;
    }
    // Armazena os dados restantes no buffer
    memcpy(&context->buffer[j], data + i, len - i);
}

void sha1_final(byte digest[SHA1_BLOCK_SIZE], SHA1_CTX* context)
{
    unsigned char finalcount[8];
    unsigned char pad = 0x80;
    size_t i;

    // Converte a contagem de bits para um array de 8 bytes (big-endian)
    for (i = 0; i < 8; i++) 
    {
        finalcount[7 - i] = (unsigned char)((context->count >> (i * 8)) & 0xFF);
    }

    // Acrescenta o bit '1'
    sha1_update(context, &pad, 1);

    // Preenche com zeros até atingir 448 bits (56 bytes)
    unsigned char zero = 0x00;
    while (((context->count >> 3) & 63) != 56) 
    {
        sha1_update(context, &zero, 1);
    }

    // Acrescenta o tamanho original (em bits)
    sha1_update(context, finalcount, 8);

    // Armazena o resultado final (digest) no array de saída
    for (i = 0; i < 20; i++) 
    {
        digest[i] = (unsigned char)((context->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 0xFF);
    }
    // Zera o contexto (opcional, por segurança)
    memset(context, 0, sizeof(*context));
}

void sha1_transform(uint32_t state[5], const byte buffer[64])
{
    uint32_t W[80];
    uint32_t a, b, c, d, e, f, k, temp;

    // Prepara os 16 primeiros termos a partir do bloco (em big-endian)
    for (int t = 0; t < 16; t++) 
    {
        W[t] = ((uint32_t)buffer[t * 4]) << 24 | ((uint32_t)buffer[t * 4 + 1]) << 16 | ((uint32_t)buffer[t * 4 + 2]) << 8 | ((uint32_t)buffer[t * 4 + 3]);
    }

    // Expande a mensagem para 80 palavras
    for (int t = 16; t < 80; t++) 
    {
        W[t] = ROTL(W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16], 1);
    }

    // Inicializa as variáveis com o estado atual
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

    // Loop principal: 80 rodadas
    for (int t = 0; t < 80; t++)
    {
        if (t < 20)
        {
            f = (b & c) | ((~b) & d);
            k = 0x5A827999;
        }
        else if (t < 40)
        {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1;
        }
        else if (t < 60) 
        {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
        }
        else 
        {
            f = b ^ c ^ d;
            k = 0xCA62C1D6;
        }
        temp = ROTL(a, 5) + f + e + k + W[t];
        e = d;
        d = c;
        c = ROTL(b, 30);
        b = a;
        a = temp;
    }

    // Atualiza o estado
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

byte* sha1(const byte* data, size_t len, byte* digest)
{
    SHA1_CTX ctx;
    sha1_init(&ctx);
    sha1_update(&ctx, data, len);
    sha1_final(digest, &ctx);
    return digest;
}
