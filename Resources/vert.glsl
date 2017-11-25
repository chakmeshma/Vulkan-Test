#version 450 core

layout (set = 0, binding = 0) uniform ModelMatrix {
	mat4 model;
} modelMatrix;

layout (push_constant) uniform ViewProjection {
	mat4 view;
	mat4 projection;
} viewProjection;

layout (location = 0) in vec3 inPos;

layout (location = 1) in vec3 inNor;

layout (location = 2) in vec2 inUV;

layout (location = 3) in vec3 inTan;

layout (location = 4) in vec3 inBitan;

layout (location = 0) out vec3 fragPos;

layout (location = 1) out vec2 fragUV;

layout (location = 2) out vec3 fragNor;

layout (location = 3) out vec3 fragTan;

layout (location = 4) out vec3 fragBitan;


void main()
{
	mat3 vp3 = mat3(viewProjection.view * modelMatrix.model);
	mat3 rotation;
	rotation[0] = vp3[0] / length(vp3[0]);
	rotation[1] = vp3[1] / length(vp3[1]);
	rotation[2] = vp3[2] / length(vp3[2]);

	fragPos		= vec3(viewProjection.view * modelMatrix.model * vec4(inPos, 1.0));
	fragNor		= rotation * inNor;
	fragUV		= inUV;
    fragTan		= rotation * inTan;
    fragBitan	= rotation * inBitan;
	

	gl_Position = viewProjection.projection * vec4(fragPos, 1.0);
}