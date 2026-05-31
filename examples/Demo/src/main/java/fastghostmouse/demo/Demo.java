package fastghostmouse.demo;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import fasttheme.FastTheme;
import fastghostmouse.FastGhostMouse;

public class Demo {

    // Window state
    private static volatile int winX, winY;
    private static final int HOME_W = 300, HOME_H = 300;
    private static volatile boolean userDragging = false;
    private static volatile boolean botActive = false;
    private static volatile long lastUserAction = 0;

    // Drag offsets
    private static int dragOffsetX = 0;
    private static int dragOffsetY = 0;

    // Zones
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

        frame.setMinimumSize(new Dimension(HOME_W, HOME_H));

        JPanel panel = new JPanel();
        panel.setBackground(Color.BLACK);
        frame.add(panel);

        // Track window movement
        frame.addComponentListener(new ComponentAdapter() {
            public void componentMoved(ComponentEvent e) {
                if (!botActive) {
                    winX = frame.getX();
                    winY = frame.getY();
                    lastUserAction = System.currentTimeMillis();
                }
            }
            public void componentResized(ComponentEvent e) {
                if (!botActive) {
                    winX = frame.getX();
                    winY = frame.getY();
                    lastUserAction = System.currentTimeMillis();
                }
            }
        });

        // Mouse listeners (REAL mouse only)
        frame.addMouseListener(new MouseAdapter() {
            public void mousePressed(MouseEvent e) {
                if (botActive) return; // ignore ghost mouse

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
                if (botActive) return; // ignore ghost mouse

                int ex = e.getX();
                int ey = e.getY();

                // Resize zone
                if (isInResizeZone(frame, ex, ey)) {
                    int newW = Math.max(HOME_W, frame.getWidth() + (ex - dragOffsetX));
                    int newH = Math.max(HOME_H, frame.getHeight() + (ey - dragOffsetY));
                    frame.setSize(newW, newH);
                    dragOffsetX = ex;
                    dragOffsetY = ey;
                }
                // Titlebar drag
                else if (ey < TITLEBAR_HEIGHT) {
                    winX = frame.getX() + ex - dragOffsetX;
                    winY = frame.getY() + ey - dragOffsetY;
                    frame.setLocation(winX, winY);
                }

                lastUserAction = System.currentTimeMillis();
            }
        });

        frame.addNotify();
        long hwnd = FastTheme.getWindowHandle(frame);
        FastTheme.setTitleBarDarkMode(hwnd, true);
        FastTheme.setTitleBarColor(hwnd, 0, 0, 0);
        FastTheme.setTitleBarTextColor(hwnd, 255, 255, 255);
        FastTheme.setWindowTransparency(hwnd, 224);

        frame.setVisible(true);

        // Create a round icon for the title bar (JFrame icon)
        java.awt.image.BufferedImage iconImg = new java.awt.image.BufferedImage(32, 32, java.awt.image.BufferedImage.TYPE_INT_ARGB);
        Graphics2D gIcon = iconImg.createGraphics();
        gIcon.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        gIcon.setColor(new Color(200, 200, 200)); // Light grey circle
        gIcon.fillOval(4, 4, 24, 24);
        gIcon.setColor(Color.WHITE);
        gIcon.setStroke(new BasicStroke(2));
        gIcon.drawOval(4, 4, 24, 24);
        gIcon.dispose();
        frame.setIconImage(iconImg);

        // Update Title with X, Y, W, H at 60fps
        new Timer(16, e -> {
            frame.setTitle(frame.getX() + ", " + frame.getY() + ", " + frame.getWidth() + ", " + frame.getHeight());
        }).start();

        // Start GhostMouse
        FastGhostMouse ghost = new FastGhostMouse();
        ghost.setSmoothing(0.15f);
        // Zentriere die Sprechblase Ã¼ber der Maus (Bild ist 64px breit, Mitte ist bei -32)
        ghost.setTextOffset(-32, -35);

        int parkX = winX - 100;
        int parkY = winY + 50;
        
        ghost.useAsSecondaryMouse(parkX, parkY, 1);

        // Start bot thread
        new Thread(() -> botLoop(frame, ghost)).start();
    }

    private static boolean isInResizeZone(JFrame frame, int ex, int ey) {
        return ex > frame.getWidth() - RESIZE_ZONE && ey > frame.getHeight() - RESIZE_ZONE;
    }

    private static void botLoop(JFrame frame, FastGhostMouse ghost) {
        try {
            Thread.sleep(1000);
            fastrobot.FastRobot bot = new fastrobot.FastRobot();

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

                if (dist < 20 && !wrongSize) continue;

                // ErhÃ¶he den Timeout auf 1500ms! Da die Titelleiste vom Windows-Betriebssystem verwaltet wird,
                // sendet Java kein "mouseReleased" Event beim Loslassen der nativen Titelleiste.
                // Der Bot wartet nun 1,5 Sekunden, nachdem du aufhÃ¶rst das Fenster zu bewegen.
                if (System.currentTimeMillis() - lastUserAction < 1500) continue;

                botActive = true;

                // Random delay up to 500ms before correcting
                Thread.sleep((long) (Math.random() * 500));
                
                // Say "No" just before correcting
                setGhostText(ghost, "No");
                Thread.sleep(1000); // 1000ms statt 500ms
                setGhostText(ghost, null);

                if (winX != homeX || winY != homeY) {
                    // Move ghost to titlebar center
                    int targetX = winX + frame.getWidth() / 2;
                    int targetY = winY + TITLEBAR_HEIGHT / 2;

                    smoothMove(ghost, targetX, targetY, 300);

                    // Drag window home smoothly using FastTween
                    fasttween.Tween tweenX = fasttween.FastTween.to(winX, homeX, 600).ease(fasttween.Ease.QUAD_IN_OUT).start();
                    fasttween.Tween tweenY = fasttween.FastTween.to(winY, homeY, 600).ease(fasttween.Ease.QUAD_IN_OUT).start();
                    
                    while (tweenX.update() | tweenY.update()) {
                        int curX = (int) tweenX.currentValue();
                        int curY = (int) tweenY.currentValue();
                        
                        int gx = curX + frame.getWidth() / 2;
                        int gy = curY + TITLEBAR_HEIGHT / 2;
                        
                        frame.setLocation(curX, curY);
                        ghost.setCursorPositionImmediate(gx, gy);
                        
                        Thread.sleep(16);
                    }

                    winX = homeX;
                    winY = homeY;
                    frame.setLocation(winX, winY);
                }

                // Check again in case it was just moved
                wrongSize = frame.getWidth() != HOME_W || frame.getHeight() != HOME_H;

                // Resize if needed
                if (wrongSize) {
                    int grabX = winX + frame.getWidth() - 5;
                    int grabY = winY + frame.getHeight() - 5;
                    
                    smoothMove(ghost, grabX, grabY, 300);
                    bot.virtualMousePress(fastrobot.FastRobot.BUTTON1, grabX, grabY);
                    
                    int startW = frame.getWidth();
                    int startH = frame.getHeight();
                    
                    fasttween.Tween tweenW = fasttween.FastTween.to(startW, HOME_W, 600).ease(fasttween.Ease.QUAD_IN_OUT).start();
                    fasttween.Tween tweenH = fasttween.FastTween.to(startH, HOME_H, 600).ease(fasttween.Ease.QUAD_IN_OUT).start();
                    
                    while (tweenW.update() | tweenH.update()) {
                        int curW = (int) tweenW.currentValue();
                        int curH = (int) tweenH.currentValue();
                        
                        int gx = winX + curW - 5;
                        int gy = winY + curH - 5;
                        
                        frame.setSize(curW, curH);
                        ghost.setCursorPositionImmediate(gx, gy);
                        bot.virtualMouseMove(gx, gy);
                        
                        Thread.sleep(16);
                    }
                    
                    int finalGrabX = winX + HOME_W - 5;
                    int finalGrabY = winY + HOME_H - 5;
                    bot.virtualMouseRelease(fastrobot.FastRobot.BUTTON1, finalGrabX, finalGrabY);
                    frame.setSize(HOME_W, HOME_H);
                }

                // Park ghost
                smoothMove(ghost, winX - 100, winY + 50, 400);

                // Say "Yes" when done!
                setGhostText(ghost, "Yes");
                Thread.sleep(2000); // 2000ms statt 1000ms
                setGhostText(ghost, null);

                botActive = false;
                lastUserAction = System.currentTimeMillis();
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static void smoothMove(FastGhostMouse ghost, int tx, int ty, int durationMs) throws Exception {
        fasttween.Tween tweenX = fasttween.FastTween.to(ghost.currentX, tx, durationMs).ease(fasttween.Ease.QUAD_IN_OUT).start();
        fasttween.Tween tweenY = fasttween.FastTween.to(ghost.currentY, ty, durationMs).ease(fasttween.Ease.QUAD_IN_OUT).start();

        while (tweenX.update() | tweenY.update()) {
            ghost.moveTo(tweenX.currentValue(), tweenY.currentValue());
            Thread.sleep(16);
        }
    }

    private static void setGhostText(FastGhostMouse ghost, String text) {
        if (text == null) {
            ghost.setTextImage(null);
            return;
        }
        
        // Bubble size (75% margin)
        int bw = 36, bh = 20;
        // Image size (padded for shadow)
        int w = 52, h = 40;
        
        java.awt.image.BufferedImage img = new java.awt.image.BufferedImage(w, h, java.awt.image.BufferedImage.TYPE_INT_ARGB);
        Graphics2D g = img.createGraphics();
        g.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        
        int bx = 8;
        int by = 4;
        
        // Blurred shadow (+Y offset = 4, strictly NO X offset or X expansion)
        int shadowY = by + 4;
        for (int i = 5; i >= 1; i--) {
            g.setColor(new Color(0, 0, 0, 15));
            g.fillRoundRect(bx, shadowY - i, bw, bh + i * 2, 7, 7);
        }
        
        // Weisser Hintergrund (leicht transparent)
        g.setColor(new Color(255, 255, 255, 230));
        g.fillRoundRect(bx, by, bw, bh, 7, 7);
        
        // Schwarzer Text
        g.setColor(new Color(0, 0, 0));
        
        // Font size 12 is roughly 9pt which matches native titlebars. 
        // We use PLAIN to match default title bars (or BOLD if your Windows theme has it bold)
        g.setFont(new Font("Segoe UI", Font.PLAIN, 12));
        FontMetrics fm = g.getFontMetrics();
        int tx = bx + (bw - fm.stringWidth(text)) / 2;
        int ty = by + ((bh - fm.getHeight()) / 2) + fm.getAscent();
        
        g.drawString(text, tx, ty);
        g.dispose();
        ghost.setTextImage(img);
    }
}

