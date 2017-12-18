#ifndef ARDUINOOSC_PATTERN_H
#define ARDUINOOSC_PATTERN_H

#include "OSCMessage.h"

class Pattern
{
public:

    typedef void (*AdrFunc)(OSCMessage&);

    void addOscAddress(const char* adr, AdrFunc func)
    {
        adrFunc[patternNum] = func;
        addr[patternNum] = adr;
        ++patternNum;
    }

    void execFunc(uint8_t index, OSCMessage& m)
    {
        adrFunc[index](m);
    }

    void paternComp(OSCMessage& m)
    {
        for (uint8_t i=0 ; i<patternNum; i++)
        {
            Serial.print("match");
            Serial.print(i);
            Serial.print(" ");
            Serial.println(addr[i]);
            Serial.println("and");
            Serial.println(m.getOSCAddress());
            if (strcmp(addr[i] , m.getOSCAddress()) == 0) execFunc(i , m);
        }
    }

private:

    AdrFunc adrFunc[kMaxOSCCallback];
	const char* addr[kMaxOSCCallback];
    uint8_t patternNum {0};
};


#endif // ARDUINOOSC_PATTERN_H
