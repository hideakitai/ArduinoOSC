"use strict";

const maxApi = require('max-api');
const osc = require('osc-min');
const packetizer = require('./packetizer/packetizer.js');

maxApi.addHandler('encode', (...args) =>
{
    let addr = args[0];
    args.shift();

    // encode : osc message -> binary array
    for (let i = 0; i < args.length; ++i)
        if (Number.isInteger(args[i]))
            args.splice(i, 1, {type:"integer", value:args[i]});
    let obj = osc.toBuffer(addr, args);
    let arr = Array.from(obj);
    maxApi.outlet(packetizer.encode(arr));
});

maxApi.addHandler('decode', (...args) =>
{
    packetizer.feed(args);
    if (packetizer.available())
    {
        let packet = packetizer.decode();
        packet.shift(); // discard index

        // decode : binary array -> osc message
        let obj = osc.fromBuffer(new Buffer(packet));
        let data = Array.from(obj.args.values(), v => v["value"]);
        data.unshift(obj.address);
        maxApi.outlet(data);
    }
});

