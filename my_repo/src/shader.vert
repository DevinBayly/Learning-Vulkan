#version 450

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
// unclear whether the name has anything to do with the result
layout(push_constant) uniform MysteryName {
   mat4 mvp  ;   
} mvpPushConst;

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
