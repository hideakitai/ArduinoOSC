#ifndef ARDUINOOSC_OSCCOMMON_H
#define ARDUINOOSC_OSCCOMMON_H

extern "C" {
    #include <inttypes.h>
}

#ifdef __AVR__
static constexpr uint8_t kMaxOSCPacketSize = 128;
static constexpr uint8_t kMaxOSCArgSize = 8;
static constexpr uint8_t kMaxOSCDataBinSize = 4 * kMaxOSCArgSize;
static constexpr uint8_t kMaxOSCAdrChar = kMaxOSCPacketSize - (kMaxOSCDataBinSize + kMaxOSCArgSize + 1);
static constexpr uint8_t kMaxOSCCallback = 8;
static constexpr uint8_t kMaxOSCPacketBuffer = 2;
#else
static constexpr uint8_t kMaxOSCPacketSize = 128;
static constexpr uint8_t kMaxOSCArgSize = 15;
static constexpr uint8_t kMaxOSCDataBinSize = 4 * kMaxOSCArgSize;
static constexpr uint8_t kMaxOSCAdrChar = kMaxOSCPacketSize - (kMaxOSCDataBinSize + kMaxOSCArgSize + 1);
static constexpr uint8_t kMaxOSCCallback = 32;
static constexpr uint8_t kMaxOSCPacketBuffer = 32;
#endif

#define CULC_ALIGNMENT(x) ((x + 4) & 0xfffc)

#define kTagInt32   'i'
#define kTagFloat   'f'
#define kTagString  's'

#endif // ARDUINOOSC_OSCCOMMON_H
