const std = @import("std");
const Io = std.Io;

const audio = @import("audio");
const c = audio.jack;

// DSP State
var phase: f32 = 0.0;
var sample_rate: f32 = 44100.0; // Mutated dynamically by sampleRateCallback
const freq: f32 = 440.0; // A4 pitch
const amplitude: f32 = 0.1; // -20 dBFS attenuation

// JACK Graph State
var client: ?*c.jack_client_t = null;
var port: ?*c.jack_port_t = null;

export fn process(nframes: c.jack_nframes_t, arg: ?*anyopaque) callconv(.c) c_int {
    _ = arg;

    // Obtain the pointer to the zero-copy buffer allocated by the JACK server
    const raw_buf = c.jack_port_get_buffer(port, nframes) orelse return 0;
    const out: [*]f32 = @ptrCast(@alignCast(raw_buf));

    const phase_inc = freq / sample_rate;

    for (0..nframes) |i| {
        out[i] = @sin(phase * std.math.pi * 2.0) * amplitude;
        phase += phase_inc;

        // Wrap phase strictly within [0.0, 1.0)
        if (phase >= 1.0) {
            phase -= 1.0;
        }
    }

    return 0; // Return 0 to indicate successful execution
}

export fn sampleRateCallback(nframes: c.jack_nframes_t, arg: ?*anyopaque) callconv(.c) c_int {
    _ = arg;
    sample_rate = @floatFromInt(nframes);
    return 0;
}

pub fn main(init: std.process.Init) !void {
    const io = init.io;
    const arena = init.arena.allocator();
    _ = arena;

    var status: c.jack_status_t = undefined;

    // 1. Open JACK client
    client = c.jack_client_open("zig_oscillator", c.JackNullOption, &status);
    if (client == null) return error.JackOpenFailed;
    defer _ = c.jack_client_close(client);

    // 2. Register structural callbacks
    _ = c.jack_set_process_callback(client, process, null);
    _ = c.jack_set_sample_rate_callback(client, sampleRateCallback, null);

    // 3. Register audio output port
    port = c.jack_port_register(
        client,
        "out",
        c.JACK_DEFAULT_AUDIO_TYPE,
        c.JackPortIsOutput,
        0,
    );
    if (port == null) return error.JackPortRegisterFailed;

    // 4. Activate client on the server graph
    if (c.jack_activate(client) != 0) return error.JackActivateFailed;

    std.debug.print("Audio graph active. Patch 'zig_oscillator:out' to playback device.\n", .{});

    // Block the non-real-time main thread indefinitely
    while (true) {
        try io.sleep(Io.Duration.fromSeconds(5), .awake);
    }
}
