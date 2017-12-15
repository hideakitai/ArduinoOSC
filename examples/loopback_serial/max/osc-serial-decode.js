inlets = 1;
outlets = 1;

function isInteger(x)
{
    x = parseFloat(x);
    return Math.round(x) === x;
}

function anything(a)
{
    var args = arrayfromargs(messagename, arguments);
    var outs = [];

    var packet = args;

    var start_code = '/'.charCodeAt(0);

    if (packet[0] !== start_code)
    {
        for (var i = 0; i < packet.length; ++i)
        {
            if (packet[i] === start_code) break;
            packet.shift();
        }
    }

    var addr_end = 0
    for (var i = 0; i < packet.length; ++i)
    {
        if (packet[i] === 0)
        {
            addr_end = i;
            break;
        }
    }

    if (addr_end === 0) return;

    var addr = "";
    for (var i = 0; i < addr_end; ++i)
    {
        addr += String.fromCharCode(packet[i]);
    }
    outs.push(addr);

    var tag_code = ','.charCodeAt(0);
    var tag_start = 0;
    for (var i = addr_end; i < packet.length; ++i)
    {
        if (packet[i] === tag_code)
        {
            tag_start = i + 1;
            break;
        }
    }

    if (tag_start === 0) return;
    if (tag_start >= packet.length) return;

    var tag_end = 0;
    for (var i = tag_start; i < packet.length; ++i)
    {
        if (packet[i] === 0)
        {
            tag_end = i;
            break;
        }
    }

    var arg_size = tag_end - tag_start;

    var arg_remainder = (arg_size + 1) % 4;
    var data_start = tag_end + 4 - arg_remainder;

    for (var i = 0; i < arg_size; ++i)
    {
        var buffer = ArrayBuffer(4);

        if (packet[tag_start + i] === 'i'.charCodeAt(0))
        {
            var int32 = Int32Array(buffer);
            var integer = 0;
            integer |= (packet[data_start + 4 * i + 0] << 24);
            integer |= (packet[data_start + 4 * i + 1] << 16);
            integer |= (packet[data_start + 4 * i + 2] <<  8);
            integer |= (packet[data_start + 4 * i + 3] <<  0);
            int32[0] = integer;
            outs.push(int32[0]);
        }
        else if (packet[tag_start + i] === 'f'.charCodeAt(0))
        {
            var int32 = Int32Array(buffer);
            var float32 = Float32Array(buffer);
            var integer = 0;
            integer |= (packet[data_start + 4 * i + 0] << 24);
            integer |= (packet[data_start + 4 * i + 1] << 16);
            integer |= (packet[data_start + 4 * i + 2] <<  8);
            integer |= (packet[data_start + 4 * i + 3] <<  0);
            int32[0] = integer;
            outs.push(float32[0]);
        }
        else if (packet[tag_start + i] === 's'.charCodeAt(0))
        {
            var s = data_start + 4 * i;
            var str = "";
            for (var c = s; c < packet.length; ++c)
                str += String.fromCharCode(packet[c]);
            outs.push(str);
        }
    }

    outlet(0, outs);
}
