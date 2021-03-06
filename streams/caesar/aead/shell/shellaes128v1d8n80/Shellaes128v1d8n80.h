#ifndef SHELLAES128V1D8N80_H
#define SHELLAES128V1D8N80_H

#include "../../../CaesarInterface.h"

class Shellaes128v1d8n80 : public CaesarInterface {
    const int maxNumRounds = -1;
public:
    Shellaes128v1d8n80(int numRounds);
    ~Shellaes128v1d8n80();
    int encrypt(bits_t *c, length_t *clen, const bits_t *m, length_t mlen,
                        const bits_t *ad, length_t adlen, const bits_t *nsec, const bits_t *npub,
                        const bits_t *k);
    int decrypt(bits_t *m, length_t *outputmlen, bits_t *nsec,
                        const bits_t *c, length_t clen, const bits_t *ad, length_t adlen,
                        const bits_t *npub, const bits_t *k);
    std::string shortDescription() const;
};

#endif // SHELLAES128V1D8N80_H
