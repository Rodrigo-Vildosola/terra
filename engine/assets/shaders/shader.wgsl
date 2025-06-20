struct VertexInput {
    @location(0) position: vec2f,
    @location(1) color:    vec3f,
};

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0)        color:    vec3f,
};

@group(0) @binding(0)
var<uniform> u_time: f32;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    // lift 2d → 4d position:
    out.position = vec4f(in.position, 0.0, 1.0);
    // just forward the color
    out.color    = in.color;
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    // Apply time-based modulation to color
    let pulse = 0.5 + 0.5 * sin(u_time); // from 0.0 to 1.0
    let animated_color = in.color * pulse;

    return vec4f(animated_color, 1.0);
}
