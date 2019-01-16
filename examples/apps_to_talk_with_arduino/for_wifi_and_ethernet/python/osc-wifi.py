from osc4py3.as_eventloop import *
from osc4py3 import oscbuildparse
from osc4py3 import oscmethod as osm
import time

osc_startup()

osc_udp_client("192.168.1.201", 10000, "aclientname")
osc_udp_server("0.0.0.0", 12000, "aservername")

def handler(address, *args):
    print(address, args)

osc_method("/*", handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)

try:
    while True:
        msg = oscbuildparse.OSCMessage('/lambda', ",ifs", [123, 4.5, "six"])
        osc_send(msg, "aclientname")
        osc_process() # one message, one call

        msg = oscbuildparse.OSCMessage('/callback', ",ifs", [1, 2.2, "test"])
        osc_send(msg, "aclientname")
        osc_process() # one message, one call

        msg = oscbuildparse.OSCMessage('/wildcard/abc/test', ",i", [1])
        osc_send(msg, "aclientname")
        osc_process() # one message, one call

        msg = oscbuildparse.OSCMessage('/need/reply', ",", [])
        osc_send(msg, "aclientname")
        osc_process() # one message, one call

        time.sleep(1)

except KeyboardInterrupt:

    # Properly close the system.
    osc_terminate()
