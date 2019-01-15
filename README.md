# ArduinoOSC

OSC library for Arduino (ESP, Teensy, AVR, etc.)

ArduinoOSC is OSC Library for Arduino. OSC packet parsing is based on the [oscpkt](http://gruntthepeon.free.fr/oscpkt/html/) and optimized for Arduino.

## NOTE : BREAKING API CHANGES (v0.2.0 or later)

Almost all APIs has been changed in `v0.2.0` and got much simpler.
Please check below if you use previous versions.


## Supported Platform

This library is tested for following platforms and interfaces.

- ESP32 (WiFi, Serial)
- ESP8266 (WiFi, Serial)
- Teensy 3.x (Ethernet, Serial, Serial1, 2, 3...)
- Arduino Mega (Ethernet, Serial, Serial1, 2, 3)
- Arduino Uno (Ethernet, Serial)


## Feature

- simpler usage than ever
    - callback registration with lambda
    - osc packet sending in one-line
- support pattern-matching (wildcards)
- support basic OSC types
	- TF (bool: true, false)
	- i (int32_t)
	- h (int64_t)
	- f (float)
	- d (double)
	- s (string)
	- b (bundle)
- does NOT support timestamp values.

## Usage

Please see examples for detals.

### WiFi

```C++
#include <ArduinoOSC.h>
OscWiFi osc;

void setup()
{
    WiFi.begin(ssid, pwd);
    WiFi.config(ip, gateway, subnet);
    osc.begin(recv_port);
    
    // add callbacks...
    osc.subscribe("/lambda", [](OscMessage& m)
    {
        // do something with osc message
        Serial.print(m.arg<int>(0));    Serial.print(" ");
        Serial.print(m.arg<float>(1));  Serial.print(" ");
        Serial.print(m.arg<String>(2)); Serial.println();
    });
}

void loop()
{
    osc.parse(); // should be called
    osc.send(host, send_port, "/send", 1, 2.2F, 3.3, "string"); // send osc packet in one line
}
```

### Ethernet

```C++
#include <ArduinoOSC.h>
OscSerial osc;

void setup()
{
    Ethernet.begin(mac, ip);
    osc.begin(recv_port);	
    
    // add callbacks...
}

void loop()
{
    osc.parse(); // should be called
    osc.send(host, send_port, "/send", 1, 2.2F, 3.3, "string");
}
```

### Serial

```C++
#include <ArduinoOSC.h>

void setup()
{
    Serial.begin(115200);
    osc.attach(Serial);
	
	// add callbacks...
}

void loop()
{
    osc.parse(); // should be called
    osc.send("/send", 1, 2.2F, 3.3, "string");
}
```


### Subscribe Callbacks / Publish OSC (TBD)

```C++
// TODO: TBD
// osc.subscribe("/int32", i);
// osc.subscribe("/float", f);
// osc.subscribe("/string", s);
// osc.subscribe("/blob", b);

osc.subscribe("/callback", onOscReceived); // old style (v0.1.x)

osc.subscribe("/lambda", [](OscMessage& m)
{
    Serial.print("lambda : ");
    Serial.print(m.ip()); Serial.print(" ");
    Serial.print(m.port()); Serial.print(" ");
    Serial.print(m.size()); Serial.print(" ");
    Serial.print(m.address()); Serial.print(" ");
    Serial.print(m.arg<int>(0)); Serial.print(" ");
    Serial.print(m.arg<float>(1)); Serial.print(" ");
    Serial.print(m.arg<String>(2)); Serial.println();
});
osc.subscribe("/wildcard/*/test", [](OscMessage& m)
{
    Serial.print("wildcard : ");
    Serial.print(m.ip()); Serial.print(" ");
    Serial.print(m.port()); Serial.print(" ");
    Serial.print(m.size()); Serial.print(" ");
    Serial.print(m.address()); Serial.print(" ");
    Serial.print(m.arg<int>(0)); Serial.println();

});
osc.subscribe("/need/reply", [](OscMessage& m)
{
    Serial.println("/need/reply");

    int i = 12;
    float f = 34.56F;
    double d = 78.987;
    String s = "hello";
    bool b = true;

    osc.send(host, send_port, "/send", i, f, d, s, b);
});

// TODO: TBD
// osc.publish(host, send_port, "/value", value);
// osc.publish(host, send_port, "/millis", &millis);
```


## Limitation for AVR Boards (Uno, Mega, etc.)

- max number of arguments is 3
- max packet data size is 64 byte
- max number of callbacks is 8
- osc packet is not queued (only latest packet can be held inside)
- bundle is not supported
- limited API (see below and `examples/UnoMegaAvr/*`)

### Read API Limitation

`m.arg<type>(index)` cannot be used in AVR. 
Please use old APIs.

```C++
m.getArgAsInt32(0);
m.getArgAsFloat(1);
m.getArgAsString(2);
```

### Send API Limitation

In sending osc, one-line feature is not available in AVR.
Please create `OscMessage` and `send(msg)` after that.

```C++
OscMessage msg(host, send_port, "/send"); // WiFi, Ethernet
OscMessage msg("/send");                  // Serial
msg.push(i).push(f).push(s);
osc.send(msg);
```


## TBD

- one-line subscriber for single variable

```C++
// directly changes variable 'i' if message with "/int32" comes
int32_t i;
osc.subscribe("/int32", i);
```

- one-line publisher for single variable

```C++
// send "value" automatically
float value;
osc.publish(host, send_port, "/value", value);
```

- one-line publisher for function which returns single value

```C++
// send the result of "millis()" automatically
osc.publish(host, send_port, "/millis", &millis);
```

- automatically detect the type of arguments (without template arguments)

```C++
int32_t i = m.arg<int32_t>(0);
float   f = m.arg<float>(1);
String  s = m.arg<String>(2);
// becomes
int32_t i = m.arg(0);
float   f = m.arg(1);
String  s = m.arg(2);
```


## License

MIT
