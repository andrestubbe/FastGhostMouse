package examples.demo;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import fastghostmouse.FastGhostMouse;
import fastghostmouse.FastGhostMouseBot;
import fasttheme.FastTheme;

public class FastGhostMouseDemo2 {
    private static volatile int winX, winY;
    private static final int HOME_W = 300, HOME_H = 300;
    private static volatile boolean userDragging = false;
    private static volatile boolean botActive = false;
    private static volatile long lastUserAction = 0;

    private static int dragOffsetX = 0;
    private static int dragOffsetY = 0;

    private static final int TITLEBAR_HEIGHT = 30;
    private static final int RESIZE_ZONE = 10;

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> createUI());
    }

    private static void createUI() {
        Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
        winX = (screen.width - HOME_W) / 2;
        winY = (screen.height - HOME_H) / 2;

        JFrame frame = new JFrame();
        frame.setSize(HOME_W, HOME_H);
        frame.setLocation(winX, winY);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        JPanel panel = new JPanel();
        panel.setBackground(Color.BLACK);
        frame.add(panel);

        frame.addNotify();
        long hwnd = FastTheme.getWindowHandle(frame);
        FastTheme.setTitleBarDarkMode(hwnd, true);
        FastTheme.setTitleBarColor(hwnd, 0, 0, 0);
        FastTheme.setTitleBarTextColor(hwnd, 255, 255, 255);

        frame.addComponentListener(new ComponentAdapter() {
            public void componentMoved(ComponentEvent e) {
                if (!botActive) {
                    winX = frame.getX();
                    winY = frame.getY();
                    lastUserAction = System.currentTimeMillis();
                }
            }
            public void componentResized(ComponentEvent e) {
                if (!botActive) lastUserAction = System.currentTimeMillis();
            }
        });

        frame.addMouseListener(new MouseAdapter() {
            public void mousePressed(MouseEvent e) {
                if (botActive) return;
                dragOffsetX = e.getX();
                dragOffsetY = e.getY();
                userDragging = true;
                lastUserAction = System.currentTimeMillis();
            }
            public void mouseReleased(MouseEvent e) {
                if (botActive) return;
                userDragging = false;
                lastUserAction = System.currentTimeMillis();
            }
        });

        frame.addMouseMotionListener(new MouseMotionAdapter() {
            public void mouseDragged(MouseEvent e) {
                if (botActive) return;
                int ex = e.getX();
                int ey = e.getY();

                if (ex > frame.getWidth() - RESIZE_ZONE && ey > frame.getHeight() - RESIZE_ZONE) {
                    frame.setSize(Math.max(150, frame.getWidth() + (ex - dragOffsetX)),
                                   Math.max(150, frame.getHeight() + (ey - dragOffsetY)));
                    dragOffsetX = ex;
                    dragOffsetY = ey;
                } else if (ey < TITLEBAR_HEIGHT) {
                    winX = frame.getX() + ex - dragOffsetX;
                    winY = frame.getY() + ey - dragOffsetY;
                    frame.setLocation(winX, winY);
                }
                lastUserAction = System.currentTimeMillis();
            }
        });

        frame.setVisible(true);

        FastGhostMouse ghost = new FastGhostMouse();
        ghost.setSmoothing(0.15f);
        ghost.useAsSecondaryMouse(winX - 100, winY + 50, 1);

        FastGhostMouseBot bot = new FastGhostMouseBot();

        new Thread(() -> botLoop(frame, ghost, bot)).start();
    }

    private static void botLoop(JFrame frame, FastGhostMouse ghost, FastGhostMouseBot bot) {
        try {
            Thread.sleep(1000);
            Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
            int homeX = (screen.width - HOME_W) / 2;
            int homeY = (screen.height - HOME_H) / 2;

            while (true) {
                Thread.sleep(50);
                if (userDragging || botActive) continue;

                int dx = homeX - winX;
                int dy = homeY - winY;
                double dist = Math.sqrt(dx * dx + dy * dy);
                boolean wrongSize = frame.getWidth() != HOME_W || frame.getHeight() != HOME_H;

                if (dist < 20 && wrongSize) {
                    botActive = true;
                    frame.setSize(HOME_W, HOME_H);
                    frame.setLocation(homeX, homeY);
                    winX = homeX;
                    winY = homeY;
                    smoothMove(ghost, homeX - 100, homeY + 50, 200);
                    botActive = false;
                    continue;
                }

                if (dist < 20 && !wrongSize) continue;
                if (System.currentTimeMillis() - lastUserAction < 500) continue;

                botActive = true;

                int grabX = winX + frame.getWidth() / 2;
                int grabY = winY + TITLEBAR_HEIGHT / 2;
                smoothMove(ghost, grabX, grabY, 200);

                bot.softPress(grabX, grabY);

                int steps = 40;
                for (int i = 0; i < steps; i++) {
                    double t = ease(i / (double) steps);
                    int curX = (int) (winX + dx * t);
                    int curY = (int) (winY + dy * t);

                    int gx = curX + frame.getWidth() / 2;
                    int gy = curY + TITLEBAR_HEIGHT / 2;

                    frame.setLocation(curX, curY);

                    ghost.setCursorPositionImmediate(gx, gy);

                    bot.softDrag(gx, gy);
                    Thread.sleep(16);
                }

                bot.softRelease(homeX + frame.getWidth() / 2, homeY + TITLEBAR_HEIGHT / 2);

                if (wrongSize) frame.setSize(HOME_W, HOME_H);

                winX = homeX;
                winY = homeY;
                frame.setLocation(winX, winY);

                smoothMove(ghost, winX - 100, winY + 50, 200);

                botActive = false;
                lastUserAction = System.currentTimeMillis();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static void smoothMove(FastGhostMouse ghost, int tx, int ty, int durationMs) throws Exception {
        int steps = durationMs / 16;
        double sx = ghost.currentX;
        double sy = ghost.currentY;
        for (int i = 0; i <= steps; i++) {
            double t = ease(i / (double) steps);
            ghost.moveTo((float)(sx + (tx - sx) * t), (float)(sy + (ty - sy) * t));
            Thread.sleep(16);
        }
    }

    private static double ease(double t) {
        return t < 0.5 ? 2 * t * t : 1 - Math.pow(-2 * t + 2, 2) / 2;
    }
}
