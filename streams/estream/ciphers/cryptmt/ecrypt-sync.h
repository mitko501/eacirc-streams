/* ecrypt-sync.h */

/*
 * Header file for synchronous stream ciphers without authentication
 * mechanism.
 *
 * *** Please only edit parts marked with "[edit]". ***
 */

#ifndef CRYPTMT_SYNC
#define CRYPTMT_SYNC

#include "../../estream_interface.h"
#include "../ecrypt-portable.h"

/* ------------------------------------------------------------------------- */

/* Cipher parameters */

/*
 * The name of your cipher.
 */
#define CRYPTMT_NAME "CryptMT-v3" /* [edit] */
#define CRYPTMT_PROFILE "S3___"

/*
 * Specify which key and IV sizes are supported by your cipher. A user
 * should be able to enumerate the supported sizes by running the
 * following code:
 *
 * for (i = 0; ECRYPT_KEYSIZE(i) <= ECRYPT_MAXKEYSIZE; ++i)
 *   {
 *     keysize = ECRYPT_KEYSIZE(i);
 *
 *     ...
 *   }
 *
 * All sizes are in bits.
 */

#define CRYPTMT_MAXKEYSIZE 2048            /* [edit] */
#define CRYPTMT_KEYSIZE(i) (128 + (i)*128) /* [edit] */

#define CRYPTMT_MAXIVSIZE 2048            /* [edit] */
#define CRYPTMT_IVSIZE(i) (128 + (i)*128) /* [edit] */

/* ------------------------------------------------------------------------- */

/* Data structures */

/*
 * ECRYPT_ctx is the structure containing the representation of the
 * internal state of your cipher.
 */

typedef struct {
#if defined(__ALTIVEC__)
    vector unsigned int dummy;
#else
    ALIGN(u8, dummy, 1);
#endif
    u32 sfmt[156 + 2 + ((CRYPTMT_MAXKEYSIZE + CRYPTMT_MAXIVSIZE) * 3) / 128][4];
    /* the array for the state vector  */
    u32 accum[4]; /* filter */
    u32 lung[4];  /* booter */
    u32* psfmt;   /* pointer to sfmt internal state */
    u32 length;   /* length of first block */
    u32 key[CRYPTMT_MAXKEYSIZE / 32];
    s32 keysize; /* size in 16bit words (bits / 32) */
    s32 ivsize;  /* size in 16bit words (bits / 32) */
    s32 first;   /* first flag */
} CRYPTMT_ctx;

/* ------------------------------------------------------------------------- */

class ECRYPT_Cryptmt : public estream_interface {
    CRYPTMT_ctx _ctx;

public:
    /* Mandatory functions */

    /*
     * Key and message independent initialization. This function will be
     * called once when the program starts (e.g., to build expanded S-box
     * tables).
     */
    void ECRYPT_init(void) override;

    /*
     * Key setup. It is the user's responsibility to select the values of
     * keysize and ivsize from the set of supported values specified
     * above.
     */
    void ECRYPT_keysetup(const u8* key,
                         u32 keysize,          /* Key size in bits. */
                         u32 ivsize) override; /* IV size in bits. */

    /*
     * IV setup. After having called ECRYPT_keysetup(), the user is
     * allowed to call ECRYPT_ivsetup() different times in order to
     * encrypt/decrypt different messages with the same key but different
     * IV's.
     */
    void ECRYPT_ivsetup(const u8* iv) override;

    /*
     * Encryption/decryption of arbitrary length messages.
     *
     * For efficiency reasons, the API provides two types of
     * encrypt/decrypt functions. The ECRYPT_encrypt_bytes() function
     * (declared here) encrypts byte strings of arbitrary length, while
     * the ECRYPT_encrypt_blocks() function (defined later) only accepts
     * lengths which are multiples of ECRYPT_BLOCKLENGTH.
     *
     * The user is allowed to make multiple calls to
     * ECRYPT_encrypt_blocks() to incrementally encrypt a long message,
     * but he is NOT allowed to make additional encryption calls once he
     * has called ECRYPT_encrypt_bytes() (unless he starts a new message
     * of course). For example, this sequence of calls is acceptable:
     *
     * ECRYPT_keysetup();
     *
     * ECRYPT_ivsetup();
     * ECRYPT_encrypt_blocks();
     * ECRYPT_encrypt_blocks();
     * ECRYPT_encrypt_bytes();
     *
     * ECRYPT_ivsetup();
     * ECRYPT_encrypt_blocks();
     * ECRYPT_encrypt_blocks();
     *
     * ECRYPT_ivsetup();
     * ECRYPT_encrypt_bytes();
     *
     * The following sequence is not:
     *
     * ECRYPT_keysetup();
     * ECRYPT_ivsetup();
     * ECRYPT_encrypt_blocks();
     * ECRYPT_encrypt_bytes();
     * ECRYPT_encrypt_blocks();
     */

    void ECRYPT_encrypt_bytes(const u8* plaintext,
                              u8* ciphertext,
                              u32 msglen) override; /* Message length in bytes. */

    void ECRYPT_decrypt_bytes(const u8* ciphertext,
                              u8* plaintext,
                              u32 msglen) override; /* Message length in bytes. */

/* ------------------------------------------------------------------------- */

/* Optional features */

/*
 * For testing purposes it can sometimes be useful to have a function
 * which immediately generates keystream without having to provide it
 * with a zero plaintext. If your cipher cannot provide this function
 * (e.g., because it is not strictly a synchronous cipher), please
 * reset the ECRYPT_GENERATES_KEYSTREAM flag.
 */

#define CRYPTMT_GENERATES_KEYSTREAM
#ifdef CRYPTMT_GENERATES_KEYSTREAM

    void CRYPTMT_keystream_bytes(CRYPTMT_ctx* ctx,
                                 u8* keystream,
                                 u32 length); /* Length of keystream in bytes. */

#endif

/* ------------------------------------------------------------------------- */

/* Optional optimizations */

/*
 * By default, the functions in this section are implemented using
 * calls to functions declared above. However, you might want to
 * implement them differently for performance reasons.
 */

/*
 * All-in-one encryption/decryption of (short) packets.
 *
 * The default definitions of these functions can be found in
 * "ecrypt-sync.c". If you want to implement them differently, please
 * undef the ECRYPT_USES_DEFAULT_ALL_IN_ONE flag.
 */
#define CRYPTMT_USES_DEFAULT_ALL_IN_ONE /* [edit] */

    void CRYPTMT_encrypt_packet(
            CRYPTMT_ctx* ctx, const u8* iv, const u8* plaintext, u8* ciphertext, u32 msglen);

    void CRYPTMT_decrypt_packet(
            CRYPTMT_ctx* ctx, const u8* iv, const u8* ciphertext, u8* plaintext, u32 msglen);

/*
 * Encryption/decryption of blocks.
 *
 * By default, these functions are defined as macros. If you want to
 * provide a different implementation, please undef the
 * ECRYPT_USES_DEFAULT_BLOCK_MACROS flag and implement the functions
 * declared below.
 */

#define CRYPTMT_BLOCKLENGTH (624 * 2) /* [edit] */

    /* #define ECRYPT_USES_DEFAULT_BLOCK_MACROS */ /* [edit] */
#ifdef CRYPTMT_USES_DEFAULT_BLOCK_MACROS

#define CRYPTMT_encrypt_blocks(ctx, plaintext, ciphertext, blocks)                                 \
    CRYPTMT_encrypt_bytes(ctx, plaintext, ciphertext, (blocks)*CRYPTMT_BLOCKLENGTH)

#define CRYPTMT_decrypt_blocks(ctx, ciphertext, plaintext, blocks)                                 \
    CRYPTMT_decrypt_bytes(ctx, ciphertext, plaintext, (blocks)*CRYPTMT_BLOCKLENGTH)

#ifdef CRYPTMT_GENERATES_KEYSTREAM

#define CRYPTMT_keystream_blocks(ctx, keystream, blocks)                                           \
    CRYPTMT_keystream_bytes(ctx, keystream, (blocks)*CRYPTMT_BLOCKLENGTH)

#endif

#else

    void CRYPTMT_encrypt_blocks(CRYPTMT_ctx* ctx,
                                const u8* plaintext,
                                u8* ciphertext,
                                u32 blocks); /* Message length in blocks. */

    void CRYPTMT_decrypt_blocks(CRYPTMT_ctx* ctx,
                                const u8* ciphertext,
                                u8* plaintext,
                                u32 blocks); /* Message length in blocks. */

#ifdef CRYPTMT_GENERATES_KEYSTREAM

    void CRYPTMT_keystream_blocks(CRYPTMT_ctx* ctx,
                                  u8* keystream,
                                  u32 blocks); /* Keystream length in blocks. */

#endif

#endif
};
/*
 * If your cipher can be implemented in different ways, you can use
 * the ECRYPT_VARIANT parameter to allow the user to choose between
 * them at compile time (e.g., gcc -DECRYPT_VARIANT=3 ...). Please
 * only use this possibility if you really think it could make a
 * significant difference and keep the number of variants
 * (ECRYPT_MAXVARIANT) as small as possible (definitely not more than
 * 10). Note also that all variants should have exactly the same
 * external interface (i.e., the same ECRYPT_BLOCKLENGTH, etc.).
 */
#define CRYPTMT_MAXVARIANT 1 /* [edit] */

#ifndef CRYPTMT_VARIANT
#define CRYPTMT_VARIANT 1
#endif

#if (CRYPTMT_VARIANT > CRYPTMT_MAXVARIANT)
#error this variant does not exist
#endif

/* ------------------------------------------------------------------------- */

#endif
