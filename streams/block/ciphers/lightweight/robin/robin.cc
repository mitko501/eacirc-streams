//
// Created by mhajas on 7/22/18.
//

#include "robin.h"

namespace block {

    void robin::Encrypt(uint8_t *block) {
        /*
         *
         * Access state as 16-bit values
         * Assumes little-endian machine
         *
         */
        uint16_t *data = (uint16_t *)block;
        uint16_t *key = (uint16_t *)_key;
        uint8_t i, j;


        /* Initial key addition */
        for (j = 0; j < 8; j++)
        {
            data[j] ^= READ_ROUND_KEY_WORD(key[j]);
        }

        for (i = 0; i < _rounds; i++)
        {
            /* Round constant */
            data[0] ^= READ_LBOX_WORD(LBox1[i + 1]);

            /* SBox layer (bitsliced) */
            SBOX(data);

            /* LBox layer (tables) */
            for (j = 0; j < 8; j++)
            {
                data[j] = READ_LBOX_WORD(LBox2[data[j] >> 8]) ^
                          READ_LBOX_WORD(LBox1[data[j] & 0xff]);
            }

            /* Key addition */
            for (j = 0; j < 8; j++)
            {
                data[j] ^= READ_ROUND_KEY_WORD(key[j]);
            }
        }
    }

    void robin::Decrypt(uint8_t *block) {
        /*
         *
         * Access state as 16-bit values
         * Assumes little-endian machine
         *
         */
        uint16_t *data = (uint16_t *)block;
        uint16_t *key = (uint16_t *)_key;
        uint8_t i, j;


        /* Initial key addition */
        for (j = 0; j < 8; j++)
        {
            data[j] ^= READ_ROUND_KEY_WORD(key[j]);
        }

        for (i = 0; i < _rounds; i++)
        {
            /* LBox layer (tables) */
            for (j = 0; j < 8; j++)
            {
                data[j] = READ_LBOX_WORD(LBox2[data[j] >> 8]) ^
                          READ_LBOX_WORD(LBox1[data[j] & 0xff]);
            }

            /* SBox layer (bitsliced) */
            SBOX(data);

            /* Key addition */
            for (j = 0; j < 8; j++)
            {
                data[j] ^= READ_ROUND_KEY_WORD(key[j]);
            }

            /* Round constant */
            data[0] ^= READ_LBOX_WORD(LBox1[16 - i]);
        }
    }
}