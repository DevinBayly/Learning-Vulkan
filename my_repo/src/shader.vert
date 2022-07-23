#version 450

layout(location = 0) out vec4 fragColor;
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
// unclear whether the name has anything to do with the result
layout(push_constant) uniform MysteryName {
   mat4 mvp  ;   
} mvpPushConst;
void main() {
    gl_Position = position;
    fragColor = color;
}
