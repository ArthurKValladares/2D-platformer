#version 450

layout(set = 0, binding = 0) uniform GlobalData{
    mat4 proj;
} globalData;

layout(set = 1, binding = 1) uniform SceneData{
    mat4 transform;
	vec4 color;
} sceneData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inTexCoord;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out vec4 outColor;

void main() {
    gl_Position = globalData.proj * sceneData.transform * vec4(inPosition, 1.0);

    outTexCoord = inTexCoord.xy;
    outColor = sceneData.color;
}