from osc4py3.as_eventloop import *
from osc4py3 import oscbuildparse
from osc4py3 import oscmethod as osm
import time

osc_startup()

osc_udp_client("192.168.1.201", 54321, "client_send")
osc_udp_client("192.168.1.201", 54345, "client_bind")
osc_udp_server("0.0.0.0", 55555, "server_recv")
osc_udp_server("0.0.0.0", 54445, "server_published")

def handler(address, *args):
    print(address, args)

osc_method("/*", handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)

try:
    while True:
        msg = oscbuildparse.OSCMessage('/lambda/msg', ",ifs", [123, 4.5, "six"])
        osc_send(msg, "client_send")
        osc_process() # one message, one call

        msg = oscbuildparse.OSCMessage('/callback', ",ifs", [1, 2.2, "test"])
        osc_send(msg, "client_send")
        osc_process() # one message, one call

        msg = oscbuildparse.OSCMessage('/wildcard/abc/test', ",i", [1])
        osc_send(msg, "client_send")
        osc_process() # one message, one call

        msg = oscbuildparse.OSCMessage('/need/reply', ",", [])
        osc_send(msg, "client_send")
        osc_process() # one message, one call

        msg = oscbuildparse.OSCMessage('/bind/values', ",ifs", [345, 6.7, "string"])
        osc_send(msg, "client_bind")
        osc_process() # one message, one call

        msg = oscbuildparse.OSCMessage('/lambda/bind/args', ",ifs", [789, 1.23, "bind"])
        osc_send(msg, "client_send")
        osc_process() # one message, one call

        time.sleep(1)

except KeyboardInterrupt:

    # Properly close the system.
    osc_terminate()
