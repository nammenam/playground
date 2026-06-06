const std = @import("std");
const Io = std.Io;
const TrueType = @import("TrueType");

const tt = @import("ttf");

pub fn main(init: std.process.Init) !void {
    const ttf = try TrueType.load(@embedFile("Geist-Medium.otf"));
    const example_string = "こんにちは!";
    const scale = ttf.scaleForPixelHeight(20);
    var stdout_buffer: [1024]u8 = undefined;
    const w = std.Io.File.stdout().writer(init.io, &stdout_buffer);
    const stdout = &w.interface;
    var buffer: std.ArrayListUnmanaged(u8) = .empty;
    var it = std.unicode.Utf8View.initComptime(example_string).iterator();
    while (it.nextCodepoint()) |codepoint| {
        if (ttf.codepointGlyphIndex(codepoint)) |glyph| {
            std.log.debug("0x{d}: {d}", .{ codepoint, glyph });
            buffer.clearRetainingCapacity();
            const dims = try ttf.glyphBitmap(init.arena, &buffer, glyph, scale, scale);
            const pixels = buffer.items;
            for (0..dims.height) |j| {
                for (0..dims.width) |i| {
                    try stdout.writeByte(" .:ioVM@"[pixels[j * dims.width + i] >> 5]);
                }
                try stdout.writeByte('\n');
            }
        } else {
            std.log.debug("0x{d}: none", .{codepoint});
        }
    }
}
