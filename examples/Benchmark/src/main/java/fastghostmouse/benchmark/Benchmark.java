package fastghostmouse.benchmark;

import fastghostmouse.FastGhostMouse;
import org.openjdk.jmh.annotations.*;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.util.concurrent.TimeUnit;

@BenchmarkMode(Mode.Throughput)
@OutputTimeUnit(TimeUnit.MILLISECONDS)
@State(Scope.Benchmark)
@Warmup(iterations = 2, time = 1, timeUnit = TimeUnit.SECONDS)
@Measurement(iterations = 3, time = 1, timeUnit = TimeUnit.SECONDS)
@Fork(1)
public class Benchmark {

    private FastGhostMouse ghostMouse;
    private BufferedImage cursorImg;
    private float moveTarget = 100.0f;

    @Setup(Level.Trial)
    public void setup() {
        ghostMouse = new FastGhostMouse();
        ghostMouse.init(100, 100);

        cursorImg = new BufferedImage(32, 32, BufferedImage.TYPE_INT_ARGB);
        Graphics2D g = cursorImg.createGraphics();
        g.setColor(new Color(255, 0, 0, 200));
        g.fillOval(0, 0, 32, 32);
        g.dispose();
    }

    @TearDown(Level.Trial)
    public void tearDown() {
        if (ghostMouse != null) {
            ghostMouse.dispose();
        }
    }

    @org.openjdk.jmh.annotations.Benchmark
    public void benchmarkMoveToTarget() {
        moveTarget = (moveTarget > 800.0f) ? 100.0f : moveTarget + 2.0f;
        ghostMouse.moveTo(moveTarget, moveTarget);
    }

    @org.openjdk.jmh.annotations.Benchmark
    public void benchmarkSetCursorPositionImmediate() {
        ghostMouse.setCursorPositionImmediate(250, 250);
    }

    @org.openjdk.jmh.annotations.Benchmark
    public void benchmarkSetTextOffset() {
        ghostMouse.setTextOffset(15.0f, -25.0f);
    }

    @org.openjdk.jmh.annotations.Benchmark
    public void benchmarkSetSmoothing() {
        ghostMouse.setSmoothing(0.18f);
    }

    @org.openjdk.jmh.annotations.Benchmark
    public void benchmarkUpdateCursorImage() {
        ghostMouse.setCursorImage(cursorImg);
    }
}
