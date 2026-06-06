const std = @import("std");

pub fn main() !void {
    var stdout_buffer: [1024]u8 = undefined;
    var stdout_writer = std.fs.File.stdout().writer(&stdout_buffer);
    const stdout = &stdout_writer.interface;
    try stdout.print("======================\n", .{});

    var data1: [5]u32 = .{ 1, 2, 3, 4, 5 };
    const data2: [5]u32 = .{ 1, 2, 3, 4, 5 };
    const data3: [5]u32 = .{ 1, 2, 3, 4, 5 };

    for (&data1, data2, data3) |*x, y, z| {
        try stdout.print("{}, {}, {}\n", .{ x.*, y, z });
        x.* = 0;
    }
    for (data1, data2, data3) |x, y, z| {
        try stdout.print("{}, {}, {}\n", .{ x, y, z });
    }

    try stdout.flush();
}
