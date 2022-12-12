#version 450

in vec3 position;
in vec3 normal;
in vec2 uv;
in float is_base_triangle;

out VS_OUT {
    vec3 normal;
    vec2 uv;
    float is_base_triangle;
} vs_out;

uniform mat4 transform;

void main()
{
    //enablePrintf();
    //printf("%f %f %f | %f %f %f | %f %f | %f\n", position.x, position.y, position.z, normal.x, normal.y, normal.z, uv.x, uv.y, is_base_triangle);
    gl_Position = transform * vec4(position, 1.0);
    vs_out.normal = normal;
    vs_out.uv = uv;
    vs_out.is_base_triangle = is_base_triangle;
}
