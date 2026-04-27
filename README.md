# FastGhostMouse — [ALPHA] High-Performance GPU-Accelerated Secondary Mouse Overlay

**⚡ Ultra-smooth, 120Hz secondary mouse for Java applications — hardware-accelerated via DirectComposition.**

[![Status](https://img.shields.io/badge/status-ALPHA-orange.svg)]()
[![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![Java](https://img.shields.io/badge/Java-17+-blue.svg)](https://www.java.com)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010+-lightgrey.svg)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![JitPack](https://jitpack.io/v/andrestubbe/FastGhostMouse.svg)](https://jitpack.io/#andrestubbe/FastGhostMouse)

FastGhostMouse provides a **high-performance, GPU-accelerated secondary mouse overlay** for Windows. Built for the **FastJava** ecosystem, it enables buttery-smooth (120Hz+) independent cursor visualization, "Butler" assistance bubbles, and AI-driven UI indicators without any flickering or system lag.

---

## Quick Start

### Installation (JitPack)

**Maven:**
```xml
<repositories>
    <repository>
        <id>jitpack.io</id>
        <url>https://jitpack.io</url>
    </repository>
</repositories>

<dependency>
    <groupId>com.github.andrestubbe</groupId>
    <artifactId>FastGhostMouse</artifactId>
    <version>v0.4.0</version>
</dependency>
<dependency>
    <groupId>com.github.andrestubbe</groupId>
    <artifactId>fastcore</artifactId>
    <version>v1.0.0</version>
</dependency>
```

**Gradle:**
```groovy
repositories {
    maven { url 'https://jitpack.io' }
}

dependencies {
    implementation 'com.github.andrestubbe:FastGhostMouse:v0.4.0'
    implementation 'com.github.andrestubbe:fastcore:v1.0.0'
}
```

### Basic Usage — The "Butler" Cursor

```java
FastGhostMouse ghost = new FastGhostMouse();

// 1. Initialize as a Secondary Mouse (cloning system cursor)
ghost.useAsSecondaryMouse(100, 100, 2.0f); // supports 200% scaling

// 2. Add a status bubble via the Universal Visual Painter
ghost.setStatusVisual(140, 50, g -> {
    g.setColor(new Color(30, 30, 30, 220));
    g.fillRoundRect(0, 0, 130, 40, 12, 12);
    g.setColor(Color.WHITE);
    g.drawString("Thinking...", 15, 25);
});

// 3. Move smoothly to a target
ghost.setSmoothing(0.15f); // 120Hz smooth interpolation
ghost.moveTo(800, 450);
ghost.show();
```

---

## Key Features

- **🚀 DirectComposition Visual Tree** — Pure GPU-based rendering for zero-latency transparency.
- **⚡ 120Hz+ High-Speed Loop** — Tuned for modern high-refresh-rate gaming monitors.
- **🎨 Universal Visual Painter** — Render complex Java UI (progress bars, icons) directly next to the cursor via `Graphics2D`.
- **🖱️ System Cursor Replacement** — Ability to hide the real system cursor (`setSystemCursorVisible`) for a full solo ghost experience.
- **📉 Dynamic Smoothing** — Built-in time-based interpolation for fluid, human-like motion.
- **🛠️ Zero Dependencies** — Java 17+ and Windows only (Powered by FastCore).

---

## API Reference

| Method | Description |
|--------|-------------|
| `useAsSecondaryMouse(x, y, scale)` | Clones the system cursor and initializes the overlay. |
| `moveTo(x, y)` | Smoothly interpolates the ghost mouse to a new position. |
| `setSmoothing(factor)` | Adjusts the "snappiness" of the movement (0.0 - 1.0). |
| `setStatusVisual(w, h, painter)` | Renders a custom status bubble using Java `Graphics2D`. |
| `setSystemCursorVisible(visible)` | Hides or restores the actual Windows system cursor. |
| `show() / hide()` | Controls the visibility of the entire overlay. |

---

## Architecture

FastGhostMouse uses a modern **Windows Graphics Stack** to achieve its performance:

```
Java App (Graphics2D)
    ↓ RGBA Pixel Buffer
Direct2D (GPU Bitmap)
    ↓
DirectComposition (Visual Tree)
    ↓ Visual Offsets (SetOffsetX/Y)
Desktop Window Manager (DWM)
```

By moving visuals via **Hardware Offsets** instead of CPU redraws, we achieve perfectly fluid movement even when the system is under heavy load.

---

## Platform Support

| Platform | Status | Notes |
|----------|--------|-------|
| Windows 11 | ✅ Full support | GPU-accelerated DirectComposition |
| Windows 10 | ✅ Full support | GPU-accelerated DirectComposition |
| Linux | ❌ Not planned | |
| macOS | ❌ Not planned | |

---

## Build from Source

See [COMPILE.md](COMPILE.md) for detailed build instructions.

---

## Related Projects

- [FastRobot](https://github.com/andrestubbe/FastRobot) — Ultra-fast screen capture & automation.
- [FastDisplay](https://github.com/andrestubbe/FastDisplay) — Real-time display & DPI monitoring.
- [FastCore](https://github.com/andrestubbe/FastCore) — Native library loader for FastJava.
- [FastTheme](https://github.com/andrestubbe/FastTheme) — Window styling and dark mode detection.

---

**Made with ⚡ by Andre Stubbe**
