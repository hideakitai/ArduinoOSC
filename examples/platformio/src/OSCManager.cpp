#include "OSCManager.h"

OSCManager::OSCManager()
{
}

void OSCManager::onOscReceived(OscMessage &m)
{
    Serial.println("onOscReceived : Size ");
    Serial.print(m.size());
}
