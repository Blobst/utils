# Utilities

A lightweight C++ header providing:

- Scope-based timers (`tick::Timer`, `tick::NamedTimer`)
- Manual `tick::Stopwatch` with pause/resume
- Benchmarking with statistics and CSV export (ms)
- Output capturing tools (`snatch::Capture`, `Guard`, `DualCapture`, `Tee`, `Mute`)
- String-builder utility `snatch::Repent` (value + before/after text or callables)

Designed for precise profiling and easy drop-in use.

---

## Features

### 1️⃣ Timer / NamedTimer

Scope-based timers that start on construction and print elapsed time on destruction.

```cpp
{
    tick::Timer t(tick::TimeUnit::Milliseconds);
    heavy_task();
}

{
    tick::NamedTimer t("Loading Assets", tick::TimeUnit::Microseconds);
    heavy_task();
}
```

Supported units: seconds (`s`), milliseconds (`ms`), microseconds (`us`), nanoseconds (`ns`).

---

### 2️⃣ Stopwatch

Manual timer with start, pause, resume, and stop. Query in multiple units:

```cpp
tick::Stopwatch sw;
sw.start();
heavy_task();
sw.pause();
std::this_thread::sleep_for(std::chrono::milliseconds(50)); // ignored while paused
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

Run a function many times, then report stats in milliseconds:

- Min / Max / Mean
- Standard deviation
- Percentiles: p50, p90, p95, p99
- Per-operation average
- CSV export with Append or Truncate modes

```cpp
tick::Benchmark bench(5000);
bench.batch_size = 5;

bench.run([]() {
    volatile int x = 0;
    for (int i = 0; i < 1000; ++i) x += i;
});

bench.print_stats();
bench.export_csv("benchmark_results.csv", tick::Benchmark::CsvWriteMode::Append);
```

CSV columns: `Iterations,BatchSize,SamplesUsed,Min(ms),Max(ms),Mean(ms),Stddev(ms),p50(ms),p90(ms),p95(ms),p99(ms),PerOpAvg(ms)`.

---

### 4️⃣ Output Capture (snatch)

Capture or redirect `std::cout` / `std::cerr`:

```cpp
std::string output;
snatch::Capture cap(output, [](){ std::cout << "Hello, World!\n"; });
std::cout << "Captured: " << output;
```

Other tools:

- `snatch::Guard` – RAII capture within a scope
- `snatch::DualCapture` – capture both cout and cerr
- `snatch::Tee` – mirror to console and string
- `snatch::Mute` – temporarily silence console output

---

### 5️⃣ Repent (string builder)

Build strings by combining a value with text or callables. Two forms:

```cpp
// 1) value + after
snatch::Repent r1(42, " units");           // -> "42 units"
snatch::Repent r2(3.14, []{ return " pi"; }); // -> "3.14 pi"

// 2) before + value + after
snatch::Repent r3(100, "[", "]");        // -> "[100]"
snatch::Repent r4(
    std::string{"temp"},
    []{ return std::string{"<"}; },
    []{ return std::chrono::system_clock::now(); }
);
std::cout << r1 << "\n" << r2 << "\n" << r3 << "\n" << r4 << '\n';
```

Callables may return strings, numbers, or `std::chrono::system_clock::time_point` (formatted).

---

## Installation

1. Copy utils.hpp into your project.
2. Include the header:

```cpp
#include "utils.hpp"
```

1. Optionally bring namespaces into scope:

```cpp
using namespace tick;
using namespace snatch;
```

---

## Example main.cpp

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
    bench.export_csv("benchmark_results.csv", Benchmark::CsvWriteMode::Append);

    return 0;
}
```

Build example (GCC/Clang):

```bash
g++ -std=c++20 -O2 -Wall -Wextra -o app main.cpp
```

MSVC (Developer Command Prompt):

```bat
cl /std:c++20 /O2 /W4 main.cpp
```

---

## Output Example

```text
Timer took 50.12ms
Tiny Task: 50123us
Elapsed: 50123 us

Benchmark results
  Iterations: 5000
  Batch size: 5
  Samples:    4980

  Min:   1.87 ms
  Max:   4.06 ms
  Mean:  1.95 ms
  Stddev:0.05 ms

  p50: 1.97 ms
  p90: 2.07 ms
  p95: 2.17 ms
  p99: 2.27 ms

  Per-op avg: 0.39 ms
```

---

## License

MIT — free to use, modify, and distribute.
Created by Ariel Zvinowanda, 2026.

---
