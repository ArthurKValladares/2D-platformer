#version 450

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout( push_constant ) uniform constants
{
	vec3 color;
    float pad;
    vec3 outline;
    float thickness;
} PushConstants;

void main() {
    if (fragTexCoord.x < PushConstants.thickness || (1.0 - fragTexCoord.x) < PushConstants.thickness ||
        fragTexCoord.y < PushConstants.thickness || (1.0 - fragTexCoord.y) < PushConstants.thickness) {
        outColor = vec4(PushConstants.outline, 1.0);
    } else {
        outColor = texture(texSampler, fragTexCoord) * vec4(PushConstants.color, 1.0);
    }
}