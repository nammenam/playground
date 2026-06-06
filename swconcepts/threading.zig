const std = @import("std");

// We add a semaphore to our shared state.
const SharedState = struct {
    counter: u32,
    sem: std.Thread.Semaphore,
};

// The function each thread will run.
fn safeIncrementer(state: *SharedState) void {
    var i: u32 = 0;
    while (i < 1_000_000) : (i += 1) {
        // 1. Acquire the semaphore.
        // `wait()` decrements the semaphore's internal count. If the count is 0,
        // this thread will block (wait) until another thread calls `post()`.
        state.sem.wait();

        // 2. We now have exclusive access to this critical section.
        // It's guaranteed no other thread can be between `wait()` and `post()`.
        state.counter += 1;

        // 3. Release the semaphore.
        // `post()` increments the semaphore's count, allowing another waiting
        // thread (if any) to proceed.
        state.sem.post();
    }
}

pub fn main() !void {
    // Initialize our shared state.
    // We create the semaphore with an initial permit count of 1.
    var state = SharedState{
        .counter = 0,
        .sem = .{ .permits = 1 },
    };

    std.log.info("Spawning two threads to increment a counter one million times each...", .{});
    std.log.info("Expected result: 2,000,000", .{});

    const thread1 = try std.Thread.spawn(.{}, safeIncrementer, .{&state});
    const thread2 = try std.Thread.spawn(.{}, safeIncrementer, .{&state});

    // Wait for both to finish.
    thread1.join();
    thread2.join();

    std.log.info("Actual result:   {d}", .{state.counter});
    if (state.counter == 2_000_000) {
        std.log.info("✅ The semaphore successfully prevented the race condition!", .{});
    }
}
