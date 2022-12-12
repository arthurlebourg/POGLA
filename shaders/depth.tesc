#version 450

layout(vertices = 3) out;
 
in VS_OUT {
    vec3 normal;
    vec2 uv;
    float is_base_triangle;
} vs_out[];

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = vs_out[gl_InvocationID].is_base_triangle;
        gl_TessLevelOuter[1] = vs_out[gl_InvocationID].is_base_triangle;
        gl_TessLevelOuter[2] = vs_out[gl_InvocationID].is_base_triangle;
        gl_TessLevelOuter[3] = vs_out[gl_InvocationID].is_base_triangle;

        gl_TessLevelInner[0] = vs_out[gl_InvocationID].is_base_triangle;
        gl_TessLevelInner[1] = vs_out[gl_InvocationID].is_base_triangle;
    }
}