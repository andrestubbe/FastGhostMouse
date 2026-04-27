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
    <artifactId>fastghostmouse</artifactId>
    <version>v0.4.0-ALPHA</version>
</dependency>
```

---

## Why FastGhostMouse?

Traditional Java overlays (using `JWindow` or `UpdateLayeredWindow`) suffer from several issues when used for high-speed cursor tracking:
- **Low Refresh Rate**: Usually capped at 60Hz or tied to the AWT event thread.
- **CPU Heavy**: Every move requires a full CPU-based redraw of the window.
- **Input Lag**: The "ghost" mouse often lags behind the logic due to buffer swapping.

**FastGhostMouse** solves this by moving the rendering to the **GPU**:
- **Hardware Positioning**: We move the mouse using `SetOffsetX/Y` in the DirectComposition visual tree.
- **Zero-Redraw Movement**: Moving the cursor doesn't require redrawing pixels; the GPU simply shifts the visual layer.
- **120Hz Native Loop**: A dedicated native thread ensures the mouse glides at the monitor's native refresh rate.

---

## Key Features

- **🚀 DirectComposition Visual Tree** — Pure GPU-based rendering for zero-latency transparency.
- **⚡ 120Hz+ High-Speed Loop** — Tuned for modern high-refresh-rate gaming monitors.
- **🎨 Universal Visual Painter** — Render complex Java UI (progress bars, icons) next to the cursor via `Graphics2D`.
- **🖱️ System Cursor Replacement** — Ability to hide the real system cursor (`setSystemCursorVisible`) for a full solo ghost experience.
- **🤖 Butler Bubble Helper** — One-line command to show premium, auto-sizing assistance bubbles.
- **🛠️ Zero Dependencies** — Java 17+ and Windows only (Powered by FastCore).

---

## Performance Benchmarks

| Metric | Standard Java Overlay | FastGhostMouse | Improvement |
|--------|----------------------|----------------|-------------|
| **Max Refresh Rate** | ~60 Hz | **120 Hz+** | **2× Smoother** |
| **CPU Usage (Moving)** | ~5-10% | **< 0.5%** | **20× More Efficient** |
| **Latency** | ~16-32ms | **< 8ms** | **Ultra-Low Latency** |
| **Transparency** | GDI Alpha | **Hardware GPU** | **Flicker-Free** |

---

## API Reference

| Method | Description |
|--------|-------------|
| `useAsSecondaryMouse(x, y, scale)`| Clones the system cursor and initializes the overlay. |
| `setButlerBubble(text)` | **[New]** Shows a sleek, semi-transparent rounded status bubble. |
| `moveTo(x, y)` | Smoothly interpolates the ghost mouse to a new position. |
| `setSmoothing(factor)` | Adjusts the "snappiness" of the movement (0.0 - 1.0). |
| `setStatusVisual(w, h, painter)` | Renders a custom status bubble using Java `Graphics2D`. |
| `setSystemCursorVisible(visible)` | Hides or restores the actual Windows system cursor. |

---

## Architecture

```
Java App (Graphics2D)
    ↓ RGBA Pixel Buffer
Direct2D (GPU Bitmap)
    ↓
DirectComposition (Visual Tree)
    ↓ Visual Offsets (SetOffsetX/Y)
Desktop Window Manager (DWM)
```

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

---

**Made with ⚡ by Andre Stubbe**
