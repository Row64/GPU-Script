#version 450

layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec3 i_Color;
layout(location = 2) in vec2 i_Texcoord;

layout(location = 0) out vec3 o_FragColor;
layout(location = 1) out vec2 o_TexCoord;

void main() {

    if(gl_VertexIndex == 0){
        o_TexCoord = vec2(0,0);
    }
    else if(gl_VertexIndex == 1){
        o_TexCoord = vec2(0,1);
    }
    else if(gl_VertexIndex == 2){
        o_TexCoord = vec2(1,0);

    }
    else if(gl_VertexIndex == 3){
        o_TexCoord = vec2(1,1);
    }

    gl_Position = vec4(o_TexCoord * 2.0f - 1.0f, 0.0f, 1.0f);
}
