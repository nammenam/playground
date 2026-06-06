const std = @import("std");

// 1. Define a function pointer type for readability
//    This defines a pointer to a function that takes two i32s and returns an i32.
const MathOp = *const fn (a: i32, b: i32) i32;
// 2. Define some matching functions
fn add(a: i32, b: i32) i32 {
    return a + b;
}

fn multiply(a: i32, b: i32) i32 {
    return a * b;
}

// 3. A function that accepts a callback (The "Caller")
fn compute(a: i32, b: i32, operation: MathOp) i32 {
    // We call the function pointer just like a normal function
    return operation(a, b);
}

// A generic function that iterates over numbers and calls a callback.
// notice we accept a 'context' of type 'anytype'.
fn forEach(
    items: []const i32,
    context: anytype,
    callback: *const fn (ctx: @TypeOf(context), item: i32, stdout: *std.Io.Writer) void,
    stdout: *std.Io.Writer,
) void {
    for (items) |item| {
        callback(context, item, stdout);
    }
}

// --- usage below ---

// A struct to hold our state
const SumState = struct {
    total: i32 = 0,
    count: i32 = 0,
};

// The callback function.
// Note: The first argument matches the type of the context we pass in.
fn sumCallback(ctx: *SumState, item: i32, stdout: *std.Io.Writer) void {
    ctx.total += item;
    ctx.count += 1;
    stdout.print("Added {d}, running total is {d}\n", .{ item, ctx.total }) catch {};
}

pub fn main() !void {
    var stdout_buffer: [1024]u8 = undefined;
    var stdout_writer = std.fs.File.stdout().writer(&stdout_buffer);
    const stdout = &stdout_writer.interface;
    try stdout.print("======================\n", .{});

    const x = 10;
    const y = 5;

    // Use the 'add' callback
    const sum = compute(x, y, add);
    try stdout.print("Sum: {d}\n", .{sum});

    // Use the 'multiply' callback
    const product = compute(x, y, multiply);
    try stdout.print("Product: {d}\n", .{product});

    // You can also hold the pointer in a variable
    var current_op: MathOp = add;
    current_op = multiply; // Switch behavior at runtime
    try stdout.print("Switched Op Result: {d}\n", .{current_op(x, y)});
    try stdout.print("===== with state =====\n", .{});

    const data = [_]i32{ 10, 20, 30, 40 };

    // Initialize our state
    var my_state = SumState{};

    // Pass the state (as a pointer) and the callback function
    forEach(data[0..], &my_state, sumCallback, stdout);

    try stdout.print("Final Result -- Count: {d}, Total: {d}\n", .{ my_state.count, my_state.total });
    try stdout.print("======================\n", .{});
    try stdout.flush();
}
