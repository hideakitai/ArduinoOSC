"use strict"

const osc = require('node-osc');

const server_recv = new osc.Server(55555, '0.0.0.0');
server_recv.on("message", function (msg, rinfo) {
    console.log("message : ");
    console.log(msg);
    console.log("press Ctrl-C to exit...");
});

const server_pubished = new osc.Server(54445, '0.0.0.0');
server_pubished.on("message", function (msg, rinfo) {
    console.log("message : ");
    console.log(msg);
    console.log("press Ctrl-C to exit...");
});

const client_send = new osc.Client('192.168.1.201', 54321);
const client_bind = new osc.Client('192.168.1.201', 54345);
function send() {
    client_send.send('/lambda/msg', 123, 4.5, "six");
    client_send.send('/callback', 1, 2.2, "test");
    client_send.send('/wildcard/abc/test', 1);
    client_send.send('/need/reply');

    client_bind.send('/bind/values', 345, 6.7, "string");
    client_bind.send('/lambda/bind/args', 789, 1.23, "string");
}

setInterval(send, 1500);
