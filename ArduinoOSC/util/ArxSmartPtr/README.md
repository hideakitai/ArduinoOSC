# ArxSmartPtr

C++ smart pointer-like classes for Arduino which can't use standard smart pointers


## Note

`ArxSmartPtr` is C++ smart pointer-__like__ classes for Arduino.
All of the functions is not supported currently.
Almost all functions are imported and simplified based on [Boost.SmartPtr](https://github.com/boostorg/smart_ptr).


## Supported SmartPtr

- `shared_ptr`


### Limitations

- Custom Deleter cannot be used
- Custom Allocater cannot be used
- `make_shared` is just an alias for constructor
- namespace `arx` is used instead of `std`


## Supported Boards

This library is currently enabled only if you use following architecture.
Please use C++ Standard Template Library for other boards.

- AVR (Uno, Nano, Mega, etc.)
- MEGAAVR (Uno WiFi, Nano Ecery, etc.)
- SAM (Due)
- SAMD (Zero, MKR, M0, etc.)
- SPRESENSE


## Usage

Please see example for more information.

``` C++
{
    Serial.println("start");
    arx::shared_ptr<Base> t1(new Base(4));
    arx::shared_ptr<Base> t2;
    {
        arx::shared_ptr<Base> t3(new Base(5));
        arx::shared_ptr<Base> t4(new Base(6));
        t2 = t3;
    }
    Serial.println("end");
}
// start
// Base::Constructor 4
// Base::Constructor 5
// Base::Constructor 6
// Base::Destructor 6
// end
// Base::Destructor 5
// Base::Destructor 4
```

## Used Inside of

- [Packetizer](https://github.com/hideakitai/Packetizer)
- [MsgPacketizer](https://github.com/hideakitai/MsgPacketizer)
- [ArduinoOSC](https://github.com/hideakitai/ArduinoOSC)
- [Tween](https://github.com/hideakitai/Tween)


## Roadmap

This library will be updated if I want to use more smart pointer interfaces on supported boards shown above.
PRs are welcome!

## License

MIT
