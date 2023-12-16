struct CameraBuffer {
	view: mat4x4f,
	projection: mat4x4f,
	viewProjection: mat4x4f,
	position: vec3f
}

@group(0) @binding(0) var<uniform> u_Camera: CameraBuffer;

struct VertexInput {
	@location(0) a_Pos: vec3f,
	@location(1) a_Normal: vec3f,
	@location(1) a_TexCoord: vec2f
}

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) fragPos: vec3f,
	@location(1) normal: vec3f
}

@vertex
fn vs_main(input: VertexInput) -> VertexOutput {
	var output: VertexOutput;
	output.position = u_Camera.viewProjection * vec4f(input.a_Pos, 1.0);
	output.fragPos = input.a_Pos;
	output.normal = input.a_Normal;
	return output;
}

@fragment
fn fs_main(input: VertexOutput) -> @location(0) vec4f {
	var lightPos = vec3f(10, 10, 10);

	var color = vec3f(0.2, 0.8, 0.1);
	// ambient
	var ambient = 0.5 * color;
	// diffuse
	var lightDir = normalize(lightPos - input.fragPos);
	var normal = normalize(input.normal);
	var diff = max(dot(lightDir, normal), 0.0);
	var diffuse = diff * color;
	// specular
	var viewDir = normalize(u_Camera.position - input.fragPos);
	var reflectDir = reflect(-lightDir, normal);
	var spec = 0.0;
	var halfwayDir = normalize(lightDir + viewDir);  
	spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	var specular = vec3f(0.3) * spec; // assuming bright white light color

	return vec4f(ambient + diffuse + specular, 1.0);
}
