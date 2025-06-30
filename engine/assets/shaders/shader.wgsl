// ---------------------
// Per-frame Uniforms
// ---------------------
struct Uniforms {
    u_view: mat4x4<f32>,
    u_proj: mat4x4<f32>,
    u_time: f32,
    u_aspect_ratio: f32,
};
@group(0) @binding(0)
var<uniform> ubo: Uniforms;

// ---------------------
// Per-instance Data (Storage Buffer)
// ---------------------
struct Instance {
    model: mat4x4<f32>,
    color: vec4<f32>,
};
@group(1) @binding(0)
var<storage, read> instances: array<Instance>;

// ---------------------
// Vertex Input
// ---------------------
struct VertexInput {
    @location(0) position: vec3f,
    @location(1) color: vec3f,
    @builtin(instance_index) instance_idx: u32,
};

// ---------------------
// Vertex Output
// ---------------------
struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) v_color: vec3f,
    @location(1) v_instance_color: vec4f,
};

// ---------------------
// Vertex Shader
// ---------------------
@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;

    let instance = instances[in.instance_idx];

    // Animate using time (e.g., rotating model)
    let angle = ubo.u_time;
    let alpha = cos(angle);
    let beta = sin(angle);
    let rotated = vec3f(
        in.position.x,
        alpha * in.position.y + beta * in.position.z,
        alpha * in.position.z - beta * in.position.y,
    );

    let pos = vec4f(rotated.x, rotated.y * ubo.u_aspect_ratio, rotated.z * 0.5 + 0.5, 1.0);

    let world_pos = instance.model * pos;

    out.position = world_pos;

    // Pass through colors
    out.v_color = in.color;
    out.v_instance_color = instance.color;

    return out;
}

// ---------------------
// Fragment Shader
// ---------------------
@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    return vec4f(in.v_color, 1.0) * in.v_instance_color;
}
