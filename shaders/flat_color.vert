#version 450

layout(set = 0, binding = 0) uniform GlobalData{
    mat4 proj;
} globalData;

layout(location = 0) in vec3 inPosition;

void main() {
    gl_Position = globalData.proj * vec4(inPosition, 1.0);
}