const crc = require('./crc.js');

const START_BYTE  = 0x7D;
const FINISH_BYTE = 0x7E;
const ESCAPE_BYTE = 0x7F;
const ESCAPE_MASK = 0x20;

let index = 0;
let buffer = [];
let packet = [];
let b_processing = false;
let b_available = false;

function isEscapeByte(arg) { return (arg >= START_BYTE) && (arg <= ESCAPE_BYTE); }

exports.encode = function(args)
{
    args.forEach((v, i) => {
        if (isEscapeByte(v))
        {
            args.splice(i, 1, v ^ ESCAPE_MASK);
            args.splice(i, 0, ESCAPE_BYTE);
        }
    });

    if (isEscapeByte(index))
    {
        args.unshift(index ^ ESCAPE_MASK);
        args.unshift(ESCAPE_BYTE);
    }
    else
    {
        args.unshift(index);
    }
    args.unshift(START_BYTE);
    let crc8 = crc.crc8(args);
    if (isEscapeByte(crc8))
    {
        args.push(ESCAPE_BYTE);
        args.push(crc8 ^ ESCAPE_MASK);
    }
    else
    {
        args.push(crc8);
    }
    args.push(FINISH_BYTE);
    return args;
}

exports.feed = function(args)
{
    args.forEach(d => {
        if (!b_processing)
        {
            buffer = [];
            if (d === START_BYTE)
            {
                b_processing = true;
                buffer.push(d);
            }
        }
        else
        {
            if (d === FINISH_BYTE)
            {
                let crc_received = buffer[buffer.length - 1];
                if (buffer[buffer.length - 2] === ESCAPE_BYTE) // before CRC byte can be ESCAPE_BYTE only if CRC is escaped
                {
                    crc_received ^= ESCAPE_MASK;
                    buffer.splice(buffer.length - 2, 2);
                }
                else
                {
                    buffer.pop();
                }

                let crc8 = crc.crc8(buffer);
                if (crc8 === crc_received)
                {
                    buffer.forEach((v, i) => {
                        if (v === ESCAPE_BYTE) buffer.splice(i, 2, buffer[i + 1] ^ ESCAPE_MASK);
                    });
                    buffer.shift();
                    packet = buffer;
                    b_processing = false;
                    b_available = true;
                }
            }
            else
            {
                buffer.push(d);
            }
        }
    });
}

exports.decode = function()
{
    b_available = false;
    return packet;
}

exports.available = function()
{
    return b_available;
}
