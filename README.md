# FastGhostMouse

[![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![Java](https://img.shields.io/badge/Java-17+-blue.svg)](https://www.java.com)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010+-lightgrey.svg)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![JitPack](https://jitpack.io/v/andrestubbe/FastGhostMouse.svg)](https://jitpack.io/#andrestubbe/FastGhostMouse)

## Table of Contents

- [Description](#description)
- [Quick Start](#quick-start)
- [Key Features](#key-features)
- [Installation](#installation)
- [Building from Source](#building-from-source)
- [Platform Support](#platform-support)
- [License](#license)
- [Related Projects](#related-projects)

## Description

FastGhostMouse is a high-performance native ghost cursor overlay for Java applications. It provides a smooth, animated cursor that can be displayed independently of the real system cursor, with support for text display in speech bubbles.

The library uses DirectComposition for GPU-accelerated rendering, ensuring smooth animations at approximately 60fps. The ghost cursor appears as a glowing white circle with optional text in a rounded speech bubble, making it ideal for tutorials, demos, and visual feedback systems.

**Key Technologies:**
- DirectComposition for GPU-accelerated rendering
- Direct2D for high-quality graphics
- JNI with RegisterNatives for robust native method binding
- Multi-monitor support

## Quick Start

### Maven

Add the JitPack repository to your `pom.xml`:

```xml
<repositories>
    <repository>
        <id>jitpack.io</id>
        <url>https://jitpack.io</url>
    </repository>
</repositories>
```

Add the dependency:

```xml
<dependency>
    <groupId>com.github.andrestubbe</groupId>
    <artifactId>fastghostmouse</artifactId>
    <version>1.0.0</version>
</dependency>
```

### Example Usage

```java
import fastghostmouse.FastGhostMouse;

public class Example {
    public static void main(String[] args) throws InterruptedException {
        FastGhostMouse ghost = new FastGhostMouse();

        // Initialize the overlay
        ghost.init(100, 100);

        // Set text to display
        ghost.setText("Hello, World!");

        // Show the ghost cursor
        ghost.show();

        // Move the cursor smoothly
        ghost.moveTo(500, 300);
        Thread.sleep(1000);

        // Hide and cleanup
        ghost.hide();
        ghost.dispose();
    }
}
```

## Key Features

- **Smooth Animation**: Interpolated cursor movement at approximately 60fps for fluid motion
- **Text Display**: Optional speech bubble with customizable text above the cursor
- **Transparent Overlay**: Window is transparent to mouse events and doesn't steal focus
- **Hardware Accelerated**: Uses Direct2D for high-performance rendering
- **Glowing Effect**: Multi-layered glow effect for visual appeal
- **Thread-Safe**: All operations are protected by mutex locks for safe concurrent access
- **Easy to Use**: Simple API with just 6 methods

## Installation

### Maven

```xml
<dependency>
    <groupId>com.github.andrestubbe</groupId>
    <artifactId>fastghostmouse</artifactId>
    <version>1.0.0</version>
</dependency>
```

### Gradle

```gradle
repositories {
    maven { url 'https://jitpack.io' }
}

dependencies {
    implementation 'com.github.andrestubbe:fastghostmouse:1.0.0'
}
```

### JitPack

Visit [JitPack](https://jitpack.io/#andrestubbe/FastGhostMouse) for the latest version and build status.

## Building from Source

### Prerequisites

- Java JDK 17+
- Visual Studio Build Tools (for C++ compilation)
- Maven 3.6+

### Native Compilation

#### Windows

```bash
compile.bat
```

This script:
1. Loads Visual Studio Build Tools environment
2. Compiles C++ source to DLL
3. Copies DLL to resources directory

#### Manual Compilation

```bash
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
cl.exe /LD /Fe:build\fastghostmouse.dll native\fastghostmouse.cpp /I"C:\Program Files\Java\jdk-25\include" /I"C:\Program Files\Java\jdk-25\include\win32" user32.lib d2d1.lib dwrite.lib ole32.lib /link /DEF:native\fastghostmouse.def
copy build\fastghostmouse.dll src\main\resources\native\
```

### Maven Build

```bash
mvn clean package
```

This creates:
- `target/fastghostmouse-1.0.0.jar` - Standard JAR
- `target/fastghostmouse-1.0.0-jar-with-dependencies.jar` - FatJAR with all dependencies and native DLL

### Running

```bash
java -jar target/fastghostmouse-1.0.0-jar-with-dependencies.jar
```

## Platform Support

- **Windows 10+**: Fully supported with DirectComposition rendering
- **Linux**: Not supported (requires DirectComposition)
- **macOS**: Not supported (requires DirectComposition)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Related Projects

- [FastCore](https://github.com/andrestubbe/fastcore) - Core utilities for FastJava projects
- [FastClipboard](https://github.com/andrestubbe/fastclipboard) - Native clipboard access
- [FastHotkey](https://github.com/andrestubbe/fasthotkey) - Global hotkey registration
- [FastRobot](https://github.com/andrestubbe/fastrobot) - Enhanced robot automation

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Acknowledgments

- Built with [FastCore](https://github.com/andrestubbe/fastcore)
- Uses Direct2D for hardware-accelerated rendering
- Part of the [FastJava](https://github.com/andrestubbe) project family
