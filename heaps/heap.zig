///  [1, 4, 6, 9 ,10 ,11 ,18]
///   0  1  2  3  4   5   6
///
///  parent 0 left 1 right 2
///  parent 1 left 3 right 4
///  parent 2 left 5 right 6
///  parent 3 left 7 right 8
///
///  left = (i * 2) + 1
///  right = (i * 2) + 2
///
///  if smaller than root, swap and pass root down
///
const std = @import("std");
const Io = std.Io;

fn lessThan(context: void, a: u8, b: u8) std.math.Order {
    _ = context;
    return std.math.order(a, b);
}
pub fn main(init: std.process.Init) !void {
    const io = init.io;

    var stdout_buffer: [1024]u8 = undefined;
    var stdout_file_writer: Io.File.Writer = .init(.stdout(), io, &stdout_buffer);
    const stdout_writer = &stdout_file_writer.interface;

    var prng: std.Random.DefaultPrng = .init(9999);
    const rand = prng.random();
    var heapbuffer: [100]u8 = std.Random.array(rand, u8, 100);
    var heap: std.PriorityQueue(u8, void, lessThan) = .fromOwnedSlice(&heapbuffer, {});
    while (heap.count() > 0) {
        const i = heap.pop().?;
        try stdout_writer.print("{d}", .{i});
        try stdout_writer.print(", ", .{});
    }
    try stdout_writer.print("\n", .{});
    try stdout_writer.flush();
}
