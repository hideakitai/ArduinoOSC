#pragma once

#ifndef ARX_SMART_PTR_H
#define ARX_SMART_PTR_H

#include <Arduino.h>

#if defined(ARDUINO_ARCH_AVR)\
 || defined(ARDUINO_ARCH_MEGAAVR)\
 || defined(ARDUINO_ARCH_SAM)\
 || defined(ARDUINO_ARCH_SAMD)\
 || defined(ARDUINO_spresense_ast)
    #define ARX_SMART_PTR_DISABLED
#endif

#ifdef ARX_SMART_PTR_DISABLED
    #include "ArxSmartPtr/shared_ptr.h"
#endif

#endif // ARX_SMART_PTR_H
