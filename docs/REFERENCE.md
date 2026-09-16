# FastGhostMouse Reference

## 1. Native Architecture & Subsystems

`FastGhostMouse` provides a hardware-accelerated, transparent ghost cursor overlay for AI agent trajectory visualization, automation debugging, and bot telemetry.

It directly composites two native `FastOverlayWindow` surfaces via DirectComposition / DWM:
1. **Cursor Window**: Renders either a custom high-DPI `BufferedImage` or the captured Windows OS hardware cursor (`captureSystemCursor()`).
2. **Text Label Window**: Renders dynamic HUD status text or AI state labels directly offset next to the cursor without affecting OS hit testing.

Both windows enforce the following Win32 styles:
* `WS_EX_TRANSPARENT`: Mathematically transparent to the Windows input subsystem. Mouse and keyboard events pass directly to windows below.
* `WS_EX_NOACTIVATE`: Never receives or steals window focus, guaranteeing target application continuity.
* `WS_EX_LAYERED`: Sub-millisecond alpha blending and per-pixel transparency composited directly by the Desktop Window Manager (DWM).

---

## 2. API Reference

### `FastGhostMouse`

| Method | Signature | Description | Native / Subsystem Call |
|:---|:---|:---|:---|
| `init(startX, startY)` | `void` | Spawns dual `FastOverlayWindow` handles at coordinates and starts the 120 FPS physics loop. | `FastOverlay.createWindow` |
| `useAsSecondaryMouse(x, y, index)` | `void` | Spawns cursor, captures current OS hardware cursor, and displays overlay. | `captureSystemCursor()`, `show()` |
| `moveTo(targetX, targetY)` | `void` | Sets target coordinates for smooth physics-based cursor interpolation. | Internal delta-time Euler integration |
| `setCursorPositionImmediate(x, y)` | `void` | Instantly teleports the cursor overlay without animation delay. | `FastOverlayWindow.setPosition` |
| `setSmoothing(factor)` | `void` | Configures momentum and easing responsiveness (default: `0.12f`). | Animation physics coefficient |
| `setTextOffset(dx, dy)` | `void` | Configures pixel offset of the status label window relative to cursor tip. | Win32 layout translation |
| `setCursorImage(img)` | `void` | Applies custom `BufferedImage` icon to the cursor overlay surface. | `FastOverlayWindow.updateImage` |
| `setTextImage(img)` | `void` | Applies custom `BufferedImage` rendered label next to cursor. | `FastOverlayWindow.updateImage` |
| `captureSystemCursor()` | `void` | Queries native Win32 `GetCursorInfo` and extracts current OS cursor bitmap. | `captureSystemCursorNative` (JNI) |
| `setSystemCursorVisible(visible)` | `void` | Dynamically hides or restores the OS hardware cursor via Win32 `SetSystemCursor`. | `setSystemCursorVisible` (JNI) |
| `show()` | `void` | Makes cursor and text overlays visible. | `FastOverlayWindow.show` |
| `hide()` | `void` | Hides cursor and text overlays. | `FastOverlayWindow.hide` |
| `dispose()` | `void` | Shuts down animation thread, releases overlay windows, and restores OS cursor. | Win32 handle cleanup |

---

### `FastGhostMouseBot`

| Method | Signature | Description | Subsystem Call |
|:---|:---|:---|:---|
| `softPress(x, y)` | `void` | Injects low-latency virtual left mouse button down event. | `FastRobot.virtualMousePress` |
| `softDrag(x, y)` | `void` | Dispatches continuous coordinate stream during active drag. | `FastRobot.virtualMouseMove` |
| `softRelease(x, y)` | `void` | Injects virtual left mouse button release and resets drag state. | `FastRobot.virtualMouseRelease` |
| `isDragging()` | `boolean` | Queries active drag state. | Internal boolean state |

---

## 3. Platform Support

| Platform | Architecture | Status | Engine / Pipeline |
|:---|:---|:---|:---|
| **Windows 10 / 11** | `x64` | ✅ Fully Supported | Win32 Layered Windows + DirectComposition (`FastOverlay`) |
| **Windows 10 / 11** | `arm64` | 🚧 Planned | Native Windows on ARM DirectComposition pipeline |
| **Linux** | `x64` | 🚧 Planned | X11 Composite / Wayland Subsurface transparent overlay |
| **macOS** | `x64` / `arm64` | 🚧 Planned | CoreGraphics / Metal Overlay Window |

---

## 4. Ecosystem Integration

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

Part of the **FastJava** ecosystem. Clean architectures, zero EDT lag, maximum speed.
