/* libsodium: hmac_hmacsha512.c, v0.4.5 2014/04/16 */
/**
 * Copyright 2005,2007,2009 Colin Percival. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include "hmac_sha512.h"

#include <stdint.h>
#include <string.h>
#include "sha512.h"

void HMACSHA512(const uint8_t* input, size_t length, const uint8_t* key,
    size_t key_length, uint8_t digest[HMACSHA512_DIGEST_LENGTH])
{
    HMACSHA512CTX context;
    HMACSHA512Init(&context, key, key_length);
    HMACSHA512Update(&context, input, length);
    HMACSHA512Final(&context, digest);
}

void SHA512(const uint8_t *input, size_t length, uint8_t digest[SHA512_DIGEST_LENGTH])
{
  SHA512CTX context;
  SHA512Init(&context);
  SHA512Update(&context,input,length);
  SHA512Final(&context,digest);
};

void HMACSHA512Final(HMACSHA512CTX* context, 
    uint8_t digest[HMACSHA512_DIGEST_LENGTH])
{
    uint8_t hash[HMACSHA512_DIGEST_LENGTH];

    SHA512Final(&context->ictx, hash);
    SHA512Update(&context->octx, hash, HMACSHA512_DIGEST_LENGTH);
    SHA512Final(&context->octx, digest);

    memset((void*)hash, 0, sizeof hash);
}

void HMACSHA512Init(HMACSHA512CTX* context, const uint8_t* key, 
    size_t key_length)
{
    size_t i;
    uint8_t pad[SHA512_BLOCK_LENGTH];
    uint8_t key_hash[SHA512_DIGEST_LENGTH];

    if (key_length > SHA512_BLOCK_LENGTH)
    {
        SHA512Init(&context->ictx);
        SHA512Update(&context->ictx, key, key_length);
        SHA512Final(&context->ictx, key_hash);
        key = key_hash;
        key_length = SHA512_DIGEST_LENGTH;
    }

    SHA512Init(&context->ictx);
    memset(pad, 0x36, SHA512_BLOCK_LENGTH);

    for (i = 0; i < key_length; i++) 
        pad[i] ^= key[i];

    SHA512Update(&context->ictx, pad, SHA512_BLOCK_LENGTH);
    SHA512Init(&context->octx);
    memset(pad, 0x5c, SHA512_BLOCK_LENGTH);

    for (i = 0; i < key_length; i++) 
        pad[i] ^= key[i];

    SHA512Update(&context->octx, pad, SHA512_BLOCK_LENGTH);
    memset((void*)key_hash, 0, sizeof key_hash);
}

void HMACSHA512Update(HMACSHA512CTX* context, const uint8_t* input,
    size_t length)
{
    SHA512Update(&context->ictx, input, length);
}
