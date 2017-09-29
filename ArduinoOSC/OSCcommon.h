#ifndef ARDUINOOSC_OSCCOMMON_H
#define ARDUINOOSC_OSCCOMMON_H

extern "C" {
    #include <inttypes.h>
}

static constexpr uint8_t kMaxOSCPacketSize = 128;
static constexpr uint8_t kMaxOSCArgSize = 15;
static constexpr uint8_t kMaxOSCDataBinSize = 4 * kMaxOSCArgSize;
static constexpr uint8_t kMaxOSCAdrChar = kMaxOSCPacketSize - (kMaxOSCDataBinSize + kMaxOSCArgSize + 1);
static constexpr uint8_t kMaxOSCCallback = 32;


#define CULC_ALIGNMENT(x) ((x + 4) & 0xfffc)

#define kTagInt32   'i'
#define kTagFloat   'f'
#define kTagString  's'

#endif // ARDUINOOSC_OSCCOMMON_H
