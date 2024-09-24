#version 330 core

layout (location = 0) in vec3 in_uv;
layout (location = 1) in vec3 in_color;

uniform mat4 _transform;

out vec3 color;
out vec3 uv;

void main(void)
{
    gl_Position = _transform * vec4(in_uv, 1.0f);
    color = in_color;
    uv = in_uv;
}
