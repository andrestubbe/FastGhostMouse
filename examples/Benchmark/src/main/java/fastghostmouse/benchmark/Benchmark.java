package fastghostmouse.benchmark;

import fastghostmouse.FastGhostMouse;
import org.openjdk.jmh.annotations.*;

import java.util.concurrent.TimeUnit;

@BenchmarkMode(Mode.Throughput)
@OutputTimeUnit(TimeUnit.MILLISECONDS)
@State(Scope.Benchmark)
@Warmup(iterations = 2, time = 1, timeUnit = TimeUnit.SECONDS)
@Measurement(iterations = 3, time = 1, timeUnit = TimeUnit.SECONDS)
@Fork(1)
public class Benchmark {

    private FastGhostMouse ghostMouse;

    @Setup
    public void setup() {
        try {
            ghostMouse = new FastGhostMouse();
        } catch (Throwable t) {
            ghostMouse = null;
        }
    }

    @org.openjdk.jmh.annotations.Benchmark
    public void benchmarkSetTextOffset() {
        if (ghostMouse != null) {
            ghostMouse.setTextOffset(15.0f, -25.0f);
        }
    }
}
