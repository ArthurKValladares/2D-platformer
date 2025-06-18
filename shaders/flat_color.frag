#version 450

layout( push_constant ) uniform constants
{
	vec4 color;
} PushConstants;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = PushConstants.color;
}