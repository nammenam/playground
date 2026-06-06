const std = @import("std");
const math = @import("math.zig");
const Vec3 = math.Vec3;
const Mat4 = math.Mat4;
const Quaternion = math.Quaternion;

pub const Camera = struct {
    position: Vec3,
    rotation: Quaternion,
    fov: f32,
    aspect: f32,
    near: f32,
    far: f32,

    pub fn getViewMatrix(self: *const Camera) Mat4 {
        // Simple look-at using position and rotation
        const forward = self.rotation.rotateVec3(Vec3.new(0, 0, -1));
        const target = self.position.add(forward);
        const up = self.rotation.rotateVec3(Vec3.new(0, 1, 0));
        return Mat4.lookAt(self.position, target, up);
    }

    pub fn getProjectionMatrix(self: *const Camera) Mat4 {
        return Mat4.perspective(self.fov, self.aspect, self.near, self.far);
    }
};

var global_camera: Camera = .{
    .position = Vec3.new(0, 2, 10),
    .rotation = Quaternion.identity(),
    .fov = std.math.pi / 4.0,
    .aspect = 1.0,
    .near = 0.1,
    .far = 100.0,
};

var mouse_dx: f32 = 0;
var mouse_dy: f32 = 0;
var keys_pressed: [256]bool = undefined;

export fn initCamera(width: f32, height: f32) void {
    global_camera.aspect = width / height;
    for (&keys_pressed) |*k| {
        k.* = false;
    }
}

export fn onMouseMove(dx: f32, dy: f32) void {
    mouse_dx = dx;
    mouse_dy = dy;
}

export fn onKeyDown(key: u32) void {
    if (key < 256) keys_pressed[key] = true;
}

export fn onKeyUp(key: u32) void {
    if (key < 256) keys_pressed[key] = false;
}

export fn updateCamera(dt: f32) void {
    const sensitivity: f32 = 0.005;
    
    // Orbital-like rotation
    const yaw = Quaternion.fromAxisAngle(Vec3.new(0, 1, 0), -mouse_dx * sensitivity);
    const pitch = Quaternion.fromAxisAngle(Vec3.new(1, 0, 0), -mouse_dy * sensitivity);
    
    global_camera.rotation = yaw.mul(global_camera.rotation).mul(pitch);
    
    mouse_dx = 0;
    mouse_dy = 0;

    const speed: f32 = 10.0 * dt;
    var move = Vec3.new(0, 0, 0);
    if (keys_pressed['W']) move.z -= 1;
    if (keys_pressed['S']) move.z += 1;
    if (keys_pressed['A']) move.x -= 1;
    if (keys_pressed['D']) move.x += 1;
    
    if (move.x != 0 or move.y != 0 or move.z != 0) {
        const rotated_move = global_camera.rotation.rotateVec3(move.normalize().scale(speed));
        global_camera.position = global_camera.position.add(rotated_move);
    }
}

var view_buffer: [16]f32 = undefined;
export fn getCameraViewMatrix() *const [16]f32 {
    const m = global_camera.getViewMatrix();
    view_buffer = m.data;
    return &view_buffer;
}

var proj_buffer: [16]f32 = undefined;
export fn getCameraProjectionMatrix() *const [16]f32 {
    const m = global_camera.getProjectionMatrix();
    proj_buffer = m.data;
    return &proj_buffer;
}

// Instance management
pub const Instance = struct {
    position: Vec3,
    rotation: Quaternion,
    scale: Vec3,

    pub fn toMat4(self: *const Instance) Mat4 {
        const translation = Mat4.translate(self.position);
        const rotation = self.rotation.toMat4();
        const sc = Mat4.scale(self.scale);
        
        return translation.mul(rotation).mul(sc);
    }
};

var sphere_instances: [3]Instance = undefined;

export fn initWorld() void {
    const space_between: f32 = 3.0;
    for (&sphere_instances, 0..) |*inst, i| {
        const x = space_between * (@as(f32, @floatFromInt(i)) - 1.0);
        inst.position = Vec3.new(x, 0, 1);
        inst.rotation = Quaternion.identity();
        inst.scale = Vec3.new(1, 1, 1);
    }
}

export fn getSphereInstancesCount() u32 {
    return 3;
}

var inst_buffer: [16]f32 = undefined;
export fn getSphereInstanceData(index: u32) *const [16]f32 {
    if (index >= 3) return &inst_buffer;
    const m = sphere_instances[index].toMat4();
    inst_buffer = m.data;
    return &inst_buffer;
}
