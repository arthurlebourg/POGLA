#version 450

in vec3 position;
in vec3 normal;
in vec2 uv;
in vec3 color;

out VS_OUT {
    vec3 normal;
    vec3 color;
    vec3 localPos;
    vec2 uv;
} vs_out;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(position, 1.0);
    vs_out.normal = normalize((transpose(inverse(transform)) * vec4(normal, 1.0f)).xyz);
    vs_out.color = color;
    vs_out.localPos = position;
    vs_out.uv = uv;
}