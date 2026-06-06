const std = @import("std");

// 1. Define the Enum
// This represents the possible states a player character can be in.
const PlayerState = enum {
    Idle,
    Walking,
    Running,
    Jumping,
};

pub fn main() !void {
    // 2. Create the Enum-Indexed Array
    // We use a special anonymous struct literal syntax to initialize the array.
    // The `.{ .Variant = "value" }` syntax directly maps each enum variant
    // to its corresponding value in the array.
    // The `[_]` tells the compiler to infer the array's length.
    const state_descriptions: std.EnumArray(PlayerState, )
    // 3. Using the Enum as an Index
    // Let's set a current state for our player.
    var current_state: PlayerState = .Walking;

    // To use the enum as an index, we convert it to its integer value
    // using the `@intFromEnum()` builtin.
    const description = state_descriptions[@intFromEnum(current_state)];
    std.debug.print("Current State Info: {s}\n", .{description});

    // Let's change the state and see the result
    current_state = .Jumping;
    std.debug.print("Current State Info: {s}\n\n", .{state_descriptions[@intFromEnum(current_state)]});

    // 4. Looping Through All Enum Variants at Compile Time
    // This is a very powerful pattern. `inline for` with `std.meta.tags`
    // iterates over every variant of the enum at compile time.
    std.debug.print("--- All Possible Player States ---\n", .{});
    inline for (std.meta.tags(PlayerState)) |state| {
        // `state` here is an actual enum variant like `PlayerState.Idle`
        const message = state_descriptions[@intFromEnum(state)];
        
        // `@tagName` gives you the string name of the enum variant.
        std.debug.print("{s}: {s}\n", .{@tagName(state), message});
    }
}
