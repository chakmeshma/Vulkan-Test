#version 450 core

layout(points) in;
layout(line_strip, max_vertices = 2) out;

layout (set = 0, binding = 1) uniform sampler2D textureSampler;

layout (set = 0, binding = 2) uniform sampler2D normalSampler;

layout (set = 0, binding = 3) uniform sampler2D specSampler;

layout (location = 0) in vec3 iFragPos[];

layout (location = 1) in vec2 iFragUV[];

layout (location = 2) in vec3 iFragNor[];

layout (location = 3) in vec3 iFragTan[];

layout (location = 4) in vec3 iFragBitan[];

const float normalLength = 13.13;

in gl_PerVertex {
	vec4 gl_Position;
} gl_in[];

layout (push_constant) uniform ViewProjection {
	mat4 view;
	mat4 projection;
} viewProjection;

void main() {
	mat3 TBN = mat3(
        iFragTan[0],
        iFragBitan[0],
        iFragNor[0]
    );
	
	vec3 normapFragNor = (texture(normalSampler, iFragUV[0]).rgb * 2.0 - 1.0);

	
	normapFragNor = normalize(TBN * normapFragNor) * normalLength;
	
	//vec3 lightDirectionViewSpace = (lightPos - fragPos);
	

	gl_Position = gl_in[0].gl_Position;
	
	EmitVertex();
	
	gl_Position = viewProjection.projection * vec4(iFragPos[0] + normapFragNor, 1.0);
	
	EmitVertex();
}