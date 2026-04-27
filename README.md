# FastGhostMouse v4.0 (DirectComposition Edition) 🚀

**FastGhostMouse** is a high-performance, GPU-accelerated secondary mouse overlay for Windows. Built for the **FastJava** ecosystem, it provides an ultra-smooth (120Hz+), click-through, and focus-agnostic "Secondary Mouse" that can be used for AI cursor visualization, butler assistance, or UI indicators.

## 🌟 Key Features

- **DirectComposition Visual Tree**: Pure GPU composition using a hardware visual tree.
- **Hardware-Accelerated Movement**: Movement is handled by visual offsets (`SetOffsetX/Y`), resulting in buttery smooth motion even at 120Hz.
- **Universal Visual Painter**: Render complex Java-driven UI (progress bars, icons, bubbles) directly next to the cursor via `Graphics2D`.
- **Real-Time Cursor Capture**: Clones the actual Windows system cursor (Hand, Arrow, Wait) in real-time.
- **Multi-Monitor Support**: Automatically spawns per-monitor overlays for seamless traversal.
- **Zero-Latency Fallback**: Robust GDI+ fallback system ensures visibility even on legacy hardware.

## 🛠 Architecture

FastGhostMouse uses a dual-engine approach to ensure both performance and reliability:

1. **DirectComposition (Primary)**: The modern Windows composition engine. Binds a D3D11 device to a visual tree, allowing the GPU to handle alpha-blending and positioning.
2. **Direct2D (Rendering)**: All visuals are rendered using GPU-backed bitmaps, providing crisp anti-aliasing and high performance.
3. **JNI / FastCore**: Low-latency bridge between Java logic and the native C++ graphics stack.

## 🚀 Quick Start

### Java API

```java
FastGhostMouse ghost = new FastGhostMouse();

// 1. Initialize as a Secondary Mouse (cloning the system cursor)
ghost.useAsSecondaryMouse(startX, startY, 2.0f); // 200% scaling support

// 2. Set Smoothness (Ease-Out factor)
ghost.setSmoothing(0.12f); // Snappy but smooth

// 3. Update Status via the Universal Painter
ghost.setStatusVisual(140, 50, g -> {
    g.setColor(new Color(40, 40, 40, 200));
    g.fillRoundRect(0, 0, 140, 45, 10, 10);
    g.setColor(Color.WHITE);
    g.drawString("Butler is thinking...", 12, 25);
});

// 4. Move smoothly to a new target
ghost.moveTo(x, y);

// 5. Show it!
ghost.show();
```

## 🏗 Build & Run

### Prerequisites
- **Visual Studio 2022** (with C++ tools)
- **JDK 22+**
- **Maven**

### Commands
1. **Compile Native DLL**: Run `.\compile.bat`
2. **Build Java Project**: `mvn clean package`
3. **Run Demo**:
   ```bash
   java -cp "bin;target\fastghostmouse-1.0.0-jar-with-dependencies.jar" "-Djava.library.path=." fastghostmouse.demo.Demo
   ```

## 📜 License
Part of the **FastJava** ecosystem. All rights reserved.
