# FastGhostMouse Roadmap

**Vision:** High-performance, zero-latency ghost cursor and AI trajectory visualization for the JVM.

## Current Milestone: v0.1.0 (Released)
- [x] Standardized FastJava ecosystem architecture.
- [x] DirectComposition GPU overlay integration via `FastOverlay`.
- [x] Native hardware cursor capture and dynamic state labeling.
- [x] Smooth physics-based cursor motion interpolation.
- [x] Official JMH benchmark suite measuring throughput.
- [x] Interactive window-repositioning demo (`examples/Demo`).

## Milestone: v0.2.0 (Performance & Vector Trails)
- [ ] Hardware-accelerated motion trails (spline path drawing via Direct2D).
- [ ] Multi-cursor support for parallel agent visualization.
- [ ] Low-overhead coordinate ring buffer for telemetry recording.

## Milestone: v0.3.0 (Cross-Platform Foundations)
- [ ] Linux X11 Composite & Wayland transparent subsurface prototypes.
- [ ] macOS Metal-backed click-through window implementation.

---

Part of the **FastJava** ecosystem. Clean architectures, zero EDT lag, maximum speed.
