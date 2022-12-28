#version 450

in vec3 position;
in vec2 uv;

out VS_OUT {
    vec2 uv;
} vs_out;

uniform mat4 transform;

void main()
{
    gl_Position = vec4(position, 1.0);
    vs_out.uv = uv;
}
