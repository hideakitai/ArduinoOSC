inlets = 3;
outlets = 1;

packetizer_index = 0;
packetizer_check = 2; // 0: None, 1: Sum, 2: CRC8

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

    // TODO: replace crc
    if (packetizer_check === 2)
    {
        args.push(getCRC8(args) & 0xFF);
        args.unshift(args.length - 1);
    }
    else
    {
        args.unshift(args.length);
    }

    args.unshift(packetizer_index);
    args.unshift(START_BYTE);

    if (packetizer_check === 1)
    {
        var sum = 0;
        for (var i = 0; i < args.length; ++i) sum += args[i];
    }
    else if (packetizer_check === 2)
    {
        // TODO: replace crc to this
        // args.push(getCRC8(args) & 0xFF);
    }

    outlet(0, args);
}
