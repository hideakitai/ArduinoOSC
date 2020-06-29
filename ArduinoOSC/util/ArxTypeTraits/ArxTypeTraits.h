#pragma once

#ifndef ARX_TYPE_TRAITS_H
#define ARX_TYPE_TRAITS_H

#if defined(ARDUINO_ARCH_AVR)\
 || defined(ARDUINO_ARCH_MEGAAVR)\
 || defined(ARDUINO_ARCH_SAM)\
 || defined(ARDUINO_ARCH_SAMD)\
 || defined(ARDUINO_spresense_ast)
    #define ARX_TYPE_TRAITS_DISABLED
    #if defined(ARDUINO_ARCH_MEGAAVR)\
    || defined(ARDUINO_ARCH_SAMD)\
    || defined(ARDUINO_spresense_ast)
        #define ARX_TYPE_TRAITS_NEW_DISABLED
    #endif
#endif

#include <stddef.h>
#ifndef ARX_TYPE_TRAITS_DISABLED
    #include <type_traits>
    #include <tuple>
    #include <functional>
#endif

#include "ArxTypeTraits/type_traits.h"

#endif // ARX_TYPE_TRAITS_H
