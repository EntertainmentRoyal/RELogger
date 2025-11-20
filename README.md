# RELogger  
### A Lightweight, Cross-Platform, Thread-Safe Logging Framework  

**Author:** *Jayansh Devgan*  
**Date:** *October 10, 2025*  
**Version:** 1.0.0  

---

## Overview  

**RELogger** is a **cross-language logging library** designed for **C/C++**, **C#**, and **Java** projects.  
It provides **color-coded**, **thread-safe**, and **highly readable logs** with support for both **console** and **file output**.  

Built with performance and developer experience in mind, RELogger helps you:
- Debug complex systems easily  
- Trace execution flow precisely  
- Maintain structured and timestamped logs across your entire codebase  

---

## Features  

| Feature | Description |
|----------|-------------|
| **Multi-language support** | Consistent logging API in **C++**, **C#**, and **Java** |
| **Thread-safe** | Protects log writes in multi-threaded applications |
| **Timestamps** | Each log includes a `HH:mm:ss` formatted timestamp |
| **Color-coded output** | Easy-to-read console messages |
| **Optional file logging** | Save logs to a file with flush-on-write |
| **Log level filtering** | Filter messages by severity |
| **Configurable** | Supports easy initialization and clean shutdown |
| **Cross-platform** | Runs seamlessly on Windows, Linux, and macOS |

---

## Log Levels  

| Level | Description | Console Color |
|--------|--------------|----------------|
| `TRACE` | Detailed internal state for debugging | Gray / White |
| `DEBUG` | Developer-level diagnostic info | Cyan |
| `INFO`  | General system information | Green |
| `WARN`  | Non-critical warning | Yellow |
| `ERROR` | Recoverable error | Red |
| `FATAL` | Severe error or crash | White on Red |

---

## Repository Structure  
```
RELogger/
│
├── c/ → C Implementation
│ ├── RELogger/relogger.h
│ └── RELogger/relogger.c
├── cpp/ → C++ Implementation
│ ├── RELogger/relogger.h
│ └── RELogger/relogger.cpp
│
├── CSharp/ → C# Implementation
│ ├── RELogger/RELogger.cs
│
└── Java/ → Java Implementation
├── RELogger/RELogger.java
└── RELogger/LogLevel.java
```
---

# C/C++ Implementation

### Files
- `relogger.h` — Public API and enum definitions  
- `relogger.cpp/c` — Implementation with thread safety and color-coded output  

---

### Initialization and Usage

```cpp
#include "relogger.h"

int main() {
    RELogger::Init("app.log");                // Optional: specify file output
    RELogger::SetLevel(LogLevel::Info);       // Minimum log level to display

    RELogger::Log(LogLevel::Info, "App started successfully", __FILE__, __LINE__, __func__);
    RELogger::Log(LogLevel::Warn, "Low memory warning", __FILE__, __LINE__, __func__);
    RELogger::Log(LogLevel::Error, "Critical system failure", __FILE__, __LINE__, __func__);

    RELogger::Shutdown();                     // Flush and close log file
    return 0;
}
```
 Example Output (Console)
```output
[12:01:32] INFO  main.cpp:10 (main) - App started successfully
[12:01:33] WARN  main.cpp:11 (main) - Low memory warning
[12:01:34] ERROR main.cpp:12 (main) - Critical system failure
```
Color Representation (Terminal)
```output
\033[32m[12:01:32] INFO  ...\033[0m       → Green
\033[33m[12:01:33] WARN  ...\033[0m       → Yellow
\033[31m[12:01:34] ERROR ...\033[0m       → Red
```
C# Implementation

Files

Logger.cs — Main logging class

LogLevel.cs — Enum of log levels

Initialization and Usage
```C#
using RE;

class Program
{
    static void Main()
    {
        Logger.Init("app.log");              // Initialize optional file logging
        Logger.SetLevel(LogLevel.Info);      // Minimum level to log

        Logger.Log(LogLevel.Info, "Application initialized successfully!");
        Logger.Log(LogLevel.Warn, "Disk space running low.");
        Logger.Log(LogLevel.Error, "Unhandled exception occurred.");

        Logger.Shutdown();                   // Clean shutdown
    }
}
```
Example Output (Console)
```output
[12:01:32] INFO  Program.cs:12 (Main) - Application initialized successfully!
[12:01:33] WARN  Program.cs:13 (Main) - Disk space running low.
[12:01:34] ERROR Program.cs:14 (Main) - Unhandled exception occurred.
```
Color Representation (Terminal)
```output
INFO  → Green
WARN  → Yellow
ERROR → Red
FATAL → White text on Red background
```
Notes

Uses [CallerFilePath], [CallerLineNumber], and [CallerMemberName] for automatic metadata.

Thread-safe using lock blocks.

Supports both console and file outputs.

Colors reset after each line for clean console display.

Java Implementation

Files

RELogger.java — Core logger

LogLevel.java — Enum definition

Initialization and Usage
```Java
import RELogger.*;

public class Main {
    public static void main(String[] args) {
        RELogger.init("app.log");                   // Optional file logging
        RELogger.setLevel(LogLevel.INFO);           // Minimum log level

        RELogger.log(LogLevel.INFO, "Server started successfully.");
        RELogger.log(LogLevel.WARN, "API response time degraded.");
        RELogger.log(LogLevel.ERROR, "Database connection failed.");

        RELogger.shutdown();                        // Flush and close file
    }
}
```
Example Output (Console)
```output
[12:01:32] INFO  Main.java:9 (main) - Server started successfully.
[12:01:33] WARN  Main.java:10 (main) - API response time degraded.
[12:01:34] ERROR Main.java:11 (main) - Database connection failed.
```
Color Representation (Terminal)
```
TRACE → Gray
DEBUG → Cyan
INFO  → Green
WARN  → Yellow
ERROR → Red
FATAL → White text on Red background
```

Configuration Summary
Method	Description	Example
```
Init(String path)	Initializes logger (with optional file path)	RELogger::Init("log.txt")
Shutdown()	Flushes and closes log streams	RELogger::Shutdown()
SetLevel(LogLevel level)	Sets minimum log severity	RELogger::SetLevel(LogLevel::Warn)
GetLevel()	Returns current log level	auto lvl = RELogger::GetLevel()
Log(LogLevel, message, file, line, func)	Logs message with metadata	RELogger::Log(LogLevel::Error, "Error occurred", __FILE__, __LINE__, __func__)
```
Color Codes Reference
Level	ANSI Code	Color
TRACE	\033[37m	White
DEBUG	\033[36m	Cyan
INFO	\033[32m	Green
WARN	\033[33m	Yellow
ERROR	\033[31m	Red
FATAL	\033[41m	Red Background

Design Principles

Thread Safety: Uses mutex locks / synchronization blocks to prevent race conditions.

Performance: Minimal locking scope, flush-on-write for reliability.

Extensibility: Easily extendable to support different output streams (e.g., network, database).

Portability: Consistent design and behavior across all supported languages.

Example Use Cases

Debugging game engine subsystems
Logging network events or AI behavior trees
Tracking user analytics in real time
Production monitoring or crash reporting
CLI tools and services that need human-readable logs

License

This project is released under the MIT License.
You are free to use, modify, and distribute it with proper attribution.

👤 Author

Jayansh Devgan
🎮 Game Developer • Systems Programmer • Founder of Royal Entertainment

“Precision in logs means clarity in code.”
