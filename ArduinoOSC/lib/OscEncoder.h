#pragma once

#ifndef ARDUINOOSC_OSCENCODER_H
#define ARDUINOOSC_OSCENCODER_H

#include "OscTypes.h"

namespace oscpkt {

    class PacketWriter
    {
        Storage storage;
        BundleType bundles;

    public:

        PacketWriter()
        {
            init();
        }

        PacketWriter& init()
        {
            storage.clear();
            bundles.clear();
            return *this;
        }

        PacketWriter& encode(const Message& msg)
        {
            msg.encode(storage, !bundles.empty());
            return *this;
        }

        uint32_t size() const { return (uint32_t)storage.size(); }
        const char* data() const { return storage.begin(); }

        PacketWriter& begin_bundle(const TimeTag& ts = TimeTag::immediate())
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

        PacketWriter& end_bundle()
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

    };


} // oscpkt

#endif // ARDUINOOSC_OSCENCODER_H
