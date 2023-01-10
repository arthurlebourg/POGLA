#version 450

in vec3 position;
in vec3 normal;
in vec2 uv;
in vec3 color;
//in float is_base_triangle;
//in float is_double_segment;

out VS_OUT {
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 uv;
    //float is_base_triangle;
    //float is_double_segment;
} vs_out;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(position, 1.0);
    vs_out.position = position;
    vs_out.normal = normal;
    vs_out.color = color;
    vs_out.uv = uv;
    //vs_out.is_base_triangle = is_base_triangle;
    //vs_out.is_double_segment = is_double_segment;
}
