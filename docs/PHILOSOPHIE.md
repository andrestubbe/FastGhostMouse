# The Philosophy of FastGhostMouse

> [!IMPORTANT]
> **"Visual Feedback without Interference. Zero Focus Theft. Absolute Click-Through."**

FastGhostMouse is built on the principle that automation bots, AI models, and testing frameworks desperately need visual debugging, but traditional Java UI solutions destroy the very environment they try to automate.

## Core Tenets

1.  **Do Not Disturb the Host**
    An automation overlay must never steal focus (`WS_EX_NOACTIVATE`) and must never block input (`WS_EX_TRANSPARENT`). If a bot clicks, the click must reach the target app, not the overlay.

2.  **Bypass the AWT/Swing EDT**
    Standard Java windows (`JFrame`) are bound to the Event Dispatch Thread. Tying your bot's visual feedback to the heavy AWT pipeline guarantees micro-stutters and input lag. FastGhostMouse hooks directly into native Win32/DWM rendering to bypass Java's UI constraints entirely.

3.  **Visual Truth for AI**
    When training or debugging AI agents, seeing the *exact* intended path of the cursor is critical. FastGhostMouse interpolates natively to show smooth trajectories and state changes, giving developers an instant "window into the AI's mind."

4.  **Blueprint Consistency**
    As part of the **FastJava** ecosystem, FastGhostMouse adheres to a standardized architecture:
    *   **Native Backend**: Direct C++ implementation via `FastOverlay`.
    *   **Unified Loading**: Powered by `FastCore`.
    *   **Premium Quality**: Built specifically for high-performance automation and autonomous agents.

---
**❤️ FastGhostMouse — Powering the next generation of Native Java.**
