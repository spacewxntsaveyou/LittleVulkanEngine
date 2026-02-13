#version 450

layout (location = 0) in vec2 fragOffsets;
layout (location = 0) out vec4 outColor;

struct PointLight {
vec4 position;	//ignore w
vec4 color;	//w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {	//set and binding numbers MUST match DescriptorSetLayout
//must match UBO struct
mat4 projection;
mat4 view;
vec4 ambientLightColor;	
PointLight pointLights[10];	//See "#define MAX_LIGHTS[]
int numLights;
} ubo;

layout(push_constant) uniform Push {

vec4 position;
vec4 color;
float radius;
} push;


void main() {

float dis = sqrt(dot(fragOffsets, fragOffsets));
if (dis >= 1.0) { discard; }	//Throws away fragment then returns

outColor = vec4(push.color.xyz, 1.0);

}