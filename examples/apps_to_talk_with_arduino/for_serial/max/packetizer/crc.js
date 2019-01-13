
exports.crc8 = function (args)
{
    let result = 0xFF;
    let size = args.length;
    let idx = 0;
    for (result = 0; size != 0; size--)
    {
        result ^= args[idx];
        for (let i = 0 ; i < 8; i++)
        {
            if (result & 0x80)
            {
                result <<= 1;
                result ^= 0x85; // x8 + x7 + x2 + x0
            }
            else
                result <<= 1;
        }
        idx++;
    }
    return result & 0xFF;
}
