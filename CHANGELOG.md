# Changelog

## [4.0.0] - 2026-04-27
### Added
- **Direct2D Rendering Engine**: Replaced GDI+ with hardware-accelerated Direct2D for ultra-smooth pixel rendering.
- **120Hz Refresh Rate Support**: Upgraded the animation loop to 8ms (~125Hz) for high-refresh-rate gaming monitors.
- **Universal Visual Painter API**: Added `setStatusVisual(width, height, Consumer<Graphics2D>)` allowing arbitrary Java UI rendering next to the ghost mouse.
- **System Cursor Management**: Added native hooks to hide and restore the Windows system cursor (`setSystemCursorVisible`).
- **Dynamic Smoothing**: Real-time ease-out factor adjustment via `setSmoothing(float)`.

### Improved
- **Robust Fallback System**: Implemented an automatic GDI+ fallback that kicks in if hardware rendering is unavailable.
- **Scaling Support**: Enhanced 200% DPI scaling logic for high-resolution displays.
- **Architecture**: Shifted to a state-based multi-monitor overlay management system.

## [2.0.0] - Previous Stable
- Basic GDI+ transparent window overlay.
- JNI bridge for coordinate movement.
- JNI registration via `RegisterNatives`.
