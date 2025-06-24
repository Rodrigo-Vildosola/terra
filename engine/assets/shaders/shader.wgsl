struct CameraData {
    view_proj: mat4x4<f32>,
};

@group(0) @binding(0)
var<uniform> u_camera: CameraData;

@group(0) @binding(1)
var<uniform> u_time: f32;

struct VertexInput {
    @location(0) position: vec2f,
    @location(1) color:    vec3f,
};

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0)        color:    vec3f,
};

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    let world_pos = vec4f(in.position, 0.0, 1.0);
    out.position = u_camera.view_proj * world_pos;
    out.color    = in.color;
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let pulse = 0.5 + 0.5 * sin(u_time);
    let animated_color = in.color * pulse;
    return vec4f(animated_color, 1.0);
}
