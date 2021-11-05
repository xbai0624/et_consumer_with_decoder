#include "FakeDecoder.h"
#include <iostream>

FakeDecoder::FakeDecoder()
{
}

FakeDecoder::~FakeDecoder()
{
}

FakeDecoder::FakeDecoder(unsigned int* buf, int &fBufLen)
{
    Decode(buf, fBufLen);
}

void FakeDecoder::Decode(unsigned int* buf, int &fBufLen)
{
    for(int i=0; i<fBufLen; i++)
    {
        std::cout<<std::hex<<buf[i]<<std::endl;
    }
}

void FakeDecoder::Clear()
{
}

void FakeDecoder::Decode(const uint32_t *pBuf, uint32_t fBufLen, std::vector<int> &vTagTrack)
{
    for(int i=0; i<fBufLen; i++)
    {
        std::cout<<std::hex<<pBuf[i]<<std::endl;
    }
}
