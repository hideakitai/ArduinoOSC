# DebugLog

Serial based assertion and log library for Arduino


## Feature

- print variadic arguments in one line
- release mode (`#define DEBUGLOG_RELEASE_MODE`) can disables debug info (`LOG_XXXX`)
- log level control
- control automatically/manually saving to SD card
- [SdFat](https://github.com/greiman/SdFat) support


## Usage

```C++
// uncommend DEBUGLOG_RELEASE_MODE disables ASSERT and all debug serial (Release Mode)
//#define DEBUGLOG_RELEASE_MODE

// you can also set default log level by defining macro
// #define DEBUGLOG_DEFAULT_LOGLEVEL LogLevel::WARNINGS

#include <DebugLog.h>

void setup() {
    Serial.begin(115200);

    // you can change target stream (default: Serial, only for Arduino)
    // LOG_ATTACH_SERIAL(Serial2);

    // PRINT and PRINTLN are always enabled regardless of debug mode or release mode
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
Available macros are listed below.
`PRINT` and  `PRINTLN` are available in both release and debug mode.

```C++
#define PRINT(...)
#define PRINTLN(...)
```

These APIs are enabled only in debug mode.

```C++
#define LOG_ERROR(...)
#define LOG_WARNING(...)
#define LOG_VERBOSE(...)
#define ASSERT(b)
```
Several options can be used.

```C++
#define LOG_GET_LEVEL()
#define LOG_SET_LEVEL(l)
#define LOG_SET_OPTION(file, line, func)
#define LOG_SET_DELIMITER(d)
// Arduino Only
#define LOG_SD_FLUSH()
#define LOG_SD_CLOSE()
#define LOG_ATTACH_SERIAL(s)
#define LOG_ATTACH_SD(s, p, b, ...)
```

These macros can be used in standard C++ apps.


### Log Level

```C++
enum class LogLevel {
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

void setup() {
  if (SD.begin()) {
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


### Control Scope

You can control the scope of `DebugLog` by including following header files.

- `DebugLogEnable.h`
- `DebugLogDisable.h`
- `DebugLogRestoreState.h`

After including `DebugLogEnable.h` or `DebugLogDisable.h`, macros are enabled/disabled.
Finally you should include `DebugLogRestoreState.h` to restore the previous state.
Please see practical example  `examples/control_scope` for details.



```C++
#define DEBUGLOG_RELEASE_MODE
#include <DebugLog.h>

// here is release mode (disable DebugLog)

#include <DebugLogEnable.h>

// here is debug mode (enable DebugLog)

#include <DebugLogRestoreState.h>

// here is release mode (restored)
```

## Used Inside of

- [MsgPack](https://github.com/hideakitai/MsgPack)
- [ES920](https://github.com/hideakitai/ES920)


## License

MIT

