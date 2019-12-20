#version 450

layout(set=0, binding=0) uniform u_UniformBuffer {
    float uTime;
    float mouseX;
    float mouseY;
    float mDown;
	vec4 color;
    vec4 dim;
} ubo;

layout(location = 0) in vec3 i_FragColor;
layout(location = 1) in vec2 i_TexCoord;

layout(location = 0) out vec4 o_Color;

float iTime = ubo.uTime;
vec2 iResolution = vec2( ubo.dim[2], ubo.dim[3]);
vec2 iMouse = vec2(ubo.mouseX, ubo.mouseY);
vec2 fragCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);


void main() {
	o_Color = vec4(1,1,1,1);
}
