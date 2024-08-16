#if !defined(ARDUINO_ARCH_ESP32)
#error "This example is for ESP32 only"
#endif

// #define ARDUINOOSC_DEBUGLOG_ENABLE

#include <ArduinoOSCWiFi.h>

// WiFi stuff
const char* ssid = "your-ssid";
const char* pwd = "your-password";
const IPAddress ip(192, 168, 0, 201);
const IPAddress gateway(192, 168, 0, 1);
const IPAddress subnet(255, 255, 255, 0);

// for ArduinoOSC
const char* host = "127.0.0.1";
const int port = 54321;

void onBundleReceived(const OscMessage& m) {
    Serial.print(m.remoteIP());
    Serial.print(" ");
    Serial.print(m.remotePort());
    Serial.print(" ");
    Serial.print(m.size());
    Serial.print(" ");
    Serial.print(m.address());
    Serial.print(" ");
    Serial.print(m.timeTag().value());
    Serial.print(" ");
    Serial.print(m.arg<int>(0));
    Serial.println();
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    // WiFi stuff (no timeout setting for WiFi)
    WiFi.disconnect(true, true);  // disable wifi, erase ap info
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pwd);
    WiFi.config(ip, gateway, subnet);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.print("WiFi connected, IP = ");
    Serial.println(WiFi.localIP());

    // subscribe osc messages
    OscWiFi.subscribe(port, "/bundle/foo", onBundleReceived);
    OscWiFi.subscribe(port, "/bundle/bar", onBundleReceived);
}

void loop() {
    OscWiFi.update();  // should be called to receive + send osc

static int counter = 0;

    const uint64_t now = esp_timer_get_time();
    const OscTimeTag tt(now);
    OscWiFi.begin_bundle(tt);
    OscWiFi.add_bundle("/bundle/foo", millis());
    OscWiFi.add_bundle("/bundle/bar", millis() / 1000);
    OscWiFi.end_bundle();
    OscWiFi.send_bundle(host, port);

counter++;

    delay(500);
}
