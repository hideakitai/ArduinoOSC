# ArxSmartPtr

C++ smart pointer-like classes for Arduino which can't use standard smart pointers


## Note

- `ArxSmartPtr` is C++ smart pointer-__like__ classes for Arduino.
- Most of the functions are imported and simplified based on [Boost.SmartPtr](https://github.com/boostorg/smart_ptr).
- Automatically use standard library first if the boards can use them


## Supported SmartPtr

- `shared_ptr`


### Limitations

- Custom Deleter cannot be used
- Custom Allocater cannot be used
- `std::make_shared` is just an alias for constructor


## Supported Boards

This library is currently enabled only if you use following architecture.

- AVR (Uno, Nano, Mega, etc.)
- MEGAAVR (Uno WiFi, Nano Ecery, etc.)
- SAM (Due)


## Usage

Please see example for more information.

``` C++
{
    Serial.println("start");
    std::shared_ptr<Base> t1(new Base(4));
    std::shared_ptr<Base> t2;
    {
        std::shared_ptr<Base> t3(new Base(5));
        std::shared_ptr<Base> t4(new Base(6));
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
- [SceneManager](https://github.com/hideakitai/SceneManager)
- [TaskManager](https://github.com/hideakitai/TaskManager)


## Roadmap

This library will be updated if I want to use more smart pointer interfaces on supported boards shown above.
PRs are welcome!

## License

MIT
