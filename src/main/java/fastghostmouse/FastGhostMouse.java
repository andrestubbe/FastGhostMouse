package fastghostmouse;

import fastcore.FastCore;
import fastoverlay.FastOverlay;
import fastoverlay.FastOverlayWindow;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.util.concurrent.atomic.AtomicBoolean;

public class FastGhostMouse {
    static {
        FastCore.loadLibrary("fastghostmouse");
        registerNatives();
        FastOverlay.initEngine();
    }

    private static native void registerNatives();

    private FastOverlayWindow cursorWindow;
    private FastOverlayWindow textWindow;

    public float targetX = 0;
    public float targetY = 0;
    public float currentX = 0;
    public float currentY = 0;
    private float smoothing = 0.12f;

    private int cursorW = 0, cursorH = 0;
    private int textW = 0, textH = 0;
    public float textDx = 10, textDy = -20;
    
    public void setTextOffset(float dx, float dy) {
        this.textDx = dx;
        this.textDy = dy;
    }

    private Thread animationThread;
    private AtomicBoolean running = new AtomicBoolean(false);

    public FastOverlayWindow getCursorWindow() {
        return cursorWindow;
    }

    public void setCursorPositionImmediate(int x, int y) {
        this.currentX = x;
        this.currentY = y;
        this.targetX = x;
        this.targetY = y;
        if (cursorWindow != null) cursorWindow.setPosition(x, y);
    }

    public void init(int startX, int startY) {
        currentX = startX;
        currentY = startY;
        targetX = startX;
        targetY = startY;

        cursorWindow = new FastOverlayWindow(startX, startY, 1, 1, true, true);
        textWindow = new FastOverlayWindow(startX, startY, 1, 1, true, true);

        running.set(true);
        animationThread = new Thread(() -> {
            long last = System.currentTimeMillis();
            while (running.get()) {
                long now = System.currentTimeMillis();
                float dt = now - last;
                last = now;

                float k = (dt / 16.0f) * smoothing;
                if (k > 1.0f) k = 1.0f;

                currentX += (targetX - currentX) * k;
                currentY += (targetY - currentY) * k;

                int cx = (int)currentX;
                int cy = (int)currentY;
                cursorWindow.setPosition(cx, cy);

                int tx = (int)(currentX + textDx);
                int ty = (int)(currentY + textDy);
                textWindow.setPosition(tx, ty);

                try { Thread.sleep(8); } catch (InterruptedException e) { break; }
            }
        });
        animationThread.start();
    }

    public void moveTo(float x, float y) {
        this.targetX = x;
        this.targetY = y;
    }

    public void setSmoothing(float factor) { this.smoothing = factor; }

    /**
     * Convenience method: initialises the ghost cursor at (startX, startY),
     * captures the current system cursor image, and shows the overlay.
     * cursorIndex is reserved for future multi-cursor support.
     */
    public void useAsSecondaryMouse(int startX, int startY, int cursorIndex) {
        init(startX, startY);
        captureSystemCursor();
        show();
    }

    public void setCursorImage(BufferedImage img) {
        if (img == null) return;
        this.cursorW = img.getWidth();
        this.cursorH = img.getHeight();
        if (cursorWindow != null) {
            cursorWindow.setSize(cursorW, cursorH);
            cursorWindow.markDirty();
            cursorWindow.updateImage(img);
        }
    }

    public void setTextImage(BufferedImage img) {
        if (img == null) {
            if (textWindow != null) textWindow.hide();
            return;
        }
        this.textW = img.getWidth();
        this.textH = img.getHeight();
        if (textWindow != null) {
            textWindow.setSize(textW, textH);
            textWindow.markDirty();
            textWindow.updateImage(img);
            textWindow.show();
        }
    }

    private native byte[] captureSystemCursorNative(int[] dimensions);
    public native void setSystemCursorVisible(boolean visible);

    public void captureSystemCursor() {
        int[] dims = new int[2];
        byte[] rgba = captureSystemCursorNative(dims);
        if (rgba != null && dims[0] > 0 && dims[1] > 0) {
            int w = dims[0];
            int h = dims[1];
            BufferedImage img = new BufferedImage(w, h, BufferedImage.TYPE_INT_ARGB);
            int[] pixels = new int[w * h];
            for (int i = 0; i < pixels.length; i++) {
                int b = rgba[i * 4] & 0xFF;
                int g = rgba[i * 4 + 1] & 0xFF;
                int r = rgba[i * 4 + 2] & 0xFF;
                int a = rgba[i * 4 + 3] & 0xFF;
                pixels[i] = (a << 24) | (r << 16) | (g << 8) | b;
            }
            img.setRGB(0, 0, w, h, pixels, 0, w);
            setCursorImage(img);
        }
    }

    public void show() {
        if (cursorWindow != null) cursorWindow.show();
        if (textWindow != null && textW > 0 && textH > 0) textWindow.show();
    }

    public void hide() {
        if (cursorWindow != null) cursorWindow.hide();
        if (textWindow != null) textWindow.hide();
    }

    public void dispose() {
        running.set(false);
        if (animationThread != null) {
            try { animationThread.join(200); } catch (Exception ignored) {}
        }
        if (cursorWindow != null) cursorWindow.dispose();
        if (textWindow != null) textWindow.dispose();
        setSystemCursorVisible(true);
    }
}
