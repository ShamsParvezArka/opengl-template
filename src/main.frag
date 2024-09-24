#version 330 core 
#define GRADIENT 1

in vec3 uv;
in vec3 color;

uniform float _time;

out vec4 frag_color;

void main(void)
{
    vec3 gradient = 0.5 + 0.5 * cos(_time + uv + vec3(0, 2, 4));

#if !GRADIENT
    frag_color = vec4(color, 1.0f);
#endif
#if GRADIENT
    frag_color =  vec4(gradient, 1.0f);
#endif

}