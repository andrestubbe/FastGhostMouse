package fastghostmouse.demo;

import fastghostmouse.FastGhostMouse;

import java.awt.*;
import java.awt.image.BufferedImage;

public class Demo {
    public static void main(String[] args) throws InterruptedException {
        System.setProperty("sun.java2d.uiScale", "1.0");
        FastGhostMouse ghost = new FastGhostMouse();

        System.out.println("Initializing FastGhostMouse as Secondary Mouse...");
        ghost.useAsSecondaryMouse(100, 100, 2); // 200% scaling
        ghost.setStatusText("Ready to serve");

        Thread.sleep(1000);
        System.out.println("Hiding real system mouse for 3 seconds...");
        ghost.setSystemCursorVisible(false);
        ghost.setStatusText("Real mouse is hidden!");
        Thread.sleep(3000);

        System.out.println("Starting circle animation...");
        float centerX = 800;
        float centerY = 450;
        float radius = 250;

        // One circle with text updates and speed changes
        for (float t = 0; t < 2 * Math.PI; t += 0.05f) {
            float x = centerX + (float) Math.cos(t) * radius;
            float y = centerY + (float) Math.sin(t) * radius;

            if (t < Math.PI) {
                ghost.setSmoothing(0.25f);
                ghost.setStatusVisual(120, 40, g -> {
                    g.setColor(new Color(0, 0, 0, 180));
                    g.fillRoundRect(0, 0, 120, 30, 15, 15);
                    g.setColor(Color.WHITE);
                    g.setFont(new Font("Segoe UI", Font.BOLD, 12));
                    g.drawString("Snappy Phase", 15, 20);
                });
            } else {
                ghost.setSmoothing(0.04f);
                final float progress = (t - (float)Math.PI) / (float)Math.PI;
                ghost.setStatusVisual(140, 50, g -> {
                    g.setColor(new Color(40, 40, 40, 200));
                    g.fillRoundRect(0, 0, 140, 45, 10, 10);
                    g.setColor(new Color(200, 200, 200));
                    g.setFont(new Font("Segoe UI", Font.PLAIN, 11));
                    g.drawString("Butler Mode...", 12, 18);
                    g.setColor(new Color(100, 100, 100));
                    g.fillRoundRect(12, 28, 116, 6, 3, 3);
                    g.setColor(new Color(0, 200, 255));
                    g.fillRoundRect(12, 28, (int)(116 * progress), 6, 3, 3);
                });
            }

            ghost.moveTo(x, y);
            Thread.sleep(16); // ~60fps updates from Java
        }

        ghost.setStatusText("Task completed.");
        System.out.println("Restoring real system mouse...");
        ghost.setSystemCursorVisible(true);
        Thread.sleep(1000);

        System.out.println("Shutting down...");
        ghost.setSystemCursorVisible(true);
        ghost.dispose();
    }
}
