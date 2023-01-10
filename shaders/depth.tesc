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
        /*vec4 clipSpace1 = projection_matrix * model_view_matrix * gl_in[0].gl_Position;
        vec4 clipSpace2 = projection_matrix * model_view_matrix * gl_in[1].gl_Position;

        if (clipSpace1.w == 0.0 || clipSpace2.w == 0.0)
        {
            gl_TessLevelOuter[0] = 0.0;
            return;
        }

        vec3 ndcSpace1 = clipSpace1.xyz / clipSpace1.w;
        vec3 ndcSpace2 = clipSpace2.xyz / clipSpace2.w;
        vec2 ndcSpace1_2d = clamp(ndcSpace1.xy, -1.0, 1.0);
        vec2 ndcSpace2_2d = clamp(ndcSpace2.xy, -1.0, 1.0);

        float dist = distance(ndcSpace1_2d, ndcSpace2_2d);
        
        gl_TessLevelOuter[0] = vs_out[gl_InvocationID].is_base_triangle;// * dist;
        gl_TessLevelOuter[1] = vs_out[gl_InvocationID].is_base_triangle;// * dist;
        gl_TessLevelOuter[2] = vs_out[gl_InvocationID].is_base_triangle;// * dist;
        gl_TessLevelOuter[3] = vs_out[gl_InvocationID].is_base_triangle;// * dist;

        gl_TessLevelInner[0] = vs_out[gl_InvocationID].is_base_triangle;// * dist;
        gl_TessLevelInner[1] = vs_out[gl_InvocationID].is_base_triangle;// * dist;*/
        gl_TessLevelOuter[0] = 1.0;
        gl_TessLevelOuter[1] = 1.0;
        gl_TessLevelOuter[2] = 1.0;
        gl_TessLevelOuter[3] = 1.0;

        gl_TessLevelInner[0] = 1.0;
        gl_TessLevelInner[1] = 1.0;
    }
}