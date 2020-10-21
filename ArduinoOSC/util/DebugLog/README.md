# DebugLog

Serial based assertion and log library for Arduino


## Feature

- print variadic arguments in one line
- release mode (`#define NDEBUG`) can disables print/log debug info
- log level control
- control automatically/manually saving to SD card
- [SdFat](https://github.com/greiman/SdFat) support


## Usage

These macros can be used in standard C++ apps.

```C++
// uncommend NDEBUG disables ASSERT and all debug serial (Release Mode)
//#define NDEBUG

#include <DebugLog.h>

void setup()
{
    Serial.begin(115200);

    // you can change target stream (default: Serial, only for Arduino)
    // LOG_ATTACH_SERIAL(Serial2);

    PRINT("this is for debug");
    PRINTLN(1, 2.2, "you can", "print variable args")

    // check log level 0: NONE, 1: ERRORS, 2: WARNINGS, 3: VERBOSE
    PRINTLN("current log level is", (int)LOG_GET_LEVEL());

    // set log level (default: DebugLogLevel::VERBOSE)
    LOG_SET_LEVEL(DebugLogLevel::ERRORS); // only ERROR log is printed
    LOG_SET_LEVEL(DebugLogLevel::WARNINGS); // ERROR and WARNING is printed
    LOG_SET_LEVEL(DebugLogLevel::VERBOSE); // all log is printed

    // set log output format options (show file, line, and func)
    // default: true, true, true
    LOG_SET_OPTION(false, false, true);

    LOG_ERROR("this is error log");
    LOG_WARNING("this is warning log");
    LOG_VERBOSE("this is verbose log");

    // you can change delimiter from default " " to anything
    LOG_SET_DELIMITER(" and ");
    LOG_VERBOSE(1, 2, 3, 4, 5);

    int x = 1;
    ASSERT(x != 1); // if assertion failed, Serial endlessly prints message
}
```

### Log Level

```C++
enum class LogLevel
{
    NONE     = 0,
    ERRORS   = 1,
    WARNINGS = 2,
    VERBOSE  = 3
};
```

### Save Log to SD Card

```C++
// if you want to use standard SD library
#include <SD.h>

// if you want to use SdFat
// #include <SdFat.h>
// SdFat SD;
// SdFatSdio SD;

// after that, include DebugLog.h
#include <DebugLog.h>

void setup()
{
  if (SD.begin())
  {
    String filename = "test.txt";
    LOG_ATTACH_SD(SD, filename, false, true);
    // 3rd arg => true: auto save every logging, false: manually save
    // 4th arg => true: only log to SD, false: also print via Serial
  }

  // if 3rd arg is true, logs will be automatically saved to SD
  LOG_ERROR("error!");

  // if 3rd arg is false, you should manually save logs
  // however this is much faster than auto save (saving takes few milliseconds)
  LOG_SD_FLUSH(); // manually save to SD card and continue logging
  // DebugLog::close(); // flush() and finish logging (ASSERT won't be saved to SD)
}
```

Please see `examples/sdcard` , `examples/sdcard_manual_save` for more details. And please note:

- one log function call can takes 3-20 ms if you log to SD (depending on environment)
- if you disable auto save, you should call `LOG_SD_FLUSH()` or `LOG_SD_CLOSE()` to save logs


## Used Inside of

- [MsgPack](https://github.com/hideakitai/MsgPack)
- [ES920](https://github.com/hideakitai/ES920)


## License

MIT

