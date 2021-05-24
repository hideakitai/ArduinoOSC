#pragma once
#ifndef ARDUINOOSC_OSCUDPMAP_H
#define ARDUINOOSC_OSCUDPMAP_H

#include <Arduino.h>
#include "util/ArxTypeTraits/ArxTypeTraits.h"
#include "util/ArxSmartPtr/ArxSmartPtr.h"
#include "util/ArxContainer/ArxContainer.h"

#include "OscMessage.h"
#include "OscEncoder.h"

namespace arduino {
namespace osc {

    template <typename S>
    class UdpMapManager {
        UdpMapManager() {}
        UdpMapManager(const UdpMapManager&) = delete;
        UdpMapManager& operator=(const UdpMapManager&) = delete;

        UdpMap<S> udp_map;

    public:
        static UdpMapManager& getInstance() {
            static UdpMapManager m;
            return m;
        }

        const UdpMap<S>& getUdpMap() const {
            return udp_map;
        }

        UdpRef<S> getUdp(const uint16_t port) {
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
            using namespace std;
#else
            using namespace arx;
#endif
            // use first port for PORT_DISCARD if some udp instances exist
            if (port == PORT_DISCARD) {
                if (udp_map.empty()) {
                    udp_map.insert(make_pair(port, UdpRef<S>(new S())));
                    udp_map[port]->begin(port);
                }
                return udp_map.begin()->second;
            }

            if (udp_map.find(port) == udp_map.end()) {
                // if there is udp listening to port 9, erase it to this port
                auto udp_discard_ref = udp_map.find(PORT_DISCARD);
                if (udp_discard_ref != udp_map.end()) {
                    udp_discard_ref->second->stop();
                    udp_map.erase(udp_discard_ref);
                }
                udp_map.insert(make_pair(port, UdpRef<S>(new S())));
                udp_map[port]->begin(port);
            }
            return udp_map[port];
        }
    };

}  // namespace osc
}  // namespace arduino

template <typename S>
using OscUdpMap = arduino::osc::UdpMap<S>;
template <typename S>
using OscUdpMapManager = arduino::osc::UdpMapManager<S>;

#endif  // ARDUINOOSC_OSCUDPMAP_H
