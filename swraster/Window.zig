const c = @import("c");
const std = @import("std");

const Self = @This();

handle: *c.SDL_Window,
width: u32,
height: u32,
state: State = .{},

pub fn init(width: u32, height: u32) Self {
    checkSdl(c.SDL_Init(c.SDL_INIT_VIDEO));
    const flags = c.SDL_WINDOW_RESIZABLE | c.SDL_WINDOW_UTILITY;

    const window = c.SDL_CreateWindow("matrisen", @intCast(width), @intCast(height), flags) orelse {
        @panic("Failed to create SDL window");
    };

    checkSdl(c.SDL_ShowWindow(window));

    return .{
        .handle = window,
        .width = width,
        .height = height,
    };
}

pub fn deinit(self: *Self) void {
    c.SDL_DestroyWindow(self.handle);
    c.SDL_Quit();
}

pub const State = packed struct {
    mouse_x: f32 = 0,
    mouse_y: f32 = 0,
    tab: bool = false,
    quit: bool = false,
    capturemouse: bool = false,
    resizerequest: bool = false,
};

pub fn processInput(self: *Self) void {
    var event: c.SDL_Event = undefined;
    self.state.mouse_x = 0;
    self.state.mouse_y = 0;
    while (c.SDL_PollEvent(&event) == true) { // Or > 0 depending on SDL version
        switch (event.type) {
            c.SDL_EVENT_QUIT => self.state.quit = true,
            c.SDL_EVENT_KEY_DOWN => {
                if (event.key.scancode == c.SDL_SCANCODE_TAB) {
                    toggleMouseCapture(self);
                }
            },
            c.SDL_EVENT_MOUSE_MOTION => {
                self.state.mouse_x = event.motion.xrel;
                self.state.mouse_y = event.motion.yrel;
            },
            c.SDL_EVENT_WINDOW_RESIZED => self.state.resizerequest = true,
            else => {},
        }
    }
}

pub fn toggleMouseCapture(self: *Self) void {
    self.state.capturemouse = !self.state.capturemouse;
    checkSdl(c.SDL_SetWindowRelativeMouseMode(self.handle, self.state.capturemouse));
}

pub fn checkSdl(res: bool) void {
    if (!res) {
        std.log.err("Detected SDL error: {s}", .{c.SDL_GetError()});
        @panic("SDL error");
    }
}
