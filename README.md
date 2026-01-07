# 🕒 Tick Utilities

A lightweight **C++ timing and benchmarking library** featuring:

* Scope-based timers (`Timer`, `NamedTimer`)
* Manual `Stopwatch` with pause/resume
* Flexible time units: **seconds, milliseconds, microseconds, nanoseconds**
* Benchmarking functions with statistics & CSV export
* Output capturing (`snatch::Capture`, `Guard`, `DualCapture`, `Tee`, `Mute`)

Designed for precise profiling and easy integration into any C++ project.

---

## Features

### 1️⃣ Timer / NamedTimer

Scope-based timers that **start automatically** and print elapsed time when they go out of scope.

```cpp
{
    tick::Timer t(TimeUnit::Milliseconds); // prints ms by default
    heavy_task();
}

{
    tick::NamedTimer t("Loading Assets", TimeUnit::Microseconds);
    heavy_task();
}
```

**Supported units:**

* Seconds (`s`)
* Milliseconds (`ms`)
* Microseconds (`us`)
* Nanoseconds (`ns`)

---

### 2️⃣ Stopwatch

Manual timer with **start, pause, resume, stop** functionality.
Query elapsed time in any unit:

```cpp
Stopwatch sw;
sw.start();
heavy_task();
sw.pause();
std::this_thread::sleep_for(std::chrono::milliseconds(50)); // ignored
sw.resume();
heavy_task();
sw.stop();

std::cout << sw.elapsed_s()  << " s\n";
std::cout << sw.elapsed_ms() << " ms\n";
std::cout << sw.elapsed_us() << " us\n";
std::cout << sw.elapsed_ns() << " ns\n";
```

---

### 3️⃣ Benchmark

Run a function multiple times and report detailed statistics:

* Min / Max / Mean
* Standard deviation
* Percentiles: p50, p90, p95, p99
* Per-operation average
* CSV export with **Append or Truncate** modes
* Optional output in any time unit (s/ms/µs/ns)

```cpp
Benchmark bench(5000);
bench.batch_size = 5;

bench.run([]() {
    volatile int x = 0;
    for (int i = 0; i < 1000; ++i) x += i;
});

bench.print_stats();
bench.export_csv("benchmark_results.csv", Benchmark::CsvWriteMode::Append, TimeUnit::Microseconds);
```

---

### 4️⃣ Output Capture (`snatch` namespace)

Capture or redirect `std::cout` / `std::cerr` easily:

```cpp
std::string output;
snatch::Capture cap(output, []() {
    std::cout << "Hello, World!" << std::endl;
});
std::cout << "Captured: " << output;
```

**Other tools:**

* `snatch::Guard` – RAII capture in a scope
* `snatch::DualCapture` – capture both `cout` and `cerr`
* `snatch::Tee` – write to console **and** store in a string
* `snatch::Mute` – suppress console output temporarily

---

## Installation

1. Copy `utils.hpp` into your project.
2. Include the header:

```cpp
#include "utils.hpp"
```

1. Use the `tick::` and `snatch::` namespaces:

```cpp
using namespace tick;
using namespace snatch;
```

---

## Example `main.cpp`

```cpp
#include <iostream>
#include <thread>
#include "utils.hpp"

using namespace tick;

void heavy_task() {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

int main() {
    // Timer examples
    Timer t_ms(TimeUnit::Milliseconds);
    heavy_task();

    NamedTimer t_us("Tiny Task", TimeUnit::Microseconds);
    heavy_task();

    // Stopwatch example
    Stopwatch sw;
    sw.start();
    heavy_task();
    sw.stop();
    std::cout << "Elapsed: " << sw.elapsed_us() << " us\n";

    // Benchmark example
    Benchmark bench(5000);
    bench.batch_size = 5;
    bench.run([]() {
        volatile int x = 0;
        for (int i = 0; i < 1000; ++i) x += i;
    });
    bench.print_stats();
    bench.export_csv("results.csv", Benchmark::CsvWriteMode::Append, TimeUnit::Microseconds);

    return 0;
}
```

---

## Output Example

```text
Timer took 50.123ms
Tiny Task: 50123us
Elapsed: 50123000 ns

Benchmark results
  Iterations: 5000
  Batch size: 5
  Samples:    4980

  Min:   1.874 us
  Max:   4.069 us
  Mean:  1.946 us
  Stddev:0.045 us

  p50: 1.974 us
  p90: 2.072 us
  p95: 2.172 us
  p99: 2.272 us

  Per-op avg: 0.389 us
```

---

## License

MIT — free to use, modify, and distribute.
Created by **Ariel Zvinowanda**, 2026.

---
