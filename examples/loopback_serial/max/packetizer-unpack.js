inlets = 3;
outlets = 1;

packetizer_index = 0;
packetizer_check = 2; // 0: None, 1: Sum, 2: CRC8
packet = [];

function getCRC8(args)
{
    var result = 0xFF;

    var size = args.length;

    var count = 0;
    for (result = 0 ; size != 0 ; size--)
    {
        result ^= args[count];

        for (var i = 0 ; i < 8; i++)
        {
            if (result & 0x80)
            {
                result <<= 1; result ^= 0x85; // x8 + x7 + x2 + x0
            }
            else
            {
                result <<= 1;
            }
        }
        count++;
    }
    return result;
}

function msg_int(a)
{
    const START_BYTE = 0x7E;
    const ESCAPE_BYTE = 0x7D;
    const ESCAPE_MASK = 0x20;
    const ESCAPE_MARKER = 0xFFFF;

    const SEND_BUFFER_SIZE = 128;
    const READ_BUFFER_SIZE = 128;

    var args = arrayfromargs(messagename, arguments);
    var outs = [];

    if (this.inlet === 1) packetizer_index = arguments[0];
    if (this.inlet === 2) packetizer_check = arguments[0];

    for (var i = 0; i < args.length; ++i)
    {
        if (args[i] === ESCAPE_BYTE)
        {
            var val = args[i + 1] ^ ESCAPE_MASK;
            args.splice(i, 2, val);
        }
    }

    Array.prototype.push.apply(packet, args);

    if (packet[0] !== START_BYTE)
    {
        var index = 0;
        for (var i = 0; i < packet.length; ++i)
        {
            if (packet[i] === START_BYTE)
            {
                index = i;
                break;
            }
        }
        for (var i = 0; i < index; ++i) packet.shift();
    }

    if (packet.length < 3) return;
    if (packet.length < 3 + packet[2] + 1) return;


    if (packetizer_check === 1)
    {
        var sum = 0;
        for (var i = 0; i < packet.length - 1; ++i) sum += packet[i];
        if (packet.pop() === sum)
        {
            for (var a = 0; a < 3; ++a) packet.shift();
            outlet(0, packet);
        }
        packet = [];
    }
    else if (packetizer_check === 2)
    {
        // TODO: slice(0, packet.length - 1)
        var arr = packet.slice(3, -1);
        var crc8 = getCRC8(arr) & 0xFF;
        if (packet.pop() === crc8)
        {
            for (var a = 0; a < 3; ++a) packet.shift();
            outlet(0, packet);
        }
        packet = [];
    }
}

