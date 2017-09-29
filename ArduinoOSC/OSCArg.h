#ifndef ARDUINOOSC_OSCARG_H
#define ARDUINOOSC_OSCARG_H

#include "OSCcommon.h"

class OSCArg
{

  public:
    OSCArg() {}
    OSCArg(char tag) : _typeTag(tag) {}
    OSCArg(char tag, void *data, uint16_t size, bool packSizeCulc)
    : _typeTag(tag), _dataSize(size)
    {
        if (packSizeCulc) _alignmentSize = CULC_ALIGNMENT(size);
        else              _alignmentSize = size;

        memset(_argData, 0, kMaxOSCDataBinSize);
        memcpy(_argData, data, _dataSize);
    }

    char getTypeTag() { return _typeTag; }
    uint8_t* getArgData() { return _argData; }
    uint16_t getDataSize() { return _dataSize; }
    uint16_t getAlignmentSize() { return _alignmentSize; }

  private:

    uint8_t _argData[kMaxOSCDataBinSize];
    char _typeTag {};
    uint16_t _dataSize {};
    uint16_t _alignmentSize {};
};

#endif // ARDUINOOSC_OSCARG_H
