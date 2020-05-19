#pragma once

#ifndef ARDUINOOSC_OSCENCODER_H
#define ARDUINOOSC_OSCENCODER_H

#include <Arduino.h>
#include "OscTypes.h"
#include "OscMessage.h"

namespace arduino {
namespace osc {
namespace message {

    class Encoder
    {
        Storage storage;

#ifndef ARDUINOOSC_DISABLE_BUNDLE
        BundleData bundles;
#endif

    public:

        Encoder()
        {
            init();
        }

        Encoder& init()
        {
            storage.clear();
#ifndef ARDUINOOSC_DISABLE_BUNDLE
            bundles.clear();
#endif
            return *this;
        }

        Encoder& encode(const Message& msg)
        {
#ifdef ARDUINOOSC_DISABLE_BUNDLE
            msg.encode(storage, false);
#else
            msg.encode(storage, !bundles.empty());
#endif
            return *this;
        }

        uint32_t size() const { return (uint32_t)storage.size(); }
        const uint8_t* data() const { return (const uint8_t*)storage.begin(); }

#ifndef ARDUINOOSC_DISABLE_BUNDLE

        Encoder& begin_bundle(const TimeTag& ts = TimeTag::immediate())
        {
            char *p;
            if (bundles.size()) p = storage.getBytes(4); // hold the bundle size
            p = storage.getBytes(8);
            strcpy(p, "#bundle");
            bundles.push_back(p - storage.begin());
            p = storage.getBytes(8);
            pod2bytes<uint64_t>(ts, p);
            return *this;
        }

        Encoder& end_bundle()
        {
            if (bundles.size())
            {
                if (storage.size() - bundles.back() == 16)
                    pod2bytes<uint32_t>(0, storage.getBytes(4)); // the 'empty bundle' case, not very elegant
                if (bundles.size() > 1)
                    pod2bytes<uint32_t>(uint32_t(storage.size() - bundles.back()), storage.begin() + bundles.back() - 4);
                bundles.pop_back();
            }
            return *this;
        }

#endif // ARDUINOOSC_DISABLE_BUNDLE

    };


} // namespace message
} // namespace osc
} // namespac arduino


using OscEncoder = arduino::osc::message::Encoder;


#endif // ARDUINOOSC_OSCENCODER_H
