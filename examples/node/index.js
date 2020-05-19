"use strict"

const osc = require('node-osc');

const server = new osc.Server(12000, '0.0.0.0');
server.on("message", function (msg, rinfo) {
      console.log("message : ");
      console.log(msg);
      console.log("press Ctrl-C to exit...");
});

const client = new osc.Client('192.168.1.201', 10000);
client.send('/lambda', 123, 4.5, "six");
client.send('/callback', 1, 2.2, "test");
client.send('/wildcard/abc/test', 1);
client.send('/need/reply');

