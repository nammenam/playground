const std = @import("std");
const math = std.math;
const c = @import("c");
const Window = @import("Window.zig");

// clip space -1 1 -1 1
// sceen space 0 2000 0 1600
const SCREENWIDTH = 2000;
const SCREENHEIGHT = 1236;

const Rect = struct {
    center_x: f32,
    center_y: f32,
    width: f32,
    height: f32,

    pub fn draw(self: *Rect, pixels: [*]u32, pitch: usize) void {
        const sw: f32 = @floatFromInt(SCREENWIDTH);
        const sh: f32 = @floatFromInt(SCREENHEIGHT);
        const aspect = sw / sh;
        const clipcx = self.center_x / 100;
        const clipcy = (self.center_y / 100) * aspect;
        const clipw = self.width / 100;
        const cliph = (self.height / 100) * aspect;
        if (clipcx > 1) return;
        if (clipcy > 1) return;

        const cx: u32 = @round(((clipcx + 1) / 2) * SCREENWIDTH);
        const cy: u32 = @round(((clipcy + 1) / 2) * SCREENHEIGHT);
        const w: u32 = @round(clipw * SCREENWIDTH / 2);
        const h: u32 = @round(cliph * SCREENHEIGHT / 2);
        const start_x = cx - (w >> 1);
        const end_x = cx + (w >> 1);
        const start_y = cy - (h >> 1);
        const end_y = cy + (h >> 1);

        for (start_x..end_x) |x| {
            for (start_y..end_y) |y| {
                const index = (y * pitch) + x;
                const r: u8 = 250;
                const g: u8 = 255;
                const b: u8 = 140;
                pixels[index] = (@as(u32, r) << 16) | (@as(u32, g) << 8) | @as(u32, b);
            }
        }
    }
};

const Circle = struct {
    center_x: f32,
    center_y: f32,
    radius: f32,

    pub fn draw(self: *Circle, pixels: [*]u32, pitch: usize) void {
        // draw the bounds and fill inside the circle
        const sw: f32 = @floatFromInt(SCREENWIDTH);
        const sh: f32 = @floatFromInt(SCREENHEIGHT);
        const aspect = sw / sh;

        const clipcx = self.center_x / 100;
        const clipcy = (self.center_y / 100) * aspect;
        const clipw = self.radius * 2 / 100;
        const cliph = (self.radius * 2 / 100) * aspect;
        const clipr = (self.radius / 100);

        const cx: u32 = @round(((clipcx + 1) / 2) * SCREENWIDTH);
        const cy: u32 = @round(((clipcy + 1) / 2) * SCREENHEIGHT);
        const w: u32 = @round(clipw * SCREENWIDTH / 2);
        const h: u32 = @round(cliph * SCREENHEIGHT / 2);
        const rad: f32 = clipr * SCREENWIDTH / 2;
        const start_x = cx - (w >> 1);
        const end_x = cx + (w >> 1);
        const start_y = cy - (h >> 1);
        const end_y = cy + (h >> 1);

        for (start_x..end_x) |x| {
            for (start_y..end_y) |y| {
                var x_f32: f32 = @floatFromInt(x);
                const cx_f32: f32 = @floatFromInt(cx);
                var y_f32: f32 = @floatFromInt(y);
                const cy_f32: f32 = @floatFromInt(cy);
                x_f32 -= cx_f32;
                y_f32 -= cy_f32;
                if (rad < @sqrt(x_f32 * x_f32 + y_f32 * y_f32)) continue;
                const index = (y * pitch) + x;
                const r: u8 = 150;
                const g: u8 = 155;
                const b: u8 = 155;
                pixels[index] = (@as(u32, r) << 16) | (@as(u32, g) << 8) | @as(u32, b);
            }
        }
    }
};

pub fn loop(io: std.Io, window: *Window) !void {
    window.toggleMouseCapture();
    const t_start = std.Io.Clock.awake.now(io);
    var t = std.Io.Clock.awake.now(io);
    var time: f32 = 0;
    var rect = Rect{ .center_x = 0, .center_y = 0, .width = 8, .height = 8 };
    var rect2 = Rect{ .center_x = 50, .center_y = 0, .width = 8, .height = 8 };
    var circ = Circle{ .center_x = 50, .center_y = 0, .radius = 4 };
    var rect_theta: f32 = 0;

    while (!window.state.quit) {
        const dt = @as(f32, @floatFromInt(t.untilNow(io, .awake).toNanoseconds())) / 1_000_000_000.0;

        rect_theta += dt * 1.0;
        rect.center_x = @cos(2.0 * math.pi * rect_theta) * 20;
        rect.center_y = @sin(2.0 * math.pi * rect_theta) * 20;

        t = std.Io.Clock.awake.now(io);
        time = @as(f32, @floatFromInt(t_start.untilNow(io, .awake).toNanoseconds())) / 1_000_000_000.0;

        window.processInput();

        // 1. Get the surface from the window
        const surface = c.SDL_GetWindowSurface(window.handle) orelse {
            @panic("Failed to get window surface");
        };

        // 2. Cast the raw c_void pointer to a Zig slice of u32 (assuming 32-bit pixels)
        // We use the surface's pitch to handle potential padding at the end of rows
        const pixels = @as([*]u32, @ptrCast(@alignCast(surface.*.pixels)));
        const pitch_in_pixels = @as(usize, @intCast(surface.*.pitch)) / @sizeOf(u32);
        const w = @as(usize, @intCast(surface.*.w));
        const h = @as(usize, @intCast(surface.*.h));

        // 3. Draw directly to the surface memory
        for (0..h) |y| {
            for (0..w) |x| {
                const index = (y * pitch_in_pixels) + x;

                const r: u8 = 28;
                const g: u8 = 28;
                const b: u8 = 28;

                // Pack the color. (Format depends on the surface format, usually ARGB or ABGR)
                pixels[index] = (@as(u32, r) << 16) | (@as(u32, g) << 8) | @as(u32, b);
            }
        }
        rect.draw(pixels, pitch_in_pixels);
        rect2.draw(pixels, pitch_in_pixels);
        circ.draw(pixels, pitch_in_pixels);

        // 4. Tell SDL we are done modifying the surface and it should display it
        _ = c.SDL_UpdateWindowSurface(window.handle);
    }
}

pub fn main(init: std.process.Init) !void {
    const io = init.io;
    var window: Window = .init(SCREENWIDTH, SCREENHEIGHT);
    defer window.deinit();

    // No allocator needed for the loop anymore
    try loop(io, &window);
}
