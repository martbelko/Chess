struct ObjectBuffer {
	model: mat4x4f,
	modelInv: mat4x4f
}

struct CameraBuffer {
	view: mat4x4f,
	projection: mat4x4f,
	viewProjection: mat4x4f,
	position: vec3f
}

struct MaterialBuffer {
	ambient: vec4f,
	diffuse: vec4f,
	specular: vec4f
}

@group(0) @binding(0) var<uniform> u_Objects: array<ObjectBuffer, 10>;
@group(1) @binding(0) var<uniform> u_Camera: CameraBuffer;
@group(2) @binding(0) var<uniform> u_Materials: array<MaterialBuffer, 10>;

struct VertexInput {
	@location(0) a_Pos: vec3f,
	@location(1) a_Normal: vec3f,
	@location(2) a_TexCoord: vec2f,

	@builtin(instance_index) iId: u32
}

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) fragPos: vec3f,
	@location(1) normal: vec3f,
	@location(2) @interpolate(flat) iId: u32
}

@vertex
fn vs_main(input: VertexInput) -> VertexOutput {
	var modelMatrix = u_Objects[input.iId].model;
	var modelMatrixInverse = u_Objects[input.iId].modelInv;

	var normalMatrix = mat3x3(modelMatrixInverse[0].xyz, modelMatrixInverse[1].xyz, modelMatrixInverse[2].xyz);

	var output: VertexOutput;
	output.position = u_Camera.viewProjection * modelMatrix * vec4f(input.a_Pos, 1.0);
	output.fragPos = input.a_Pos;
	output.normal = normalMatrix * input.a_Normal;
	output.iId = input.iId;

	return output;
}

@fragment
fn fs_main(input: VertexOutput) -> @location(0) vec4f {
	var material = u_Materials[input.iId];

	var lightPos = vec3f(10, 10, 10);

	// ambient
	var ambient = 0.15 * material.ambient;
	// diffuse
	var lightDir = normalize(lightPos - input.fragPos);
	var normal = normalize(input.normal);
	var diff = max(dot(lightDir, normal), 0.0);
	var diffuse = diff * material.diffuse;
	// specular
	var viewDir = normalize(u_Camera.position - input.fragPos);
	var reflectDir = reflect(-lightDir, normal);
	var spec = 0.0;
	var halfwayDir = normalize(lightDir + viewDir);  
	spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	var specular = material.specular * spec;

	var color = ambient + diffuse + specular;
	color.a = 1.0;
	return color;
}
