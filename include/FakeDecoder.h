#ifndef FAKE_DECODER_H
#define FAKE_DECODER_H

#include "AbstractRawDecoder.h"

#include <vector>

class FakeDecoder : public AbstractRawDecoder
{
public:
    FakeDecoder();
    FakeDecoder(unsigned int *, int &);
    ~FakeDecoder();

    void Decode(unsigned int*, int &);
    void Decode(const uint32_t *pBuf, uint32_t fBufLen, std::vector<int> &vTagTrack);
    void Clear();

private:        
};

#endif
