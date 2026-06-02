# FastGhostMouse v0.1.0 [ALPHA] â€” High-Performance Native Overlay Cursor for Java

[![Status](https://img.shields.io/badge/status-v0.1.0-brightgreen.svg)](https://github.com/andrestubbe/FastGhostMouse/releases/tag/v0.1.0)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Java](https://img.shields.io/badge/Java-17+-blue.svg)](https://www.java.com)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010+-lightgrey.svg)]()
[![JitPack](https://img.shields.io/badge/JitPack-ready-green.svg)](https://jitpack.io/#andrestubbe)

---

**âš¡ A lightweight, click-through native overlay module for the FastJava ecosystem. Visualize cursor paths and AI
predictions with zero latency.**

**FastGhostMouse** provides a high-performance, transparent native overlay for visual feedback. Built for bot
visualization, UI debugging, and AI-driven cursor path prediction.

[**Watch the Demo**](https://www.youtube.com/watch?v=VbJ62zXcASs&list=PL-mASGDMkCUqJ0bXAJP28ykqPP9RqMMsA&index=19) 

[![FastKeyboard Showcase](docs/screenshot.png)](https://www.youtube.com/watch?v=VbJ62zXcASs&list=PL-mASGDMkCUqJ0bXAJP28ykqPP9RqMMsA&index=19)

---
## Table of Contents

- [Why FastGhostMouse?](#why-fastghostmouse)
- [Quick Start](#quick-start)
- [Features](#features)
- [Quick Start](#quick-start)
- [Installation](#installation)
- [License](#license)

---

## Why FastGhostMouse?

When building automation bots, AI models, or testing frameworks, visualizing what the software is actually doing is one of the hardest challenges. Standard Java solutions for drawing on the screen suffer from severe limitations:

- **The Focus Stealing Problem**: Standard undecorated `JFrame` overlays often steal window focus from the target application, immediately breaking the bot's interactions.
- **Heavy Rendering Pipelines**: Relying on full-blown UI frameworks to draw a simple cursor trail consumes massive CPU/GPU resources, degrading the performance of the game or app you are trying to automate.
- **Input Blocking**: Most overlay solutions accidentally intercept mouse clicks or keyboard events, preventing the user (or the bot) from interacting with the UI beneath the overlay.

**FastGhostMouse** bypasses these limitations by hooking directly into the Windows OS rendering pipeline:

- **True Click-Through (`WS_EX_TRANSPARENT`)**: Built on native JNI bindings, the overlay is mathematically invisible to mouse and keyboard events. You can click right through it.
- **Zero-Focus Disruption (`WS_EX_NOACTIVATE`)**: The overlay is guaranteed to never steal focus from your active game or application.
- **Featherweight Footprint**: Optimized to draw simple geometry (like an AI cursor trail or prediction nodes) with near-zero latency and negligible CPU cost.

---

## Quick Start

```java
import fastghostmouse.FastGhostMouse;

public class Example {
    public static void main(String[] args) throws Exception {
        FastGhostMouse ghost = new FastGhostMouse();
        
        // 1. Initialize and spawn the click-through cursor at screen center
        ghost.useAsSecondaryMouse(960, 540, 0);
        
        // 2. Smoothly animate the cursor to a new location
        // The internal engine calculates the delta time and interpolates the movement
        ghost.moveTo(1500, 200);
        
        Thread.sleep(1000);
        
        // 3. Move again with a customized smoothing factor
        ghost.setSmoothing(0.05f); // Slower, heavier movement
        ghost.moveTo(100, 800);
    }
}
```

---


## Features

- **ðŸ–±ï¸ Ghost Cursor**: Hardware-accelerated, click-through overlay cursor.
- **âœ¨ Smooth Paths**: Native DirectX rendering for flicker-free path visualization.
- **ðŸ“¦ Zero Latency**: Bypasses the Java Swing/AWT event thread.
- **ðŸš€ Click-Through**: Completely focus-agnostic native window.

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

1. ðŸ“¦ **[fastghostmouse-v0.1.0.jar](https://github.com/andrestubbe/FastGhostMouse/releases/download/v0.1.0/fastghostmouse-v0.1.0.jar)** (The Core Library)
2. ðŸ“¦ **[fastcore-v0.1.0.jar](https://github.com/andrestubbe/FastCore/releases/download/v0.1.0/fastcore-v0.1.0.jar)** (The Mandatory Native JNI Loader)

---

## Documentation

* **[COMPILE.md](docs/COMPILE.md)**: Full compilation guide (Maven Build Setup).
* **[REFERENCE.md](docs/REFERENCE.md)**: Exhaustive catalog of timeline strategies and engine architecture.
* **[PHILOSOPHIE.md](docs/PHILOSOPHIE.md)**: Zero-allocation and low-overhead processing designs.
* **[ROADMAP.md](docs/ROADMAP.md)**: Planned milestone features and performance extensions.

---

## Platform Support

| Platform      | Status            |
|---------------|-------------------|
| Windows 10/11 | âœ… Fully Supported |
| Linux         | ðŸš§ Planned        |
| macOS         | ðŸš§ Planned        |

---

## Documentation

* **[COMPILE.md](docs/COMPILE.md)**: Full compilation guide (MSVC C++17 build chain + JNI Setup).
* **[REFERENCE.md](docs/REFERENCE.md)**: Exhaustive catalog of native overlay features and click-through mechanics.
* **[PHILOSOPHIE.md](docs/PHILOSOPHIE.md)**: Zero-latency rendering and focus-agnostic processing designs.
* **[ROADMAP.md](docs/ROADMAP.md)**: Planned milestone features and performance extensions.

---

## License

MIT License â€” See [LICENSE](LICENSE) file for details.

---

## Related Projects

- [FastFileIndex](https://github.com/andrestubbe/FastFileIndex) â€” Ultra-fast filesystem scanner
- [FastTheme](https://github.com/andrestubbe/FastTheme) â€” High-performance native window styling
- [FastThumb](https://github.com/andrestubbe/FastThumb) â€” Native Shell Image Engine

---

**Part of the FastJava Ecosystem** â€” *Making the JVM faster. Small package. Maximum speed. Zero bloat. ðŸš€ðŸ“‹*


