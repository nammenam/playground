const std = @import("std");

pub const Vec3 = struct {
    x: f32,
    y: f32,
    z: f32,

    pub fn new(x: f32, y: f32, z: f32) Vec3 {
        return .{ .x = x, .y = y, .z = z };
    }

    pub fn add(a: Vec3, b: Vec3) Vec3 {
        return .{ .x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z };
    }

    pub fn scale(v: Vec3, s: f32) Vec3 {
        return .{ .x = v.x * s, .y = v.y * s, .z = v.z * s };
    }

    pub fn normalize(v: Vec3) Vec3 {
        const len = @sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        if (len == 0) return v;
        return v.scale(1.0 / len);
    }
};

pub const Mat4 = struct {
    data: [16]f32,

    pub fn identity() Mat4 {
        return .{
            .data = .{
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1,
            },
        };
    }

    pub fn perspective(fovy_rad: f32, aspect: f32, near: f32, far: f32) Mat4 {
        const f = 1.0 / std.math.tan(fovy_rad / 2.0);
        const range_inv = 1.0 / (near - far);

        var m = Mat4.identity();
        m.data[0] = f / aspect;
        m.data[5] = f;
        m.data[10] = far * range_inv;
        m.data[11] = -1.0;
        m.data[14] = (near * far) * range_inv;
        m.data[15] = 0.0;
        return m;
    }

    pub fn lookAt(eye: Vec3, center: Vec3, up: Vec3) Mat4 {
        const f = center.add(eye.scale(-1.0)).normalize();
        const s = Vec3.new(
            f.y * up.z - f.z * up.y,
            f.z * up.x - f.x * up.z,
            f.x * up.y - f.y * up.x,
        ).normalize();
        const u = Vec3.new(
            s.y * f.z - s.z * f.y,
            s.z * f.x - s.x * f.z,
            s.x * f.y - s.y * f.x,
        );

        var m = Mat4.identity();
        m.data[0] = s.x;
        m.data[4] = s.y;
        m.data[8] = s.z;
        m.data[1] = u.x;
        m.data[5] = u.y;
        m.data[9] = u.z;
        m.data[2] = -f.x;
        m.data[6] = -f.y;
        m.data[10] = -f.z;
        m.data[12] = -(s.x * eye.x + s.y * eye.y + s.z * eye.z);
        m.data[13] = -(u.x * eye.x + u.y * eye.y + u.z * eye.z);
        m.data[14] = (f.x * eye.x + f.y * eye.y + f.z * eye.z);
        return m;
    }

    pub fn translate(v: Vec3) Mat4 {
        var m = Mat4.identity();
        m.data[12] = v.x;
        m.data[13] = v.y;
        m.data[14] = v.z;
        return m;
    }

    pub fn scale(v: Vec3) Mat4 {
        var m = Mat4.identity();
        m.data[0] = v.x;
        m.data[5] = v.y;
        m.data[10] = v.z;
        return m;
    }

    pub fn mul(a: Mat4, b: Mat4) Mat4 {
        var res = Mat4.identity();
        for (0..4) |j| {
            for (0..4) |i| {
                var sum: f32 = 0;
                for (0..4) |k| {
                    sum += a.data[i + k * 4] * b.data[k + j * 4];
                }
                res.data[i + j * 4] = sum;
            }
        }
        return res;
    }
};

pub const Quaternion = struct {
    x: f32,
    y: f32,
    z: f32,
    w: f32,

    pub fn identity() Quaternion {
        return .{ .x = 0, .y = 0, .z = 0, .w = 1 };
    }

    pub fn fromAxisAngle(axis: Vec3, angle_rad: f32) Quaternion {
        const s = std.math.sin(angle_rad / 2.0);
        const c = std.math.cos(angle_rad / 2.0);
        const n = axis.normalize();
        return .{
            .x = n.x * s,
            .y = n.y * s,
            .z = n.z * s,
            .w = c,
        };
    }

    pub fn toMat4(q: Quaternion) Mat4 {
        var m = Mat4.identity();
        const xx = q.x * q.x;
        const yy = q.y * q.y;
        const zz = q.z * q.z;
        const xy = q.x * q.y;
        const xz = q.x * q.z;
        const yz = q.y * q.z;
        const wx = q.w * q.x;
        const wy = q.w * q.y;
        const wz = q.w * q.z;

        m.data[0] = 1.0 - 2.0 * (yy + zz);
        m.data[1] = 2.0 * (xy + wz);
        m.data[2] = 2.0 * (xz - wy);

        m.data[4] = 2.0 * (xy - wz);
        m.data[5] = 1.0 - 2.0 * (xx + zz);
        m.data[6] = 2.0 * (yz + wx);

        m.data[8] = 2.0 * (xz + wy);
        m.data[9] = 2.0 * (yz - wx);
        m.data[10] = 1.0 - 2.0 * (xx + yy);

        return m;
    }

    pub fn invert(q: Quaternion) Quaternion {
        return .{ .x = -q.x, .y = -q.y, .z = -q.z, .w = q.w };
    }

    pub fn mul(a: Quaternion, b: Quaternion) Quaternion {
        return .{
            .x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
            .y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
            .z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
            .w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
        };
    }

    pub fn rotateVec3(q: Quaternion, v: Vec3) Vec3 {
        const qv = Quaternion{ .x = v.x, .y = v.y, .z = v.z, .w = 0 };
        const res = q.mul(qv).mul(q.invert());
        return Vec3.new(res.x, res.y, res.z);
    }
};
