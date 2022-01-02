# DebugLog

Logging library for Arduino that can output to both Serial and File with one line

## Feature

- Output logs to `Serial` and `File` with one line at the same time
- Output logs with variadic arguments
- Assertion support (suspend program with messages if assertion fails)
- Release Mode `#define DEBUGLOG_DISABLE_LOG` can easily disable logging (`LOG_XXXX`, `ASSERT`)
- Log level control (`NONE`, `ERROR`, `WARN`, `INFO`, `DEBUG`, `TRACE`)
- Automatically or manually output log to file
- Multiple file system support (`SD`, `SdFat`, `SPIFFS`, etc.)
- Support array and container (`std::vector`, `std::deque`, `std::map`) output
- APIs can also be used in standard C++ apps

## Basic Usage

### Logging API Comparison

| APIs                         | Serial | File   | Log Level  | Release Mode |
| ---------------------------- | ------ | ------ | ---------- | ------------ |
| `LOG_XXXXX`                  | YES    | YES \* | CONTROLLED | DISABLED     |
| `ASSERT`, `ASSERTM`          | YES    | YES \* | IGNORED    | DISABLED     |
| `PRINT`, `PRINTLN`           | YES    | NO     | IGNORED    | ENABLED      |
| `PRINT_FILE`, `PRINTLN_FILE` | NO     | YES \* | IGNORED    | ENABLED \*   |

`*` : Only after `LOG_FS_ATTACH_AUTO` or `LOG_FS_ATTACH_MANUAL` is called

### Simple Log Example

`LOG_XXXX` output is controlled by log level. Default log level is `DebugLogLevel::LVL_INFO`

```C++
#include <DebugLog.h>

// The default log_leval is DebugLogLevel::LVL_INFO
LOG_ERROR("this is error: log level", 1);
LOG_WARN("this is warn: log level", 2);
LOG_INFO("this is info: log level", 3);
LOG_DEBUG("this is debug: log level", 4);  // won't be printed
LOG_TRACE("this is trace: log level", 5);  // won't be printed
```

`Serial` output example

```
[ERROR] basic.ino L.26 setup : this is error: log level 1
[WARN] basic.ino L.27 setup : this is warn: log level 2
[INFO] basic.ino L.28 setup : this is info: log level 3
```

### Log Level control

By defining `DEBUGLOG_DEFAULT_LOG_LEVEL_XXXX`, you can change default log level

```C++
// You can also set default log level by defining macro (default: INFO)
#define DEBUGLOG_DEFAULT_LOG_LEVEL_TRACE

// Include DebugLog after that
#include <DebugLog.h>
```

Or you can change log level dynamically by

```C++
// You can change log_leval by following macro
LOG_SET_LEVEL(DebugLogLevel::LVL_TRACE);
```

After setting log level to `DebugLogLevel::LVL_TRACE`

```C++
LOG_ERROR("this is error log");
LOG_WARN("this is warn log");
LOG_INFO("this is info log");
LOG_DEBUG("this is debug log");
LOG_TRACE("this is trace log");
```

will output

```
[ERROR] basic.ino L.26 setup : this is error: log level 1
[WARN] basic.ino L.27 setup : this is warn: log level 2
[INFO] basic.ino L.28 setup : this is info: log level 3
[DEBUG] basic.ino L.29 setup : this is debug: log level 4
[TRACE] basic.ino L.30 setup : this is trace: log level 5
```

### Assertion

`ASSERT` suspends program if the provided condition is `false`

```C++
int x = 1;
ASSERT(x != 1); // suspends program here
```

`ASSERTM` can also output message in addition to `ASSERT`

```C++
int x = 1;
ASSERTM(x != 1, "It's good to write the reason of fatal error");
```

### `PRINT` `PRINTLN` (always output to Serial)

`PRINT` and `PRINTLN` is not affected by log level (always visible) and log format

```C++
PRINT("DebugLog", "can print variable args: ");
PRINTLN(1, 2.2, "three", "=> like this");
```

`Serial` output example

```
DebugLog can print variable args: 1 2.20 three => like this
```

## Logging to File

### Attach to File System

You can log to `File` automatically by calling this macro. `LOG_XXXX` and `ASSERT` are automatically wrote to file depending on the log level

```C++
LOG_ATTACH_FS_AUTO(SD, "/log.txt", FILE_WRITE);
```

### Log Leval Control for File Output

By defining `DEBUGLOG_DEFAULT_FILE_LEVEL_XXXX`, you can change default log level. Default level is `DebugLogLevel::LVL_ERROR`.

```C++
// You can also set default file level by defining macro (default: ERROR)
#define DEBUGLOG_DEFAULT_FILE_LEVEL_INFO

// Include DebugLog after that
#include <DebugLog.h>
```

Or you can change log level dynamically by

```C++
// You can change log_leval by following macro
LOG_FILE_SET_LEVEL(DebugLogLevel::LVL_INFO);
```

### Notes for Auto Logging to `File`

- One log function call can takes 3-20 ms if you log to file (depending on environment)
- There is option to flush to file manually to avoid flushing every log
- If you've disabled auto saving, you should call `LOG_FILE_FLUSH()` or `LOG_FILE_CLOSE()` manually

### Flush File Manually

By calling `LOG_ATTACH_FS_MANUAL`, you can control flush timing manually

```C++
LOG_ATTACH_FS_MANUAL(fs, filename, FILE_WRITE);
```

You should call `LOG_FILE_FLUSH()` or `LOG_FILE_CLOSE()` manually to flush to the file

```C++
// If LOG_ATTACH_FS_MANUAL is used, you should manually save logs
// however this is much faster than auto save (saving takes few milliseconds)
LOG_FILE_FLUSH(); // manually save to SD card and continue logging
LOG_FILE_CLOSE(); // flush() and finish logging (ASSERT won't be saved to SD)
```

### `PRINT_FILE` `PRINTLN_FILE` (always output to File)

`PRINT_FILE` and `PRINTLN_FILE` is not affected by log level (always visible) and log format

```C++
PRINT_FILE("DebugLog", "can print variable args: ");
PRINTLN_FILE(1, 2.2, "three", "=> like this");
```

## Disable Logging Macro (Release Mode)

You can disable `LOG_XXXX` and `ASSERT` macro completely by defining following macro. By disabling log macros, you can save memory usage and cpu overhead (macro receives/calls nothing). Note that `PRINT` macros are not disabled even in the release mode.

```C++
#define DEBUGLOG_DISABLE_LOG

#include <DebugLog.h>
```

## Practical Example

```C++
// Uncommenting DEBUGLOG_DISABLE_LOG disables ASSERT and all log (Release Mode)
// PRINT and PRINTLN are always valid even in Release Mode
// #define DEBUGLOG_DISABLE_LOG

// You can also set default log level by defining macro (default: INFO)
// #define DEBUGLOG_DEFAULT_LOG_LEVEL_TRACE

#include <DebugLog.h>

void setup() {
    Serial.begin(115200);
    delay(2000);

    // PRINT and PRINTLN is not affected by log_level (always visible)
    PRINT("DebugLog", "can print variable args: ");
    PRINTLN(1, 2.2, "three", "=> like this");

    // You can change log_leval by following macro
    // LOG_SET_LEVEL(DebugLogLevel::LVL_TRACE);

    // The default log_leval is DebugLogLevel::LVL_INFO
    // 0: NONE, 1: ERROR, 2: WARN, 3: INFO, 4: DEBUG, 5: TRACE
    PRINTLN("current log level is", (int)LOG_GET_LEVEL());

    // The default log_leval is DebugLogLevel::LVL_INFO
    LOG_ERROR("this is error log");
    LOG_WARN("this is warn log");
    LOG_INFO("this is info log");
    LOG_DEBUG("this is debug log");  // won't be printed
    LOG_TRACE("this is trace log");  // won't be printed

    // Log array
    float arr[3] {1.1, 2.2, 3.3};
    PRINTLN("Array can be also printed like this", LOG_AS_ARR(arr, 3));

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
    // Log containers
    std::vector<int> vs {1, 2, 3};
    std::deque<float> ds {1.1, 2.2, 3.3};
    std::map<String, int> ms {{"one", 1}, {"two", 2}, {"three", 3}};
    PRINTLN("Containers can also be printed like", vs, ds, ms);
#endif

    delay(1000);

    // You can also use assert
    // If assertion failed, Serial endlessly prints message
    int x = 1;
    // ASSERT(x != 1);
    // You can also use assert with messages by ASSERTM macro
    ASSERTM(x != 1, "This always fails");
}
```

`Serial` Output

```
DebugLog can print variable args: 1 2.20 three => like this
current log level is 3
[ERROR] basic.ino L.26 setup : this is error log
[WARN] basic.ino L.27 setup : this is warn log
[INFO] basic.ino L.28 setup : this is info log
Array can be also printed like this [1.10, 2.20, 3.30]
Containers can also be printed like [1, 2, 3] [1.10, 2.20, 3.30] {one:1, three:3, two:2}
[ASSERT] basic.ino 51 setup : x != 1 => This always fails
```

## Output Log to both Serial and File

```C++
// You can also set default file level by defining macro (default: ERROR)
// #define DEBUGLOG_DEFAULT_FILE_LEVEL_WARN

// if you want to use standard SD library
#include <SD.h>
#define fs SD

// If you want to use SdFat
// #include <SdFat.h>
// SdFat fs;
// SdFatSdio fs;

// If you want use SPIFFS (ESP32) or other FileSystems
// #include <SPIFFS.h>
// #define fs SPIFFS

// after that, include DebugLog.h
#include <DebugLog.h>

void setup() {
    if (fs.begin()) {
        String filename = "log.txt";

        // Set file system to save every log automatically
        LOG_ATTACH_FS_AUTO(fs, filename, FILE_WRITE);

        // Set file system to save log manually
        // LOG_ATTACH_FS_MANUAL(fs, filename, FILE_WRITE);
    }

    // Apart from the log level to be displayed,
    // you can set the log level to be saved to a file (Default is DebugLogLevel::LVL_ERROR)
    // LOG_FILE_SET_LEVEL(DebugLogLevel::LVL_INFO);

    // If LOG_ATTACH_FS_AUTO is used, logs will be automatically saved to SD
    LOG_ERROR("error!");

    // PRINT_FILE and PRINTLN_FILE is not affected by file_level (always visible)
    // PRINT_FILE and PRINTLN_FILE is not displayed to Serial
    PRINT_FILE("DebugLog", "can print variable args: ");
    PRINTLN_FILE(1, 2.2, "three", "=> like this");

    // Apart from the log level to be displayed,
    // you can set the log level to be saved to a file (Default is DebugLogLevel::LVL_ERROR)
    // LOG_FILE_SET_LEVEL(DebugLogLevel::LVL_INFO);

    // The default log_leval is DebugLogLevel::LVL_INFO
    // 0: NONE, 1: ERROR, 2: WARN, 3: INFO, 4: DEBUG, 5: TRACE
    PRINTLN_FILE("current log level is", (int)LOG_FILE_GET_LEVEL());

    // LOG_XXXX outpus both Serial and File based on log_level and file_level
    // The default log_leval is DebugLogLevel::LVL_INFO
    // The default file_leval is DebugLogLevel::LVL_ERROR
    LOG_ERROR("this is error log");  // printed to both Serial and File
    LOG_WARN("this is warn log");    // won't be saved but printed
    LOG_INFO("this is info log");    // won't be saved but printed
    LOG_DEBUG("this is debug log");  // won't be printed
    LOG_TRACE("this is trace log");  // won't be printed

    // Log array
    float arr[3] {1.1, 2.2, 3.3};
    PRINTLN_FILE("Array can be also printed like this", LOG_AS_ARR(arr, 3));

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
    // Log containers
    std::vector<int> vs {1, 2, 3};
    std::deque<float> ds {1.1, 2.2, 3.3};
    std::map<String, int> ms {{"one", 1}, {"two", 2}, {"three", 3}};
    PRINTLN_FILE("Containers can also be printed like", vs, ds, ms);
#endif

    delay(1000);

    // You can also use assert
    // If assertion failed, suspend program after prints message and close files
    int x = 1;
    // ASSERT(x != 1);
    // You can also use assert with messages by ASSERTM macro
    ASSERTM(x != 1, "This always fails");

    // If LOG_ATTACH_FS_MANUAL is used, you should manually save logs
    // however this is much faster than auto save (saving takes few milliseconds)
    // LOG_FILE_FLUSH(); // manually save to SD card and continue logging
    LOG_FILE_CLOSE(); // flush() and finish logging (ASSERT won't be saved to SD)
}
```

`Serial` Output

```
[ERROR] log_to_file.ino L.97 setup : this is error log
[WARN] log_to_file.ino L.98 setup : this is warn log
[INFO] log_to_file.ino L.99 setup : this is info log
[ASSERT] log_to_file.ino 122 setup : x != 1 => This always fails
```

`File` Output

```
DebugLog can print variable args: 1 2.20 three => like this
current log level is 1
[ERROR] log_to_file.ino L.97 setup : this is error log
Array can be also printed like this [1.10, 2.20, 3.30]
Containers can also be printed like [1, 2, 3] [1.10, 2.20, 3.30] {one:1, three:3, two:2}
[ASSERT] log_to_file.ino 122 setup : x != 1 => This always fails
```

## Control Log Level Scope

You can control the scope of `DebugLog` by including following header files.

- `DebugLogEnable.h`
- `DebugLogDisable.h`
- `DebugLogRestoreState.h`

After including `DebugLogEnable.h` or `DebugLogDisable.h`, macros are enabled/disabled.
Finally you should include `DebugLogRestoreState.h` to restore the previous state.
Please see practical example `examples/control_scope` for details.

```C++
#define DEBUGLOG_DISABLE_LOG
#include <DebugLog.h>

// here is release mode (disable DebugLog)

#include <DebugLogEnable.h>

// here is debug mode (enable DebugLog)

#include <DebugLogRestoreState.h>

// here is release mode (restored)
```

## Logging APIs for both `Serial` and `File`

### Both `Serial` and `File`

These logging APIs are enabled only if log level is control the visibility.

```C++
#define LOG_ERROR(...)
#define LOG_WARN(...)
#define LOG_INFO(...)
#define LOG_DEBUG(...)
#define LOG_TRACE(...)
```

Assertion suspends program if the condition is `true`.

```C++
#define ASSERT(b)
#define ASSERTM(b, msg)
```

### Log only to Serial

`PRINT` and `PRINTLN` are available in both release and debug mode.

```C++
#define PRINT(...)
#define PRINTLN(...)
```

### Log only to File

`PRINT_FILE` and `PRINTLN_FILE` are available in both release and debug mode.

```C++
#define PRINT_FILE(...)
#define PRINTLN_FILE(...)
```

If you use `LOG_ATTACH_FS_MANUAL`, these macros are used to flush files manually.

```C++
// Arduino Only (Manual operation)
#define LOG_FILE_FLUSH()
#define LOG_FILE_CLOSE()
```

## Log Options

### Log Option APIs

```C++
#define LOG_AS_ARR(arr, size)
#define LOG_GET_LEVEL()
#define LOG_SET_LEVEL(level)
#define LOG_SET_OPTION(file, line, func)
#define LOG_SET_DELIMITER(delim)
#define LOG_SET_BASE_RESET(b)
// Arduino Only
#define LOG_ATTACH_SERIAL(serial)
#define LOG_FILE_IS_OPEN()
#define LOG_FILE_GET_LEVEL()
#define LOG_FILE_SET_LEVEL(lvl)
#define LOG_ATTACH_FS_AUTO(fs, path, mode)
#define LOG_ATTACH_FS_MANUAL(fs, path, mode)
```

### Log Level

```C++
enum class DebugLogLevel {
    NONE  = 0,
    ERROR = 1,
    WARN  = 2,
    INFO  = 3,
    DEBUG = 4,
    TRACE = 5
};
```

### Log Base

```C++
enum class DebugLogBase {
    DEC = 10,
    HEX = 16,
    OCT = 8,
    BIN = 2,  // only for Arduino
};
```

### Log Precision

```C++
enum class LogPrecision {
    ZERO,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
};
```

## Used Inside of

- [ArduinoOSC](https://github.com/hideakitai/ArduinoOSC)
- [MsgPack](https://github.com/hideakitai/MsgPack)
- [TaskManager](https://github.com/hideakitai/TaskManager)
- [ES920](https://github.com/hideakitai/ES920)
- [Sony9PinRemote](https://github.com/hideakitai/Sony9PinRemote)

## License

MIT
