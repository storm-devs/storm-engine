#ifndef _VSTRING_CODEC_H_
#define _VSTRING_CODEC_H_

class VSTRING_CODEC
{
  public:
    VSTRING_CODEC(){};

    virtual ~VSTRING_CODEC(){};
    virtual uint32_t GetNum() = 0;
    virtual uint32_t Convert(const char *pString) = 0;
    virtual uint32_t Convert(const char *pString, long iLen) = 0;
    virtual const char *Convert(uint32_t code) = 0;

    virtual void VariableChanged() = 0;
};

#endif
