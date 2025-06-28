#version 450

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout( push_constant ) uniform constants
{
	vec3 color;
} PushConstants;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(texSampler, fragTexCoord).r);
	outColor = sampled * vec4(PushConstants.color, 1.0);
}