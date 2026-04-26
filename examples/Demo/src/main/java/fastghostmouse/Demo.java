package fastghostmouse.demo;

import fastghostmouse.FastGhostMouse;

import java.awt.*;
import java.awt.image.BufferedImage;

public class Demo {
    public static void main(String[] args) throws InterruptedException {
        FastGhostMouse ghost = new FastGhostMouse();

        System.out.println("Initializing FastGhostMouse...");
        ghost.init(100, 100);

        // Create a simple cursor image (white circle with glow)
        BufferedImage cursorImg = new BufferedImage(32, 32, BufferedImage.TYPE_INT_ARGB);
        Graphics2D g = cursorImg.createGraphics();
        g.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        // Glow
        for (int i = 6; i > 0; i--) {
            g.setColor(new Color(255, 255, 255, 12 * i));
            g.fillOval(16 - i * 3, 16 - i * 3, i * 6, i * 6);
        }

        // Core
        g.setColor(new Color(255, 255, 255, 242));
        g.fillOval(8, 8, 16, 16);
        g.dispose();

        ghost.setCursorImage(cursorImg);
        ghost.show();
        ghost.setText("Ghost Active");

        Thread.sleep(1000);

        System.out.println("Starting circle animation...");
        float centerX = 800;
        float centerY = 450;
        float radius = 250;

        // One circle with text updates
        for (float t = 0; t < 2 * Math.PI; t += 0.05f) {
            float x = centerX + (float) Math.cos(t) * radius;
            float y = centerY + (float) Math.sin(t) * radius;

            if (t > Math.PI) {
                ghost.setText("Following path...");
            }

            ghost.moveTo(x, y);
            Thread.sleep(16); // ~60fps updates from Java
        }

        ghost.setText("Destination reached.");
        Thread.sleep(2000);

        System.out.println("Shutting down...");
        ghost.dispose();
    }
}
