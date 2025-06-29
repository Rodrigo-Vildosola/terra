struct Uniforms {
    u_view_proj: mat4x4<f32>,
    u_model: mat4x4<f32>,
    u_time: f32,
    u_aspect_ratio: f32,
    u_color: vec4<f32>,
};

@group(0) @binding(0)
var<uniform> ubo: Uniforms;

struct VertexInput {
    @location(0) position: vec3f,
    @location(1) color:    vec3f,
};

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) color: vec3f,
};

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
	let angle = ubo.u_time; // you can multiply it go rotate faster
	let alpha = cos(angle);
	let beta = sin(angle);
	var position = vec3f(
		in.position.x,
		alpha * in.position.y + beta * in.position.z,
		alpha * in.position.z - beta * in.position.y,
	);
	out.position = vec4f(position.x, position.y * ubo.u_aspect_ratio, position.z * 0.5 + 0.5, 1.0);

    out.color = in.color;
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    return vec4f(in.color, 1.0) * ubo.u_color;
}
