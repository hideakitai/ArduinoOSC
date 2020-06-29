# ArxTypeTraits

C++ type_traits for Arduino which cannot use it as default


## Supported Boards

C++11 class templates of this library is currently enabled only if you use following architecture to avoid conflict with standard `type_traits`.

- AVR (Uno, Nano, Mega, etc.)
- MEGAAVR (Uno WiFi, Nano Ecery, etc.)
- SAM (Due)
- SAMD (Zero, MKR, M0, etc.)
- SPRESENSE

Others are automatically enabled/disabled depending on compiler.


## Supported Class Templates

### C++11 (defined only for platforms above which cannot use `type_traits`)

- `std::integral_constant`
- `std::true_type`
- `std::false_type`
- `std::declval`
- `std::enable_if`
- `std::conditional`
- `std::remove_cv`
- `std::remove_const`
- `std::remove_volatile`
- `std::remove_pointer`
- `std::remove_reference`
- `std::remove_extent`
- `std::add_pointer`
- `std::forward`
- `std::is_same`
- `std::is_integral`
- `std::is_floating_point`
- `std::is_arithmetic`
- `std::is_signed`
- `std::is_unsigned`
- `std::is_pointer`
- `std::is_array`
- `std::is_convertible`
- `std::is_function`
- `std::is_empty`
- `std::decay`
- `std::result_of`


#### for utility

- `std::numeric_limits` (only `max()` and `min()` now)
- `std::swap`
- `std::initializer_list`
- `std::tuple`
- `std::get`
- `std::tuple_size`
- `std::function`


### C++14 (defined only for boards before C++14)

- `std::enable_if_t`
- `std::decay_t`
- `std::remove_cv_t`
- `std::remove_const_t`
- `std::remove_volatile_t`
- `std::remove_reference_t`
- `std::remove_pointer_t`
- `std::integer_sequence`
- `std::index_sequence`
- `std::make_index_sequence`
- `std::index_sequence_for`


### C++17 (defined only for boards before C++17)

- `std::void_t`
- `std::disjunction`
- `std::conjunction`
- `std::negation`
- `std::apply`


### C++2a

- `std::remove_cvref`
- `std::remove_cvref_t`


### Others (defined for all boards)

- `arx::is_detected`
- `arx::is_callable`
- `arx::function_traits`


## Used Inside of

- [Packetizer](https://github.com/hideakitai/Packetizer)
- [MsgPack](https://github.com/hideakitai/MsgPack)
- [MsgPacketizer](https://github.com/hideakitai/MsgPacketizer)
- [ArduinoOSC](https://github.com/hideakitai/ArduinoOSC)
- [PollingTimer](https://github.com/hideakitai/PollingTimer)
- [Tween](https://github.com/hideakitai/Tween)
- [ArxStringUtils](https://github.com/hideakitai/ArxStringUtils)

## License

MIT
