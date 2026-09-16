# FastGhostMouse 0.1.0 [ALPHA-2026-05-23]: High-Performance Native Overlay Cursor for Java

[![Status](https://img.shields.io/badge/status-0.1.0-brightgreen.svg)](https://github.com/andrestubbe/FastGhostMouse/releases/tag/0.1.0)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Java](https://img.shields.io/badge/Java-21+-blue.svg)](https://www.java.com)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010+-lightgrey.svg)]()
[![JitPack](https://img.shields.io/badge/JitPack-0.1.0-green.svg)](https://jitpack.io/#andrestubbe/FastGhostMouse)

---

**⚡ Hardware-accelerated click-through ghost cursor overlay, smooth physics interpolation, and dynamic AI status labeling for Java.**

**FastGhostMouse** provides an ultra-lightweight, hardware-accelerated ghost cursor overlay powered directly by **[FastOverlay](https://github.com/andrestubbe/FastOverlay)** (DirectComposition / DWM) and **[FastCore](https://github.com/andrestubbe/FastCore)**. Designed for AI agent trajectory visualization, bot debugging, and UI automation telemetry with zero focus theft (`WS_EX_NOACTIVATE`), true OS-level click-through (`WS_EX_TRANSPARENT`), and zero AWT Event Dispatch Thread (EDT) lag.

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

## Why FastGhostMouse?

| Feature | Standard Java (`JFrame` / Swing) | FastGhostMouse |
|:---|:---|:---|
| **Click-Through** | Intercepts mouse events or requires complex OS hacks | True Win32 `WS_EX_TRANSPARENT` pass-through |
| **Focus Handling** | Steals window focus, interrupting games and target apps | True `WS_EX_NOACTIVATE`: zero focus theft |
| **Rendering Engine** | AWT EDT single-threaded pipeline with GC stalls | DirectComposition GPU surface via `FastOverlay` |
| **Animation Latency** | High jitter and frame drops under CPU load | Smooth 120 FPS physics loop on lightweight worker thread |
| **Cursor Capture** | Manual icon extraction with GDI overhead | Direct Win32 `GetCursorInfo` capture via JNI |
| **Ecosystem Synergy** | Standalone UI container | Native bridge to `FastRobot` & `FastOverlay` |

---

## API Reference

| Method | Signature | Description | Subsystem Call |
|:---|:---|:---|:---|
| `init(startX, startY)` | `void` | Spawns dual `FastOverlayWindow` instances and starts the 120 FPS animation loop. | `FastOverlay.createWindow` |
| `useAsSecondaryMouse(x, y, index)` | `void` | Spawns cursor, captures current OS hardware cursor, and displays overlay. | `captureSystemCursor()`, `show()` |
| `moveTo(targetX, targetY)` | `void` | Sets destination coordinates for smooth physics-based cursor motion. | Delta-time Euler interpolation |
| `setCursorPositionImmediate(x, y)` | `void` | Instantly sets overlay position without animation delay. | `FastOverlayWindow.setPosition` |
| `setSmoothing(factor)` | `void` | Sets smoothing factor for cursor momentum (default: `0.12f`). | Physics coefficient |
| `setTextOffset(dx, dy)` | `void` | Sets pixel offset for the state label window relative to cursor tip. | Win32 layout translation |
| `setCursorImage(img)` | `void` | Applies custom `BufferedImage` icon to the cursor overlay surface. | `FastOverlayWindow.updateImage` |
| `setTextImage(img)` | `void` | Applies custom `BufferedImage` state label next to cursor. | `FastOverlayWindow.updateImage` |
| `captureSystemCursor()` | `void` | Captures active Windows OS cursor shape via native Win32 APIs. | `captureSystemCursorNative` (JNI) |
| `setSystemCursorVisible(visible)` | `void` | Dynamically shows or hides the OS cursor via Win32 `SetSystemCursor`. | `setSystemCursorVisible` (JNI) |
| `show()` / `hide()` | `void` | Toggles overlay visibility without destroying native handles. | `FastOverlayWindow` show/hide |
| `dispose()` | `void` | Stops animation thread, releases overlay windows, and restores OS cursor. | Handle cleanup |

---

## Technical Demos & Benchmarks

| Case | Java Example | Launcher | Description |
|:---|:---|:---|:---|
| **Interactive Window Centering Demo** | [Demo.java](examples/Demo/src/main/java/fastghostmouse/demo/Demo.java) | `run-demo.bat` | Interactive bot showcasing auto-repositioning, smooth dragging, and resize recovery. |
| **High-Throughput JMH Benchmarks** | [Benchmark.java](examples/Benchmark/src/main/java/fastghostmouse/benchmark/Benchmark.java) | `run-benchmark.bat` | JMH throughput measurements for motion updates, immediate positioning, and state tagging. |

---

## Installation

### Option 1: Maven (Recommended)

Add the JitPack repository and dependencies to your `pom.xml`:

```xml
<repositories>
    <repository>
        <id>jitpack.io</id>
        <url>https://jitpack.io</url>
    </repository>
</repositories>

<dependencies>
    <!-- FastGhostMouse Library -->
    <dependency>
        <groupId>com.github.andrestubbe</groupId>
        <artifactId>FastGhostMouse</artifactId>
        <version>0.1.0</version>
    </dependency>

    <!-- Required DirectComposition Overlay Engine -->
    <dependency>
        <groupId>com.github.andrestubbe</groupId>
        <artifactId>FastOverlay</artifactId>
        <version>0.1.0</version>
    </dependency>

    <!-- Required Native Loader Substrate -->
    <dependency>
        <groupId>com.github.andrestubbe</groupId>
        <artifactId>FastCore</artifactId>
        <version>0.1.0</version>
    </dependency>
</dependencies>
```

### Option 2: Gradle

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

---

## Documentation

* **[REFERENCE.md](docs/REFERENCE.md)**: Native overlay architecture, Win32 styles, and API signatures.
* **[PHILOSOPHY.md](docs/PHILOSOPHY.md)**: Core design tenets, focus isolation, and zero-EDT mechanics.
* **[ROADMAP.md](docs/ROADMAP.md)**: Development milestones, spline paths, and multi-cursor support.
* **[CHANGELOG.md](docs/CHANGELOG.md)**: Release history and version updates.
* **[COMPILE.md](docs/COMPILE.md)**: Native C++ build instructions (MSVC build tools).

---

## Platform Support

| Platform | Architecture | Status | Engine / Pipeline |
|:---|:---|:---|:---|
| **Windows 10 / 11** | `x64` | ✅ Fully Supported | Win32 Layered Windows + DirectComposition (`FastOverlay`) |
| **Windows 10 / 11** | `arm64` | 🚧 Planned | Native Windows on ARM DirectComposition pipeline |
| **Linux** | `x64` | 🚧 Planned | X11 Composite / Wayland Subsurface transparent overlay |
| **macOS** | `x64` / `arm64` | 🚧 Planned | CoreGraphics / Metal Overlay Window |

---

## Ecosystem Integration

```
+-------------------------------------------------------------+
|                 FastGhostMouse Application                  |
+-------------------------------------------------------------+
                              |
       +----------------------+----------------------+
       |                                             |
       v                                             v
+-----------------------------+       +-----------------------------+
|         FastOverlay         |       |          FastRobot          |
|  (DirectComposition Canvas) |       |   (Native Input Injection)  |
+-----------------------------+       +-----------------------------+
       |                                             |
       +----------------------+----------------------+
                              |
                              v
+-------------------------------------------------------------+
|                          FastCore                           |
|               (Native Library JNI Loader)                   |
+-------------------------------------------------------------+
```

---

## License

MIT License. See [LICENSE](LICENSE) for details.

Part of the **FastJava** ecosystem. Clean architectures, zero EDT lag, maximum speed.
