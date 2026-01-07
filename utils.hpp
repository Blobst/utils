/*
 ███████████  ████           █████              █████
░░███░░░░░███░░███          ░░███              ░░███
 ░███    ░███ ░███   ██████  ░███████   █████  ███████
 ░██████████  ░███  ███░░███ ░███░░███ ███░░  ░░░███░
 ░███░░░░░███ ░███ ░███ ░███ ░███ ░███░░█████   ░███
 ░███    ░███ ░███ ░███ ░███ ░███ ░███ ░░░░███  ░███ ███
 ███████████  █████░░██████  ████████  ██████   ░░█████
░░░░░░░░░░░  ░░░░░  ░░░░░░  ░░░░░░░░  ░░░░░░     ░░░░░

 * @file utils.hpp
 * @brief Utility classes for timing, output capturing, and file archiving.
 * @author Ariel Zvinowanda
 * @date January 6, 2026
 * @note This library was made by the very very smart and cool Ariel zvinowanda

 */

#pragma once
#include <algorithm> // sort, min_element, max_element
#include <chrono>
#include <cmath>   // sqrt
#include <cstddef> // size_t
#include <cstdint>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

/* usings */
using steady = std::chrono::steady_clock;

namespace tick {
	/**
	 * @brief Supported time display units for timers.
	 *
	 * This enum controls the unit used when reporting measured time.
	 */
	enum class TimeUnit {
		Seconds,	  ///< Display time in seconds (s)
		Milliseconds, ///< Display time in milliseconds (ms)
		Microseconds, ///< Display time in microseconds (us)
		Nanoseconds	  ///< Display time in nanoseconds (ns)
	};

	static inline std::string format_duration(std::chrono::duration<double> d, TimeUnit u) {
		switch (u) {
		case TimeUnit::Seconds:
			return std::to_string(d.count()) + "s";
		case TimeUnit::Milliseconds:
			return std::to_string(d.count() * 1'000.0) + "ms";
		case TimeUnit::Microseconds:
			return std::to_string(d.count() * 1'000'000.0) + "us";
		case TimeUnit::Nanoseconds:
			return std::to_string(d.count() * 1'000'000'000.0) + "ns";
		}
		return "??";
	}

	/**
	 * @brief Simple scope-based timer that measures elapsed time and prints it.
	 *
	 * The timer starts automatically when constructed and prints the elapsed time
	 * when destroyed (typically when leaving scope). The time unit can be selected
	 * using TimeUnit.
	 *
	 * @example
	 * ```cpp
	 * {
	 *     tick::Timer t(TimeUnit::Milliseconds); // default
	 *     // code...
	 * } // prints "Timer took XXms"
	 *
	 * {
	 *     tick::Timer t(TimeUnit::Microseconds);
	 *     // code...
	 * } // prints "Timer took XXus"
	 *
	 * {
	 *     tick::Timer t(TimeUnit::Nanoseconds);
	 *     // code...
	 * } // prints "Timer took XXns"
	 * ```
	 */

	struct Timer {
		TimeUnit unit;
		std::chrono::time_point<steady> start;

		Timer(TimeUnit u = TimeUnit::Milliseconds)
			: unit(u), start(steady::now()) {}

		~Timer() {
			auto end = steady::now();
			auto duration = std::chrono::duration<double>(end - start);
			std::cout << "Timer took " << format_duration(duration, unit) << std::endl;
		}
	};

	/**
	 * @brief Scope-based timer that prints elapsed time with a custom label.
	 *
	 * Works the same as Timer but allows naming the timed section.
	 * Useful for profiling multiple independent code parts.
	 *
	 * @param label Descriptive timer name
	 * @param unit  Output time unit (Seconds, ms, us, ns)
	 *
	 * @example
	 * ```cpp
	 * {
	 *     tick::NamedTimer load("Asset Load", TimeUnit::Milliseconds);
	 *     // asset load...
	 * } // prints: "Asset Load: XXms"
	 *
	 * {
	 *     tick::NamedTimer physics("Physics Step", TimeUnit::Microseconds);
	 * } // prints: "Physics Step: XXus"
	 *
	 * {
	 *     tick::NamedTimer tiny("Nano Task", TimeUnit::Nanoseconds);
	 * } // prints: "Nano Task: XXns"
	 * ```
	 */

	struct NamedTimer {
		std::string label;
		TimeUnit unit;
		std::chrono::time_point<steady> start;

		NamedTimer(const std::string &name,
				   TimeUnit u = TimeUnit::Milliseconds)
			: label(name), unit(u), start(steady::now()) {}

		~NamedTimer() {
			auto end = steady::now();
			auto duration = std::chrono::duration<double>(end - start);
			std::cout << label << ": " << format_duration(duration, unit) << std::endl;
		}
	};

	/**
	 * @brief Manual stopwatch with start, pause, resume, and stop control.
	 *
	 * Unlike Timer/NamedTimer, Stopwatch does not automatically print time and
	 * does not select a single unit. Instead, the caller can query elapsed time
	 * in seconds, milliseconds, microseconds, or nanoseconds.
	 *
	 * @example
	 * ```cpp
	 * tick::Stopwatch sw;
	 *
	 * sw.start();
	 * // work...
	 * sw.pause();
	 *
	 * // paused time does not count
	 *
	 * sw.resume();
	 * // more work...
	 * sw.stop();
	 *
	 * std::cout << sw.elapsed_s()  << " s\n";
	 * std::cout << sw.elapsed_ms() << " ms\n";
	 * std::cout << sw.elapsed_us() << " us\n";
	 * std::cout << sw.elapsed_ns() << " ns\n";
	 * ```
	 */

	struct Stopwatch {
		std::chrono::time_point<steady> start_time;
		std::chrono::duration<double> accumulated{};
		bool running = false;

		void start() {
			accumulated = std::chrono::duration<double>(0);
			start_time = steady::now();
			running = true;
		}

		void pause() {
			if (running) {
				accumulated += steady::now() - start_time;
				running = false;
			}
		}

		void resume() {
			if (!running) {
				start_time = steady::now();
				running = true;
			}
		}

		void stop() {
			if (running) {
				accumulated += steady::now() - start_time;
				running = false;
			}
		}

	private:
		std::chrono::duration<double> total() const {
			auto t = accumulated;
			if (running)
				t += steady::now() - start_time;
			return t;
		}

	public:
		double elapsed_s() const { return total().count(); }
		double elapsed_ms() const { return total().count() * 1'000.0; }
		double elapsed_us() const { return total().count() * 1'000'000.0; }
		double elapsed_ns() const { return total().count() * 1'000'000'000.0; }
	};

	/**
	 * @brief Benchmark a function by running it multiple times and reporting statistics.
	 *
	 * @example
	 * ```cpp
	 * tick::Benchmark bench(1000); // 1000 iterations
	 * bench.run([]() {
	 *     // code to benchmark
	 *     std::vector<int> v(100);
	 * });
	 * bench.print_stats();
	 * ```
	 */
	struct Benchmark {
		enum class CsvWriteMode {
			Truncate, // overwrite (default)
			Append	  // like std::ios::app
		};

		using clock_t = std::chrono::steady_clock;

		size_t iterations = 10'000;
		size_t warmup = 1'000;
		size_t batch_size = 1;

		std::vector<double> samples;

		explicit Benchmark(size_t iters = 10'000)
			: iterations(iters) {
			samples.reserve(iters);
		}

		/* ================= Compiler Barriers ================= */

		template <typename T>
		static inline void DoNotOptimize(const T &value) {
#if defined(_MSC_VER)
			volatile const T *p = &value;
			(void)p;
#else
			asm volatile("" : : "g"(value) : "memory");
#endif
		}

		static inline void ClobberMemory() {
#if defined(_MSC_VER)
			_ReadWriteBarrier();
#else
			asm volatile("" : : : "memory");
#endif
		}

		/* ================= Statistics Helpers ================= */

		static double mean(const std::vector<double> &v) {
			double sum = 0.0;
			for (double x : v)
				sum += x;
			return sum / v.size();
		}

		static double stddev(const std::vector<double> &v, double avg) {
			double sum = 0.0;
			for (double x : v)
				sum += (x - avg) * (x - avg);
			return std::sqrt(sum / v.size());
		}

		static double percentile(std::vector<double> v, double p) {
			std::sort(v.begin(), v.end());
			size_t idx = static_cast<size_t>(p * (v.size() - 1));
			return v[idx];
		}

		static std::vector<double> filter_outliers(std::vector<double> v) {
			std::sort(v.begin(), v.end());

			double q1 = percentile(v, 0.25);
			double q3 = percentile(v, 0.75);
			double iqr = q3 - q1;

			double lo = q1 - 1.5 * iqr;
			double hi = q3 + 1.5 * iqr;

			std::vector<double> out;
			for (double x : v)
				if (x >= lo && x <= hi)
					out.push_back(x);
			return out;
		}

		/* ================= Overhead ================= */

		double measure_overhead() const {
			double total = 0.0;
			for (size_t i = 0; i < iterations; ++i) {
				auto s = clock_t::now();
				auto e = clock_t::now();
				std::chrono::duration<double, std::milli> d = e - s;
				total += d.count();
			}
			return total / iterations;
		}

		/* ================= Runner ================= */

		template <typename F>
		void run(F &&func) {
			samples.clear();

			for (size_t i = 0; i < warmup; ++i)
				for (size_t j = 0; j < batch_size; ++j)
					func();

			double overhead = measure_overhead();

			for (size_t i = 0; i < iterations; ++i) {
				auto start = clock_t::now();

				for (size_t j = 0; j < batch_size; ++j) {
					func();
					ClobberMemory();
				}

				auto end = clock_t::now();
				std::chrono::duration<double, std::milli> d = end - start;

				double corrected = d.count() - overhead;
				samples.push_back(corrected < 0 ? 0 : corrected);
			}
		}

		/* ================= Report ================= */

		void print_stats() const {
			if (samples.empty())
				return;

			auto clean = filter_outliers(samples);

			double min_t = *std::min_element(clean.begin(), clean.end());
			double max_t = *std::max_element(clean.begin(), clean.end());
			double avg = mean(clean);
			double sd = stddev(clean, avg);

			std::cout << "Benchmark results\n";
			std::cout << "  Iterations: " << iterations << "\n";
			std::cout << "  Batch size: " << batch_size << "\n";
			std::cout << "  Samples:    " << clean.size() << "\n\n";

			std::cout << "  Min:   " << min_t << " ms\n";
			std::cout << "  Max:   " << max_t << " ms\n";
			std::cout << "  Mean:  " << avg << " ms\n";
			std::cout << "  Stddev:" << sd << " ms\n\n";

			std::cout << "  p50: " << percentile(clean, 0.50) << " ms\n";
			std::cout << "  p90: " << percentile(clean, 0.90) << " ms\n";
			std::cout << "  p95: " << percentile(clean, 0.95) << " ms\n";
			std::cout << "  p99: " << percentile(clean, 0.99) << " ms\n\n";

			std::cout << "  Per-op avg: "
					  << (avg / batch_size) << " ms\n";
		}

		void export_csv(const std::string &filename,
						CsvWriteMode mode = CsvWriteMode::Truncate) const {
			if (samples.empty())
				return;

			auto clean = filter_outliers(samples);

			double min_t = *std::min_element(clean.begin(), clean.end());
			double max_t = *std::max_element(clean.begin(), clean.end());
			double avg = mean(clean);
			double sd = stddev(clean, avg);

			bool write_header = true;

			if (mode == CsvWriteMode::Append && std::filesystem::exists(filename))
				write_header = false;

			std::ofstream file(
				filename,
				mode == CsvWriteMode::Append ? std::ios::app : std::ios::trunc);

			if (!file)
				return;

			file << std::setprecision(10);

			/* ================= Header ================= */

			if (write_header) {
				file << "Iterations,BatchSize,SamplesUsed,"
					 << "Min(ms),Max(ms),Mean(ms),Stddev(ms),"
					 << "p50(ms),p90(ms),p95(ms),p99(ms),PerOpAvg(ms)\n";
			}

			/* ================= Row ================= */

			file << iterations << ","
				 << batch_size << ","
				 << clean.size() << ","
				 << min_t << ","
				 << max_t << ","
				 << avg << ","
				 << sd << ","
				 << percentile(clean, 0.50) << ","
				 << percentile(clean, 0.90) << ","
				 << percentile(clean, 0.95) << ","
				 << percentile(clean, 0.99) << ","
				 << (avg / batch_size) << "\n";
		}
	};

} // namespace tick

namespace snatch {

	/**
	 * @brief Captures std::cout output from a function into a string variable.
	 * @example
	 * ```cpp
	 * std::string output;
	 * snatch::Capture cap(output, []() {
	 *     std::cout << "Hello, World!" << std::endl;
	 * });
	 * std::cout << "Captured: " << output; // prints: Captured: Hello, World!
	 * ```
	 */
	struct Capture {
		std::string &var;			///< Reference to store captured output
		std::function<void()> func; ///< Function that prints to std::cout

		/**
		 * @brief Construct a new capture object and immediately capture the function output.
		 * @param variable Reference to a string where the captured output will be stored.
		 * @param f Function that prints something to std::cout.
		 */
		Capture(std::string &variable, std::function<void()> f)
			: var(variable), func(f) {
			update(); // capture output immediately
		}

		/**
		 * @brief Capture the output of the stored function and save it to `var`.
		 *
		 * Can be called multiple times to refresh the captured content.
		 */
		void update() {
			std::ostringstream oss;
			std::streambuf *oldCout = std::cout.rdbuf();

			std::cout.rdbuf(oss.rdbuf()); // Redirect cout
			func();						  // Execute function
			std::cout.rdbuf(oldCout);	  // Restore cout

			var = oss.str(); // Store output
		}
	};

	/**
	 * @brief A wrapper that converts a value to text and appends either a string
	 *        or the result of a callable (function / lambda).
	 *
	 * This struct supports two construction forms:
	 *
	 * 1️⃣ Value + Text
	 *    VarAppender(value, "text");
	 *
	 * 2️⃣ Value + Callable
	 *    VarAppender(value, callable);
	 *
	 * @param v
	 *        The first argument. Any value streamable to std::ostream
	 *        (int, double, std::string, etc.).
	 *
	 * @param text
	 *        A std::string that is appended directly after the value.
	 *
	 * @param fn
	 *        A callable taking no parameters and returning a value to append.
	 *        Supported return types:
	 *          - std::string → appended directly
	 *          - numeric types → converted to string
	 *          - std::chrono::system_clock::time_point → formatted datetime
	 * @note also repent to the lord for using this struct ngl, you are a horrible person if you use this struct, because you are lazy to make your own structs, wait, since i made this struct, does it make me a horrible person?. who knows.
	 * @note i ate barbacue chicken
	 */
	struct Repent {
		std::string value;

		/**
		 * @brief Construct by appending a string or callable after the value
		 *
		 * @tparam T Type of the main value (int, double, string, etc.)
		 * @tparam S Type of value to append (string or callable)
		 * @param v The main value
		 * @param after The value to append after v (string or callable)
		 */
		template <typename T, typename S>
		Repent(const T &v, S after)
			requires(std::is_invocable_v<S> || std::is_convertible_v<S, std::string>)
		{
			value = toString(v) + toString(after);
		}

		/**
		 * @brief Construct by prepending and appending values (strings or callables)
		 *
		 * @tparam T Type of the main value
		 * @tparam B Type of the value to prepend (string or callable)
		 * @tparam A Type of the value to append (string or callable)
		 * @param v The main value
		 * @param before Value to prepend before v
		 * @param after Value to append after v
		 */
		template <typename T, typename B, typename A>
		Repent(const T &v, B before, A after)
			requires((std::is_invocable_v<B> || std::is_convertible_v<B, std::string>) &&
					 (std::is_invocable_v<A> || std::is_convertible_v<A, std::string>))
		{
			value = toString(before) + toString(v) + toString(after);
		}

	private:
		// Helper to convert any value to string, including callables and time_point
		template <typename U>
		static std::string toString(const U &val) {
			std::ostringstream oss;

			if constexpr (std::is_invocable_v<U>) {
				auto result = val();
				std::string s = toString(result);
				// Add space before callable result unless it's a time_point (@ already has space)
				if (!s.empty() && s[0] != '@')
					s = " " + s;
				return s;
			} else if constexpr (std::is_same_v<U, std::chrono::system_clock::time_point>) {
				std::time_t t = std::chrono::system_clock::to_time_t(val);
				oss << " @ " << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
			} else {
				oss << val;
			}

			return oss.str();
		}

	public:
		/// @brief Allow direct printing via std::cout
		friend std::ostream &operator<<(std::ostream &os, const Repent &v) {
			return os << v.value;
		}
	};

	/**
	 * @brief Scope-based RAII wrapper that redirects std::cout to a string.
	 *
	 * @example
	 * ```cpp
	 * std::string output;
	 * {
	 *     snatch::Guard guard(output);
	 *     std::cout << "This is captured!" << std::endl;
	 *     std::cout << "So is this!" << std::endl;
	 * } // output now contains both lines
	 * std::cout << "Captured: " << output;
	 * ```
	 */
	struct Guard {
		std::ostringstream buffer;		 ///< Internal buffer for capturing output
		std::streambuf *original_buffer; ///< Original cout buffer
		std::string &destination;		 ///< Reference to store captured output

		/**
		 * @brief Construct a Guard and begin capturing std::cout.
		 * @param dest Reference to string where captured output will be stored.
		 */
		Guard(std::string &dest) : destination(dest) {
			original_buffer = std::cout.rdbuf();
			std::cout.rdbuf(buffer.rdbuf());
		}

		/**
		 * @brief Destroy the Guard, restore std::cout, and save captured output.
		 */
		~Guard() {
			std::cout.rdbuf(original_buffer);
			destination = buffer.str();
		}
	};

	/**
	 * @brief Captures both std::cout and std::cerr into separate strings.
	 *
	 * @example
	 * ```cpp
	 * std::string out, err;
	 * snatch::DualCapture cap(out, err, []() {
	 *     std::cout << "Normal output" << std::endl;
	 *     std::cerr << "Error output" << std::endl;
	 * });
	 * std::cout << "Out: " << out << ", Err: " << err;
	 * ```
	 */
	struct DualCapture {
		std::string &cout_var;		///< Reference for std::cout output
		std::string &cerr_var;		///< Reference for std::cerr output
		std::function<void()> func; ///< Function to execute

		/**
		 * @brief Construct and immediately capture both stdout and stderr.
		 * @param cout_dest Reference to string for std::cout output.
		 * @param cerr_dest Reference to string for std::cerr output.
		 * @param f Function to execute while capturing.
		 */
		DualCapture(std::string &cout_dest, std::string &cerr_dest, std::function<void()> f)
			: cout_var(cout_dest), cerr_var(cerr_dest), func(f) {
			update();
		}

		/**
		 * @brief Execute function and capture both output streams.
		 */
		void update() {
			std::ostringstream cout_oss, cerr_oss;
			std::streambuf *old_cout = std::cout.rdbuf();
			std::streambuf *old_cerr = std::cerr.rdbuf();

			std::cout.rdbuf(cout_oss.rdbuf());
			std::cerr.rdbuf(cerr_oss.rdbuf());
			func();
			std::cout.rdbuf(old_cout);
			std::cerr.rdbuf(old_cerr);

			cout_var = cout_oss.str();
			cerr_var = cerr_oss.str();
		}
	};

	/**
	 * @brief Silences std::cout by redirecting to nowhere (like /dev/null).
	 *
	 * @example
	 * ```cpp
	 * {
	 *     snatch::Mute mute;
	 *     std::cout << "This won't be printed"; // suppressed
	 * } // cout restored
	 * std::cout << "This will print"; // visible again
	 * ```
	 */
	struct Mute {
		std::streambuf *original_buffer; ///< Original cout buffer

		/**
		 * @brief Construct and begin muting std::cout.
		 */
		Mute() {
			original_buffer = std::cout.rdbuf();
			std::cout.rdbuf(nullptr);
		}

		/**
		 * @brief Destroy and restore std::cout.
		 */
		~Mute() {
			std::cout.rdbuf(original_buffer);
		}
	};

	/**
	 * @brief Tee output - writes to both std::cout and captures to a string.
	 *
	 * @example
	 * ```cpp
	 * std::string log;
	 * {
	 *     snatch::Tee tee(log);
	 *     std::cout << "Visible and captured!" << std::endl;
	 * }
	 * // Displays on console AND stores in log
	 * ```
	 */
	struct Tee {
		std::ostringstream buffer;		 ///< Internal buffer
		std::streambuf *original_buffer; ///< Original cout buffer
		std::string &destination;		 ///< Reference to store output

		/**
		 * @brief Custom streambuf that writes to both original and buffer.
		 */
		class TeeBuffer : public std::streambuf {
		public:
			std::streambuf *buf1;
			std::streambuf *buf2;

			TeeBuffer(std::streambuf *b1, std::streambuf *b2) : buf1(b1), buf2(b2) {}

		protected:
			int overflow(int c) override {
				if (c == EOF)
					return !EOF;
				buf1->sputc(c);
				return buf2->sputc(c);
			}

			int sync() override {
				buf1->pubsync();
				return buf2->pubsync();
			}
		};

		TeeBuffer *tee_buffer; ///< Custom tee buffer instance

		/**
		 * @brief Construct a Tee and begin dual output.
		 * @param dest Reference to string for capturing output.
		 */
		Tee(std::string &dest) : destination(dest) {
			original_buffer = std::cout.rdbuf();
			tee_buffer = new TeeBuffer(original_buffer, buffer.rdbuf());
			std::cout.rdbuf(tee_buffer);
		}

		/**
		 * @brief Destroy the Tee, restore std::cout, and save captured output.
		 */
		~Tee() {
			std::cout.rdbuf(original_buffer);
			destination = buffer.str();
			delete tee_buffer;
		}
	};

} // namespace snatch