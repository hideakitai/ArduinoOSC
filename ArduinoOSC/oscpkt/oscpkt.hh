// 2018.12.31 Hideaki Tai
// This library is optimized to use mainly in Arduino
// and dropped support of following features
// - robust wrt malformed packets
// - optional udp transport for packets
// - error code handling

/** @mainpage OSCPKT : a minimalistic OSC ( http://opensoundcontrol.org ) c++ library

Before using this file please take the time to read the OSC spec, it
is short and not complicated: http://opensoundcontrol.org/spec-1_0

Features:
- handles basic OSC types: TFihfdsb
- handles bundles
- handles OSC pattern-matching rules (wildcards etc in message paths)
- portable on win / macos / linux
- robust wrt malformed packets
- optional udp transport for packets
- concise, all in a single .h file
- does not throw exceptions

does not:
- take into account timestamp values.
- provide a cpu-scalable message dispatching.
- not suitable for use inside a realtime thread as it allocates memory when
building or reading messages.


There are basically 3 classes of interest:
- oscpkt::Message       : read/write the content of an OSC message
- oscpkt::Decoder  : read the bundles/messages embedded in an OSC packet
- oscpkt::Encoder  : write bundles/messages into an OSC packet

And optionaly:
- oscpkt::UdpSocket     : read/write OSC packets over UDP.

@example: oscpkt_demo.cc
@example: oscpkt_test.cc
*/

/* Copyright (C) 2010  Julien Pommier

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

(this is the zlib license)
*/

#ifndef OSCPKT_HH
#define OSCPKT_HH

#endif // OSCPKT_HH
