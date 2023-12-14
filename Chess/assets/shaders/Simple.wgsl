struct CameraBuffer {
    view: mat4x4f,
    projection: mat4x4f,
    viewProjection: mat4x4f
}

@group(0) @binding(0) var<uniform> u_Camera: CameraBuffer;

@vertex
fn vs_main(@location(0) a_Pos: vec3f) -> @builtin(position) vec4f {
	return u_Camera.viewProjection * vec4f(a_Pos, 1.0);
}

@fragment
fn fs_main() -> @location(0) vec4f {
	return vec4f(0, 1, 0, 1);
}
