#version 450

layout(vertices = 3) out;
 
in VS_OUT {
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 uv;
    //float is_base_triangle;
    //float is_double_segment;
} vs_out[];

uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;

out TCS_OUT {
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 uv;
} tcs_out[];

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tcs_out[gl_InvocationID].position = vs_out[gl_InvocationID].position;
    tcs_out[gl_InvocationID].normal = vs_out[gl_InvocationID].normal;
    tcs_out[gl_InvocationID].color = vs_out[gl_InvocationID].color;
    tcs_out[gl_InvocationID].uv = vs_out[gl_InvocationID].uv;
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = 1.0;
        gl_TessLevelOuter[1] = 1.0;
        gl_TessLevelOuter[2] = 1.0;
        gl_TessLevelOuter[3] = 1.0;

        gl_TessLevelInner[0] = 1.0;
        gl_TessLevelInner[1] = 1.0;
    }
}