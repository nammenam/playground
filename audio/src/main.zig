const std = @import("std");
const Io = std.Io;

const audio = @import("audio");
const alsa = audio.alsa;

pub fn main(init: std.process.Init) !void {
    const arena: std.mem.Allocator = init.arena.allocator();
    const args = try init.minimal.args.toSlice(arena);
    for (args) |arg| {
        std.log.info("arg: {s}", .{arg});
    }

    const io = init.io;
    var stdout_buffer: [1024]u8 = undefined;
    var stdout_file_writer: Io.File.Writer = .init(.stdout(), io, &stdout_buffer);
    const stdout_writer = &stdout_file_writer.interface;
    try stdout_writer.flush(); // Don't forget to flush!

    const device_name = "default"; // Intercepted by the pipewire-alsa plugin
    var pcm_handle: *alsa.snd_pcm_t = undefined;

    // 1. Initialize the PCM device for playback
    var err = alsa.snd_pcm_open(&pcm_handle, device_name, alsa.SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        std.debug.print("Failed to open PCM device: {s}\n", .{alsa.snd_strerror(err)});
        return error.AlsaInitFailed;
    }
    defer _ = alsa.snd_pcm_close(pcm_handle);

    // 2. Allocate memory for hardware parameter configuration
    var hw_params: *alsa.snd_pcm_hw_params_t = undefined;
    _ = alsa.snd_pcm_hw_params_malloc(&hw_params);
    defer alsa.snd_pcm_hw_params_free(hw_params);

    // 3. Populate the parameter struct with the full configuration space
    _ = alsa.snd_pcm_hw_params_any(pcm_handle, hw_params);

    // 4. Constrain parameters: Interleaved access, 16-bit little-endian, 2 channels
    _ = alsa.snd_pcm_hw_params_set_access(pcm_handle, hw_params, alsa.SND_PCM_ACCESS_RW_INTERLEAVED);
    _ = alsa.snd_pcm_hw_params_set_format(pcm_handle, hw_params, alsa.SND_PCM_FORMAT_S16_LE);
    _ = alsa.snd_pcm_hw_params_set_channels(pcm_handle, hw_params, 2);

    // Request an exact sample rate. The hardware/PipeWire may negotiate this to the nearest supported rate.
    var exact_rate: c_uint = 44100;
    _ = alsa.snd_pcm_hw_params_set_rate_near(pcm_handle, hw_params, &exact_rate, null);

    // Commit hardware parameters to the device
    err = alsa.snd_pcm_hw_params(pcm_handle, hw_params);
    if (err < 0) {
        std.debug.print("Failed to set hardware parameters: {s}\n", .{alsa.snd_strerror(err)});
        return error.AlsaHwParamsFailed;
    }

    // 5. Synthesize Audio Data (440Hz Sine Wave)
    const sample_rate: f32 = 44100.0;
    const duration_seconds: usize = 2;
    const total_frames = @as(usize, @intFromFloat(sample_rate)) * duration_seconds;

    // 16-bit integer requires i16. Two channels per frame.
    const buffer = try arena.alloc(i16, total_frames * 2);

    const freq: f32 = 440.0;
    const angular_freq = 2.0 * std.math.pi * freq / sample_rate;

    for (0..total_frames) |i| {
        const t = @as(f32, @floatFromInt(i));
        // Calculate sine wave amplitude and scale linearly to the 16-bit signed integer range
        const amplitude = @as(i16, @intFromFloat(std.math.sin(t * angular_freq) * 16000.0));

        // Interleaved layout: [Left, Right, Left, Right...]
        buffer[i * 2] = amplitude; // Left channel vector
        buffer[(i * 2) + 1] = amplitude; // Right channel vector
    }

    // 6. Push frames to the ALSA ring buffer
    const frames_written = alsa.snd_pcm_writei(pcm_handle, buffer.ptr, total_frames);
    if (frames_written < 0) {
        // A negative return indicates an underrun (EPIPE) or state invalidation
        _ = alsa.snd_pcm_recover(pcm_handle, @as(c_int, @intCast(frames_written)), 0);
    }

    // Await buffer drainage before terminating the process
    _ = alsa.snd_pcm_drain(pcm_handle);
}
