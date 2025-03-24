#version 450

layout(set = 0, binding = 0) uniform SceneData{
    mat4 proj;
} globalData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec2 fragTexCoord;

void main() {
    gl_Position = globalData.proj * vec4(inPosition, 1.0);

    fragTexCoord = inColor.xy;
}