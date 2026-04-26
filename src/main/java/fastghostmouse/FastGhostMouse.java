package fastghostmouse;

import fastcore.FastCore;

import java.awt.*;
import java.awt.image.BufferedImage;

/**
 * FastGhostMouse - High-performance Native Ghost Cursor Overlay
 * <p>
 * This class provides a native ghost cursor overlay that can be animated
 * independently of the real system cursor. It uses DirectComposition for GPU-accelerated rendering
 * and supports smooth animations with bitmap-based cursor and text rendering.
 * </p>
 * <p>
 * The ghost cursor appears as a custom bitmap image with optional text
 * rendered by Java. It can be moved smoothly with time-based interpolation
 * and shown/hidden on demand.
 * </p>
 *
 * @since 3.0.0
 * @version 3.0.0
 * @author FastJava Team
 */
public class FastGhostMouse {
    static {
        FastCore.loadLibrary("fastghostmouse");
        registerNatives();
    }

    private static native void registerNatives();

    /**
     * Initializes the ghost cursor overlay window.
     * <p>
     * Creates a transparent layered window that covers the entire screen
     * and sets up Direct2D rendering resources. The window is configured
     * to be topmost, transparent to mouse events, non-activating, and
     * invisible in taskbar.
     * </p>
     *
     * @param width  Initial width parameter (currently unused, window covers full screen)
     * @param height Initial height parameter (currently unused, window covers full screen)
     * @see #show()
     * @see #dispose()
     */
    public native void init(int width, int height);

    /**
     * Moves the ghost cursor to the specified coordinates.
     * <p>
     * The movement is interpolated for smooth animation. The actual position
     * updates gradually toward the target position with time-based interpolation
     * for frame-rate-independent smooth animation.
     * </p>
     *
     * @param x Target X coordinate in screen pixels
     * @param y Target Y coordinate in screen pixels
     * @see #init(int, int)
     */
    public native void moveTo(float x, float y);

    /**
     * Sets the cursor image from RGBA byte array.
     * <p>
     * Creates a Direct2D bitmap from the provided RGBA byte array.
     * The image is rendered centered at the cursor position.
     * </p>
     *
     * @param rgba   RGBA byte array (8 bits per channel, rowStride = width * 4)
     * @param width  Image width in pixels
     * @param height Image height in pixels
     * @see #setCursorImage(BufferedImage)
     */
    public native void setCursorImage(byte[] rgba, int width, int height);

    /**
     * Sets the text image from RGBA byte array.
     * <p>
     * Creates a Direct2D bitmap from the provided RGBA byte array.
     * The image is rendered above and to the right of the cursor position.
     * Passing NULL or invalid dimensions removes the text bitmap.
     * </p>
     *
     * @param rgba   RGBA byte array (8 bits per channel, rowStride = width * 4), or NULL to clear
     * @param width  Image width in pixels
     * @param height Image height in pixels
     * @see #setText(String)
     */
    public native void setTextImage(byte[] rgba, int width, int height);

    /**
     * Sets the text offset from the cursor position.
     * <p>
     * Allows dynamic positioning of text relative to the cursor.
     * Default offset is (20, -40) for text above and to the right.
     * </p>
     *
     * @param dx X offset from cursor
     * @param dy Y offset from cursor
     */
    public native void setTextOffset(float dx, float dy);

    /**
     * Shows the ghost cursor overlay.
     * <p>
     * Makes the transparent overlay window visible. The ghost cursor
     * will appear at its current position (initially 0,0 if not moved).
     * </p>
     *
     * @see #hide()
     * @see #init(int, int)
     */
    public native void show();

    /**
     * Hides the ghost cursor overlay.
     * <p>
     * Makes the transparent overlay window invisible. The animation thread
     * continues running in the background.
     * </p>
     *
     * @see #show()
     */
    public native void hide();

    /**
     * Disposes of the ghost cursor and releases native resources.
     * <p>
     * Stops the animation thread, releases Direct2D resources and bitmaps,
     * and destroys the overlay window. After calling this method, the
     * GhostMouse instance cannot be used again.
     * </p>
     *
     * @see #init(int, int)
     */
    public native void dispose();

    /**
     * Converts a BufferedImage to RGBA byte array with premultiplied alpha.
     *
     * @param img The image to convert
     * @return RGBA byte array (8 bits per channel, premultiplied alpha)
     */
    private static byte[] toRGBA(BufferedImage img) {
        int w = img.getWidth();
        int h = img.getHeight();
        byte[] out = new byte[w * h * 4];

        int idx = 0;
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int argb = img.getRGB(x, y);

                int a = (argb >> 24) & 0xFF;
                float af = a / 255f;

                int r = (int) (((argb >> 16) & 0xFF) * af);
                int g = (int) (((argb >> 8)  & 0xFF) * af);
                int b = (int) (( argb        & 0xFF) * af);

                out[idx++] = (byte) r;
                out[idx++] = (byte) g;
                out[idx++] = (byte) b;
                out[idx++] = (byte) a;
            }
        }
        return out;
    }

    /**
     * Sets the cursor image from a BufferedImage.
     *
     * @param img The cursor image to set
     */
    public void setCursorImage(BufferedImage img) {
        byte[] rgba = toRGBA(img);
        setCursorImage(rgba, img.getWidth(), img.getHeight());
    }

    /**
     * Renders text as a BufferedImage.
     *
     * @param text The text to render
     * @return BufferedImage containing the rendered text, or null if text is empty
     */
    public BufferedImage renderText(String text) {
        if (text == null || text.isEmpty()) {
            return null;
        }

        BufferedImage tmp = new BufferedImage(1, 1, BufferedImage.TYPE_INT_ARGB);
        Graphics2D g = tmp.createGraphics();
        g.setFont(new Font("Segoe UI", Font.PLAIN, 18));
        FontMetrics fm = g.getFontMetrics();
        int w = fm.stringWidth(text) + 12;
        int h = fm.getHeight() + 6;
        g.dispose();

        BufferedImage img = new BufferedImage(w, h, BufferedImage.TYPE_INT_ARGB);
        g = img.createGraphics();
        g.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);

        g.setColor(new Color(0, 0, 0, 180));
        g.fillRoundRect(0, 0, w, h, 10, 10);

        g.setColor(Color.WHITE);
        g.setFont(new Font("Segoe UI", Font.PLAIN, 18));
        g.drawString(text, 6, h - 6);

        g.dispose();
        return img;
    }

    /**
     * Sets the text to display as a rendered image.
     * <p>
     * The text is rendered by Java and sent to native as a bitmap.
     * Pass an empty string or null to hide the text.
     * </p>
     *
     * @param text Text to display, or empty string to hide
     */
    public void setText(String text) {
        if (text == null || text.isEmpty()) {
            setTextImage(null, 0, 0);
            return;
        }

        BufferedImage img = renderText(text);
        byte[] rgba = toRGBA(img);
        setTextImage(rgba, img.getWidth(), img.getHeight());
    }
}
