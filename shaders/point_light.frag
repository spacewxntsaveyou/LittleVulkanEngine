#version 450

layout (location = 0) in vec2 fragOffsets;
layout (location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUbo {	//set and binding numbers MUST match DescriptorSetLayout
//must match UBO struct
mat4 projection;
mat4 view;
vec4 ambientLightColor;	
vec3 lightPosition;
vec4 lightColor;
} ubo;

void main() {

float dis = sqrt(dot(fragOffsets, fragOffsets));
if (dis >= 1.0) { discard; }	//Throws away fragment then returns

outColor = vec4(ubo.lightColor.xyz, 1.0);

}