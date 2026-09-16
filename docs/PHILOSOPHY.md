# The Philosophy of FastGhostMouse

> [!IMPORTANT]
> **"Visual feedback without interference: zero focus theft, absolute click-through, and zero AWT lag."**

FastGhostMouse is built on the fundamental principle that robotics bots, autonomous AI models, and testing frameworks require visual debugging, but traditional Java UI solutions degrade or destroy the target environment.

## Core Tenets

### 1. Do Not Disturb the Host
An automation overlay must never steal window focus (`WS_EX_NOACTIVATE`) and must never capture or block mouse input (`WS_EX_TRANSPARENT`). When a user or bot clicks, the hardware event must reach the underlying window, not the overlay.

### 2. Bypass the Java AWT Event Dispatch Thread (EDT)
Standard Java UI components (`JFrame`, `JWindow`, `JDialog`) are bound to AWT's single-threaded EDT. Running high-frequency cursor animations on the EDT causes micro-stuttering, GC pressure, and input latency. FastGhostMouse runs its interpolation loop on a dedicated lightweight thread, driving DirectComposition native surfaces directly via `FastOverlay`.

### 3. Visual Transparency for Autonomous Agents
When training or debugging AI agents, observing the exact planned and smoothed trajectory is critical. FastGhostMouse interpolates motion continuously and renders state tags, giving developers immediate visual inspection of AI intentions without interfering with vision capture pipelines like `FastScreen`.

### 4. Ecosystem Synergy
FastGhostMouse is designed to compose cleanly with the FastJava substrate:
* **Render Pipeline**: Built directly on `FastOverlay` for DirectComposition transparency.
* **Input Injection**: Integrates with `FastRobot` for native Win32 `SendInput` automation.
* **JNI Substrate**: Native libraries resolved and loaded seamlessly via `FastCore`.

---

Part of the **FastJava** ecosystem. Clean architectures, zero EDT lag, maximum speed.
