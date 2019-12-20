#version 450

// === START STANDARD INPUTS =================================================
layout(set=0, binding=0) uniform u_UniformBuffer {
    float uTime;
    float mouseX;
    float mouseY;
    float mDown;
	vec4 color;
    vec4 dim;
} ubo;
layout(set=0, binding=1) uniform sampler2D u_Texture;
layout(location = 0) in vec3 i_FragColor;
layout(location = 1) in vec2 i_TexCoord;
layout(location = 0) out vec4 o_Color;

// standard variables put in Layout Pane space - ubo.dim [x,y,width,height]
float iTime = ubo.uTime;
vec2 iResolution = vec2( ubo.dim[2], ubo.dim[3]);  // size of pane
vec2 iMouse = vec2(ubo.mouseX - ubo.dim[0], ubo.mouseY - ubo.dim[1]);  // mouse cord in pane (not window) 
vec2 fragCoord = vec2(gl_FragCoord.x - ubo.dim[0], gl_FragCoord.y - ubo.dim[1]);  // current pixel - 0,0 top left of pane
// === END STANDARD INPUTS ===================================================


// --- SET CONSTANTS ---------------------------------------------------------
vec2 tSize = vec2(844, 210);  // it's important to set texture size because it's evaluated by percentage


// --- START HELPERS ---------------------------------------------------------
struct button{  // sprite button
    vec4 dim; // pixel space dimension minX, minY, maxX, maxY
    vec2 t; // texture space coordinate for default (no over/down) draw
    vec2 tOver; // texture space coordinate for over
    vec2 tDown; // texture space coordinate for down
};
struct decal{  // sprite decal - no interaction
    vec4 dim; // pixel space dimension minX, minY, maxX, maxY
    vec2 t; // texture space coordinate for default (no over/down) draw
};
bool InsideBB(vec4 inDim){
     if(fragCoord.x > inDim[0] && fragCoord.x < inDim[2] && fragCoord.y > inDim[1] && fragCoord.y < inDim[3]){
         return true;
     }
     return false;
}
bool OverBB(vec4 inDim){
      if(iMouse.x > inDim[0] && iMouse.x < inDim[2] && iMouse.y > inDim[1] && iMouse.y < inDim[3]){
         return true;
     }
     return false;
}
vec4 GetButtonPixel(button inS){
    vec2 tCord = vec2(fragCoord.x - inS.dim[0], fragCoord.y - inS.dim[1]);
    if(iMouse.x > inS.dim[0] && iMouse.x < inS.dim[2] && iMouse.y > inS.dim[1] && iMouse.y < inS.dim[3]){
        if(ubo.mDown == 1){ tCord += inS.tDown;}
        else{tCord += inS.tOver;}
    }
    else{tCord += inS.t;}
    vec2 textureCord = vec2(tCord.x / tSize.x, tCord.y/ tSize.y); // texture lookup is in percentage
    return texture(u_Texture, textureCord) ;
}
vec4 GetDecalPixel(decal inD){
    vec2 tCord = vec2(fragCoord.x - inD.dim[0], fragCoord.y - inD.dim[1]);
    tCord += inD.t;
    vec2 textureCord = vec2(tCord.x / tSize.x, tCord.y/ tSize.y);
    return texture(u_Texture, textureCord) ;

}
// --- END HELPERS ---------------------------------------------------------

// define UI elements
button B1 = button(vec4(28,8,214,64),vec2(28,8),vec2(28,78),vec2(28,149));
button B2 = button(vec4(237,8,421,64),vec2(237,8),vec2(237,78),vec2(237,149));
decal D1 = decal(vec4(429,8,771,64),vec2(429,8));

void main() {
    
    if(InsideBB(B1.dim)){
        o_Color = GetButtonPixel(B1);
    }
    else if( InsideBB(B2.dim) ){
        o_Color = GetButtonPixel(B2);
    }
    else if( InsideBB(D1.dim)){
        o_Color = GetDecalPixel(D1);
    }
    else{
        o_Color = vec4(0.153, 0.169, 0.176, 1.0);
    }

}
