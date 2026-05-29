# FastGhostMouse v0.1.0 [ALPHA] — High-Performance Native Overlay Cursor for Java

[![Status](https://img.shields.io/badge/status-v0.1.0-brightgreen.svg)](https://github.com/andrestubbe/FastGhostMouse/releases/tag/v0.1.0)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Java](https://img.shields.io/badge/Java-17+-blue.svg)](https://www.java.com)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010+-lightgrey.svg)]()
[![JitPack](https://img.shields.io/badge/JitPack-ready-green.svg)](https://jitpack.io/#andrestubbe)

---

**⚡ A lightweight, click-through native overlay module for the FastJava ecosystem. Visualize cursor paths and AI
predictions with zero latency.**

**FastGhostMouse** provides a high-performance, transparent native overlay for visual feedback. Built for bot
visualization, UI debugging, and AI-driven cursor path prediction.

---

[![FastKeyboard Showcase](docs/screenshot.png)](https://www.youtube.com/watch?v=9zGz6Sdnu-c)

---
## Table of Contents

- [Quick Start](#quick-start)
- [Features](#features)
- [Quick Start](#quick-start)
- [Installation](#installation)
- [License](#license)

---

## Quick Start

```java
import fastghostmouse.FastGhostMouse;

public class Example {
    public static void main(String[] args) throws InterruptedException {
        // Initialize the transparent overlay
        FastGhostMouse ghost = new FastGhostMouse();
        ghost.useAsSecondaryMouse(100, 100, 1.0f);
        
        // Attach a dynamic text label directly to the cursor
        ghost.setStatusText("AI is clicking...");
        
        // Smoothly animate the cursor to screen coordinates X:800, Y:450
        ghost.moveTo(800, 450);
        
        Thread.sleep(3000); // Keep it visible for a moment
        ghost.dispose();
    }
}
```

---


## Features

- **🖱️ Ghost Cursor**: Hardware-accelerated, click-through overlay cursor.
- **✨ Smooth Paths**: Native DirectX rendering for flicker-free path visualization.
- **📦 Zero Latency**: Bypasses the Java Swing/AWT event thread.
- **🚀 Click-Through**: Completely focus-agnostic native window.

---

## Installation

### Option 1: Maven (Recommended)

Add the JitPack repository and the dependencies to your `pom.xml`:

```xml
<repositories>
    <repository>
        <id>jitpack.io</id>
        <url>https://jitpack.io</url>
    </repository>
</repositories>
<dependencies>
   <dependency>
       <groupId>com.github.andrestubbe</groupId>
       <artifactId>fastghostmouse</artifactId>
       <version>v0.1.0</version>
   </dependency>
   <dependency>
       <groupId>com.github.andrestubbe</groupId>
       <artifactId>fastcore</artifactId>
       <version>v0.1.0</version>
   </dependency>
</dependencies>
```

### Option 2: Gradle (via JitPack)

```groovy
repositories {
    maven { url 'https://jitpack.io' }
}
dependencies {
    implementation 'com.github.andrestubbe:fastghostmouse:v0.1.0'
    implementation 'com.github.andrestubbe:fastcore:v0.1.0'
}
```

### Option 3: Direct Download (No Build Tool)

Download the latest JARs directly to add them to your classpath:

1. 📦 *
   *[fastghostmouse-v0.1.0.jar](https://github.com/andrestubbe/FastGhostMouse/releases/download/v0.1.0/fastghostmouse-v0.1.0.jar)
   ** (The Core Library)
2. ⚙️ **[fastcore-v0.1.0.jar](https://github.com/andrestubbe/FastCore/releases/download/v0.1.0/fastcore-v0.1.0.jar)** (
   The Mandatory Native Loader)

---

## License

MIT License — See [LICENSE](LICENSE) file for details.

---

## Related Projects

- [FastTween](https://github.com/andrestubbe/FastTween) — Zero overhead pool-based tweening
- [FastAnimation](https://github.com/andrestubbe/FastAnimation) — Zero overhead timeline orchestration
- [FastDWM](https://github.com/andrestubbe/FastDWM) — Native Desktop Window Manager API
- [FastCore](https://github.com/andrestubbe/FastCore) — Native JNI Loader and Utilities
- [FastTheme](https://github.com/andrestubbe/FastTheme) — High-performance native window styling

---

**Part of the FastJava Ecosystem** — *Making the JVM faster. Small package. Maximum speed. Zero bloat. 🚀📋*


