#version 450

layout(vertices = 2) out;

in VS_OUT {
    vec3 normal;
    vec2 uv;
    float is_base_triangle;
} vs_out[]; // 3 is the adjencent vertex, segment is 0 1

out TCS_OUT {
    vec3 normal;
    vec2 uv;
    float segment_length;
} tcs_out[];

uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;

void main()
{
    tcs_out[gl_InvocationID].normal = vs_out[gl_InvocationID].normal;
    tcs_out[gl_InvocationID].uv = vs_out[gl_InvocationID].uv;
    tcs_out[gl_InvocationID].segment_length = 1.0;
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    vec3 normal_main = normalize(vs_out[0].normal + vs_out[1].normal + vs_out[2].normal);
    vec3 normal_adj = normalize(/*vs_out[0].normal + vs_out[1].normal + */vs_out[3].normal);

    vec3 center_main = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz) / 3.0;
    vec3 center_adj = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[3].gl_Position.xyz) / 3.0;
    
    vec3 cam_pos = (inverse(model_view_matrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz;

    float dot_main = dot(normalize(cam_pos - center_main), normal_main);
    float dot_adj = dot(normal_adj, normalize(cam_pos - center_adj));
    if (dot_main * dot_adj > 0)
    {
        gl_TessLevelOuter[0] = 0.0; // discards the patch
        return;
    }
    vec4 clipSpace1 = projection_matrix * model_view_matrix * gl_in[0].gl_Position;
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
    tcs_out[gl_InvocationID].segment_length = dist;

    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = max(dist * 10.0, 1.0);
        gl_TessLevelOuter[1] = max(dist * 10.0, 1.0);
    }
}