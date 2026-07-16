package fastghostmouse;

import fastrobot.FastRobot;

public class FastGhostMouseBot {
    private final FastRobot bot;
    private boolean dragging = false;

    public FastGhostMouseBot() { this.bot = new FastRobot(); }

    public void softPress(int x, int y) {
        if (!dragging) {
            bot.virtualMousePress(FastRobot.BUTTON1, x, y);
            dragging = true;
        }
    }

    public void softDrag(int x, int y) {
        if (dragging) bot.virtualMouseMove(x, y);
    }

    public void softRelease(int x, int y) {
        if (dragging) {
            bot.virtualMouseRelease(FastRobot.BUTTON1, x, y);
            dragging = false;
        }
    }

    public boolean isDragging() { return dragging; }
}
