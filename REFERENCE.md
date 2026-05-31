# FastGhostMouse Reference

## 1. Native Overlay Engine
*   **WS_EX_TRANSPARENT**: The overlay window is mathematically ignored by the Windows input subsystem. Mouse clicks and keyboard events pass directly through to the underlying application.
*   **WS_EX_NOACTIVATE**: The overlay will never steal window focus from your active game, application, or browser, ensuring automation bots are not interrupted.
*   **WS_EX_LAYERED**: Enables per-pixel alpha transparency and high-performance image blending via standard Windows APIs.

## 2. API Capabilities
*   **useAsSecondaryMouse(x, y, index)**: Captures the active Windows cursor and seamlessly integrates it into a secondary, scriptable hardware cursor.
*   **moveTo(x, y)**: Initiates a hardware-accelerated, interpolated movement to the target coordinates.
*   **setSmoothing(factor)**: Adjusts the momentum and easing of the cursor. Lower values produce heavier, more human-like drag, while higher values snap instantly.
*   **setTextImage**: Attaches dynamic visual text or labels directly beneath the cursor (e.g., "Executing AI Action").

## 3. FastCore & FastOverlay Bindings
*   **JNI Hooks**: `FastGhostMouse` relies on the unified `FastCore` JNI loader to access `FastOverlayWindow` instances.
*   **Zero Swing/AWT Logic**: The overlay bypasses the Java Event Dispatch Thread (EDT) completely, preventing UI locks or stutters during complex bot logic.

## 4. Platform Support
| Platform | Status |
|----------|--------|
| Windows 10/11 (x64) | ✅ Fully Supported |
| Linux | 🚧 Planned (X11/Wayland hooks) |
| macOS | 🚧 Planned |

---
**Part of the FastJava Ecosystem** — *Making the JVM faster. Small package. Maximum speed. Zero bloat. 🚀📋*
