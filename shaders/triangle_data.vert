#version 450

layout(set = 0, binding = 0) uniform SceneData{   
    mat4 transform;
	vec4 color;
} sceneData;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out vec4 outColor;

void main() {
    gl_Position = sceneData.transform * vec4(inPosition, 1.0);

    outTexCoord = inColor.xy;
    outColor = sceneData.color;
}