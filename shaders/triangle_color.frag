#version 450

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout( push_constant ) uniform constants
{
	vec3 color;
} PushConstants;

void main() {
    outColor = texture(texSampler, fragTexCoord) * vec4(PushConstants.color, 1.0);
}