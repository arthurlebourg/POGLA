#version 450

layout (lines) in;
layout (triangle_strip, max_vertices = 8) out;

in TCE_OUT {
    vec3 normal;
    vec2 uv;
} tce_out[];

out GS_OUT {
    vec2 uv;
} gs_out;

uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;
uniform float far = 300.0;

void main()
{
    vec4 clipSpace1 = projection_matrix * model_view_matrix * gl_in[0].gl_Position;
    vec4 clipSpace2 = projection_matrix * model_view_matrix * gl_in[1].gl_Position;
    
    vec3 ndcSpace1 = clipSpace1.xyz / clipSpace1.w;
    vec3 ndcSpace2 = clipSpace2.xyz / clipSpace2.w;

    vec2 ndcSpace1_2d = (ndcSpace1.xy + 1.0) / 2.0;
    vec2 ndcSpace2_2d = (ndcSpace2.xy + 1.0) / 2.0;

    vec2 dir = normalize(ndcSpace1_2d - ndcSpace2_2d);
    vec2 normal = vec2(-dir.y, dir.x);
    
    vec3 cam_pos = (inverse(model_view_matrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    float dist = distance(cam_pos, (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz) / 2.0);

    dist /= far;

    float coef = 2.50;

    gl_Position = clipSpace1 + vec4(normal * coef * dist, 0.0, 0.0);
    gs_out.uv = tce_out[0].uv;
    EmitVertex();

    gl_Position = clipSpace2 + vec4(normal * -coef * dist, 0.0, 0.0);
    gs_out.uv = tce_out[1].uv;
    EmitVertex();

    gl_Position = clipSpace1 + vec4(normal * -coef * dist, 0.0, 0.0);
    gs_out.uv = tce_out[0].uv;
    EmitVertex();

    gl_Position = clipSpace1 + vec4(normal * coef * dist, 0.0, 0.0);
    gs_out.uv = tce_out[0].uv;
    EmitVertex();
    
    gl_Position = clipSpace2 + vec4(normal * coef * dist, 0.0, 0.0);
    gs_out.uv = tce_out[1].uv;
    EmitVertex();

    gl_Position = clipSpace2 + vec4(normal * -coef * dist, 0.0, 0.0);
    gs_out.uv = tce_out[1].uv;
    EmitVertex();

    gl_Position = clipSpace1 + vec4(normal * coef * dist, 0.0, 0.0);
    gs_out.uv = tce_out[0].uv;
    EmitVertex();
    
    EndPrimitive();
}