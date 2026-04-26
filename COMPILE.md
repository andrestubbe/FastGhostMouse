# Compile Guide

## Requirements

- Java JDK 17+
- Visual Studio Build Tools (for C++ compilation)
- Maven 3.6+

## Native Compilation

### Windows

```bash
compile.bat
```

This script:
1. Loads Visual Studio Build Tools environment
2. Compiles C++ source to DLL
3. Copies DLL to resources directory

### Manual Compilation

```bash
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
cl.exe /LD /Fe:build\ghostmouse.dll native\ghostmouse.cpp /I"C:\Program Files\Java\jdk-25\include" /I"C:\Program Files\Java\jdk-25\include\win32" user32.lib d2d1.lib dwrite.lib ole32.lib /link /DEF:native\ghostmouse.def
copy build\ghostmouse.dll src\main\resources\native\
```

## Maven Build

```bash
mvn clean package
```

This creates:
- `target/ghostmouse-1.0.0.jar` - Standard JAR
- `target/ghostmouse-1.0.0-jar-with-dependencies.jar` - FatJAR with all dependencies and native DLL

## Running

```bash
java -jar target/ghostmouse-1.0.0-jar-with-dependencies.jar
```
