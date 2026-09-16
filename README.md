# FastGhostMouse 0.1.0 [ALPHA-2026-05-23]: High-Performance Native Ghost Cursor Overlay for Java

[![Status](https://img.shields.io/badge/status-0.1.0-brightgreen.svg)](https://github.com/andrestubbe/FastGhostMouse/releases/tag/0.1.0)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Java](https://img.shields.io/badge/Java-17+-blue.svg)](https://www.java.com)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010+-lightgrey.svg)]()
[![JitPack](https://img.shields.io/badge/JitPack-0.1.0-green.svg)](https://jitpack.io/#andrestubbe/FastGhostMouse)

---

**⚡ Hardware-accelerated click-through ghost cursor overlay, smooth physics interpolation, and dynamic AI status labeling for Java.**

**FastGhostMouse** provides an ultra-lightweight native ghost cursor overlay powered directly by **[FastOverlay](https://github.com/andrestubbe/FastOverlay)** (DirectComposition / DWM) and **[FastCore](https://github.com/andrestubbe/FastCore)**. Designed for AI agent trajectory visualization, bot debugging, and UI automation telemetry with zero focus theft (`WS_EX_NOACTIVATE`), true OS-level click-through (`WS_EX_TRANSPARENT`), and zero AWT Event Dispatch Thread (EDT) lag.

![FastGhostMouse Showcase](docs/screenshot.png)

---

## Quick Start

```java
import fastghostmouse.FastGhostMouse;

public class Demo {
    public static void main(String[] args) throws Exception {
        FastGhostMouse ghost = new FastGhostMouse();

        // 1. Initialize and spawn click-through cursor at screen center
        ghost.useAsSecondaryMouse(960, 540, 0);

        // 2. Smoothly animate the cursor to target coordinates
        ghost.moveTo(1500, 200);

        Thread.sleep(1000);

        // 3. Customize motion smoothing factor (default: 0.12f)
        ghost.setSmoothing(0.05f); // heavier, more deliberate motion
        ghost.moveTo(200, 800);
    }
}
```

---

## Table of Contents

- [Why FastGhostMouse?](#why-fastghostmouse)
- [Quick Start](#quick-start)
- [Key Features](#key-features)
- [Real-World Use Cases](#real-world-use-cases)
- [API Quick Reference](#api-quick-reference)
- [Technical Demos & Benchmarks](#technical-demos--benchmarks)
- [Installation](#installation)
- [Documentation](#documentation)
- [Platform Support](#platform-support)
- [Related Projects](#related-projects)
- [License](#license)

---

## Why FastGhostMouse?

Standard Java GUI toolkits (`JFrame`, Swing undecorated windows) have severe limitations as automation overlays:

1. **Focus Stealing**: Any undecorated `JFrame` typically activates and steals focus from the target game or application, immediately breaking the bot's interaction.
2. **Input Blocking**: Standard Java window handles sit in the Win32 hit-test path and intercept mouse events, making true click-through behavior impossible without native hacks.
3. **AWT EDT Bottleneck**: Driving a cursor overlay at 120 FPS on the Java Event Dispatch Thread introduces severe jitter, micro-stutters, and GC pressure in the same thread serving the rest of the application.

**FastGhostMouse** bypasses all of these by compositing directly via **[FastOverlay](https://github.com/andrestubbe/FastOverlay)** into the Windows DWM pipeline:

| Feature | Standard Java (`JFrame` / Swing) | FastGhostMouse |
|:---|:---|:---|
| **Click-Through** | Intercepts mouse events or requires complex OS hacks | True Win32 `WS_EX_TRANSPARENT` pass-through |
| **Focus Handling** | Steals window focus, interrupting target apps and games | True `WS_EX_NOACTIVATE`: zero focus theft |
| **Rendering Engine** | AWT EDT single-threaded pipeline with GC stalls | DirectComposition GPU surface via `FastOverlay` |
| **Animation Latency** | High jitter and frame drops under CPU load | Smooth 120 FPS physics loop on dedicated worker thread |
| **Cursor Capture** | Manual icon extraction with GDI overhead | Direct Win32 `GetCursorInfo` capture via JNI |
| **Ecosystem Synergy** | Standalone UI container | Native bridge to `FastRobot` & `FastOverlay` |

---

## Key Features

- ⚡ **DirectComposition GPU Compositing**: Two hardware-accelerated `FastOverlayWindow` surfaces for cursor and status label, rendered directly into the Windows DWM pipeline.
- 👻 **True Click-Through & Ghost Mode**: Mouse events fall through to target applications with zero interference via Win32 `WS_EX_TRANSPARENT`.
- 🎯 **Physics-Based Motion Interpolation**: Smooth delta-time Euler integration with configurable momentum (`setSmoothing`) for natural-looking cursor movement at 120 FPS.
- 💬 **Dynamic Status Labels**: Attach live AI state tags, action strings, or telemetry values directly next to the cursor via `setTextImage`.
- 🖼️ **Native Cursor Capture**: Automatically extracts the active Windows OS hardware cursor bitmap via Win32 `GetCursorInfo` for seamless ghost cursor appearance.
- 🚀 **Zero AWT EDT Lag**: Animation loop runs on a dedicated thread, never touching the Java Event Dispatch Thread.

---

## Real-World Use Cases

- 🤖 **Autonomous AI Agent Visualization**: Render the ghost cursor along the predicted trajectory of an AI agent, showing planned mouse paths in real time without interfering with the agent's input injection.
- 🕵️ **Bot Debugging & Telemetry**: Attach live state labels ("Clicking Button", "Searching Target") directly next to the injected cursor position for immediate visual debugging.
- 🎮 **Game Automation Telemetry**: Overlay a ghost cursor above high-speed game bots to verify targeting accuracy without disrupting the bot's `FastRobot` input stream.
- 🧪 **UI Test Visualization**: Display the automated test cursor path during desktop automation runs for visual inspection and recording.

---

## API Quick Reference

| Method | Return Type | Description | Docs |
|:---|:---|:---|:---|
| `init(startX, startY)` | `void` | Spawns dual overlay windows and starts the 120 FPS physics animation loop. | [Reference](docs/REFERENCE.md) |
| `useAsSecondaryMouse(x, y, index)` | `void` | Captures OS hardware cursor, initializes overlay, and shows the ghost cursor. | [Reference](docs/REFERENCE.md) |
| `moveTo(targetX, targetY)` | `void` | Sets interpolation destination for smooth physics-based cursor animation. | [Reference](docs/REFERENCE.md) |
| `setCursorPositionImmediate(x, y)` | `void` | Instantly repositions the overlay without animation delay. | [Reference](docs/REFERENCE.md) |
| `setSmoothing(factor)` | `void` | Configures momentum and easing responsiveness (default: `0.12f`). | [Reference](docs/REFERENCE.md) |
| `setTextOffset(dx, dy)` | `void` | Sets pixel offset of the status label window relative to the cursor tip. | [Reference](docs/REFERENCE.md) |
| `setCursorImage(img)` | `void` | Applies a custom `BufferedImage` to the cursor overlay surface. | [Reference](docs/REFERENCE.md) |
| `setTextImage(img)` | `void` | Applies a custom `BufferedImage` status label next to the cursor. | [Reference](docs/REFERENCE.md) |
| `captureSystemCursor()` | `void` | Queries and extracts the current Windows OS hardware cursor via Win32 JNI. | [Reference](docs/REFERENCE.md) |
| `setSystemCursorVisible(visible)` | `void` | Dynamically hides or restores the OS hardware cursor. | [Reference](docs/REFERENCE.md) |
| `show()` / `hide()` | `void` | Toggles overlay visibility without destroying native window handles. | [Reference](docs/REFERENCE.md) |
| `dispose()` | `void` | Stops animation thread, releases overlay windows, and restores OS cursor. | [Reference](docs/REFERENCE.md) |

---

## Technical Demos & Benchmarks

| Case | Java Example | Launcher | Description |
|:---|:---|:---|:---|
| **Interactive Bot Demo** | [Demo.java](examples/Demo/src/main/java/fastghostmouse/demo/Demo.java) | `run-demo.bat` | Interactive bot showcasing auto-repositioning, smooth tween dragging, and resize recovery. |
| **JMH Microbenchmark Suite** | [Benchmark.java](examples/Benchmark/src/main/java/fastghostmouse/benchmark/Benchmark.java) | `run-benchmark.bat` | JMH throughput measurements for motion updates, immediate positioning, and cursor image rendering. |

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
    <!-- FastGhostMouse - Native Ghost Cursor Overlay -->
    <dependency>
        <groupId>com.github.andrestubbe</groupId>
        <artifactId>FastGhostMouse</artifactId>
        <version>0.1.0</version>
    </dependency>

    <!-- FastOverlay - Required DirectComposition Render Engine -->
    <dependency>
        <groupId>com.github.andrestubbe</groupId>
        <artifactId>FastOverlay</artifactId>
        <version>0.1.0</version>
    </dependency>

    <!-- FastCore - Required Native JNI Loader -->
    <dependency>
        <groupId>com.github.andrestubbe</groupId>
        <artifactId>FastCore</artifactId>
        <version>0.1.0</version>
    </dependency>
</dependencies>
```

### Option 2: Gradle (via JitPack)

```groovy
repositories {
    maven { url 'https://jitpack.io' }
}

dependencies {
    implementation 'com.github.andrestubbe:FastGhostMouse:0.1.0'
    implementation 'com.github.andrestubbe:FastOverlay:0.1.0'
    implementation 'com.github.andrestubbe:FastCore:0.1.0'
}
```

### Option 3: Direct Download (No Build Tool)

Download the release JARs directly from GitHub Releases:

1. 📦 **[FastGhostMouse-0.1.0.jar](https://github.com/andrestubbe/FastGhostMouse/releases/tag/0.1.0)** (Core Ghost Cursor Library)
2. 🎨 **[FastOverlay-0.1.0.jar](https://github.com/andrestubbe/FastOverlay/releases/tag/0.1.0)** (Required DirectComposition Engine)
3. ⚙️ **[FastCore-0.1.0.jar](https://github.com/andrestubbe/FastCore/releases/tag/0.1.0)** (Mandatory Native Loader)

> [!IMPORTANT]
> All three JARs must be on your classpath. `FastOverlay` is not optional: `FastGhostMouse` calls `FastOverlay.initEngine()` on class load.

---

## Documentation

- **[COMPILE.md](docs/COMPILE.md)**: Full native compilation guide (MSVC C++ build chain + JNI setup).
- **[REFERENCE.md](docs/REFERENCE.md)**: Comprehensive API specification, Win32 overlay styles, and ecosystem architecture.
- **[PHILOSOPHY.md](docs/PHILOSOPHY.md)**: Engineering rationale for zero-EDT, zero-focus-theft ghost cursor design.
- **[ROADMAP.md](docs/ROADMAP.md)**: Planned milestones, motion trails, and cross-platform expansion.
- **[CHANGELOG.md](docs/CHANGELOG.md)**: Complete version history and release notes.

---

## Platform Support

| Platform | Architecture | Status | Driver / Subsystem |
|:---|:---:|:---:|:---|
| **Windows 10 / 11** | x64 | ✅ Fully Supported | DirectComposition + Win32 Layered DWM via `FastOverlay` |
| **Linux** | x64 / AArch64 | 🚧 Planned | X11 Composite Extension / Wayland Subsurface |
| **macOS** | Apple Silicon / x64 | 🚧 Planned | CoreGraphics Non-Activating Translucent `NSWindow` |

---

## Related Projects

- **[`FastOverlay`](https://github.com/andrestubbe/FastOverlay)**: High-Performance Native DirectComposition Transparent Overlay API for Java
- **[`FastRobot`](https://github.com/andrestubbe/FastRobot)**: Low-Latency Native Input & Bot Automation Substrate
- **[`FastCore`](https://github.com/andrestubbe/FastCore)**: Native Library Loader & JNI Utilities for Java
- **[`FastScreen`](https://github.com/andrestubbe/FastScreen)**: High-Speed DXGI Screen Capture Engine (240-2000 FPS)
- **[`FastImage`](https://github.com/andrestubbe/FastImage)**: Ultra-Fast Native SIMD Image Processing for Java
- **[`FastTheme`](https://github.com/andrestubbe/FastTheme)**: Native Windows System Theme & Titlebar Dark Mode API

---

## License

MIT License. See [LICENSE](LICENSE) file for details.

---

**Part of the FastJava Ecosystem** — *Making the JVM faster.* 🚀
