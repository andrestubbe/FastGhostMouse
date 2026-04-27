# Compile Guide

## Requirements

- Java JDK 22+ (JDK 25 recommended)
- Visual Studio 2022 Build Tools (with C++ support)
- Maven 3.6+

## Native Compilation

### Windows

Run the provided batch script to compile the C++ source into a native DLL:

```bash
compile.bat
```

This script:
1. Loads the Visual Studio x64 build environment.
2. Compiles `native/FastGhostMouse.cpp` with Direct2D, DirectComposition, and D3D11 support.
3. Copies the resulting `fastghostmouse.dll` to the `src/main/resources/native/` directory for JAR inclusion.

## Maven Build

Generate the project JARs using Maven:

```bash
mvn clean package
```

This creates:
- `target/fastghostmouse-1.0.0.jar` - Standard API JAR.
- `target/fastghostmouse-1.0.0-jar-with-dependencies.jar` - FatJAR containing the native DLL and the FastCore dependency.

## Running

You can run the included demo using the following command:

```bash
java -cp "bin;target\fastghostmouse-1.0.0-jar-with-dependencies.jar" "-Djava.library.path=." fastghostmouse.demo.Demo
```
