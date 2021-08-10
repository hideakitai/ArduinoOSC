#include "OSCManager.h"

OSCManager::OSCManager()
{
}

void onOscReceived(OscMessage &m)
{
    Serial.println("onOscReceived : Size ");
    Serial.print(m.size());
}
