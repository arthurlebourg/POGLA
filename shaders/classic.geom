#version 450

layout (lines) in;
// layout (line_strip, max_vertices = 2) out;
layout(triangle_strip, max_vertices = 4) out;

in TCE_OUT {
    vec3 normal;
    vec3 color;
    vec2 uv;
} tce_out[];

out GS_OUT {
    vec3 color;
    vec2 uv;
} gs_out;

uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;
uniform float far = 300.0;

void main()
{
    vec4 modelSpace1 = model_view_matrix * gl_in[0].gl_Position;

    vec4 modelSpace2 = model_view_matrix * gl_in[1].gl_Position;

    // int beta = 2;
    // dist = 1.0 / (1.0 + pow((dist + 1e-6) / (1.0 + 1e-6 - dist), beta)); 
    
    // size *= dist;
    
    vec2 seg = modelSpace1.xy - modelSpace2.xy;
    float len = length(seg);
    
    //float size = 0.125 * len * pow(1.0 - dist, 2.0);
    float size = 0.125 * len;
    //get perpendicular vector
    vec2 perpendicular = normalize(vec2(-seg.y, seg.x));
    
    vec2 va = modelSpace1.xy + perpendicular * -size;
    gl_Position = projection_matrix * vec4(va, modelSpace1.z, 1.0);
    gs_out.color = tce_out[0].color;
    gs_out.uv = tce_out[0].uv;
    EmitVertex();

    vec2 vd = modelSpace2.xy + perpendicular * -size;
    gl_Position = projection_matrix * vec4(vd, modelSpace2.z, 1.0);
    gs_out.color = tce_out[1].color;
    gs_out.uv = tce_out[1].uv;
    EmitVertex();

    vec2 vb = modelSpace1.xy + perpendicular * size;
    gl_Position = projection_matrix * vec4(vb, modelSpace1.z, 1.0);
    gs_out.color = tce_out[0].color;
    gs_out.uv = tce_out[0].uv;
    EmitVertex();
    
    vec2 vc = modelSpace2.xy + perpendicular * size;
    gl_Position = projection_matrix * vec4(vc, modelSpace2.z, 1.0);
    gs_out.color = tce_out[1].color;
    gs_out.uv = tce_out[1].uv;
    EmitVertex();

    EndPrimitive();

    // vec4 clipSpace1 = projection_matrix * model_view_matrix * gl_in[0].gl_Position;
    // vec4 clipSpace2 = projection_matrix * model_view_matrix * gl_in[1].gl_Position;
    // gl_Position = clipSpace1;
    // gs_out.color = tce_out[0].color;
    // gs_out.uv = tce_out[0].uv;
    // EmitVertex();

    // gl_Position = clipSpace2;
    // gs_out.color = tce_out[1].color;
    // gs_out.uv = tce_out[1].uv;
    // EmitVertex();
    // EndPrimitive();
    
    return;
    /*
    
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
    gs_out.color = tce_out[0].color;
    gs_out.uv = tce_out[0].uv;
    EmitVertex();

    gl_Position = clipSpace2 + vec4(normal * -coef * dist, 0.0, 0.0);
    gs_out.color = tce_out[1].color;
    gs_out.uv = tce_out[1].uv;
    EmitVertex();

    gl_Position = clipSpace1 + vec4(normal * -coef * dist, 0.0, 0.0);
    gs_out.color = tce_out[0].color;
    gs_out.uv = tce_out[0].uv;
    EmitVertex();

    gl_Position = clipSpace1 + vec4(normal * coef * dist, 0.0, 0.0);
    gs_out.color = tce_out[0].color;
    gs_out.uv = tce_out[0].uv;
    EmitVertex();
    
    gl_Position = clipSpace2 + vec4(normal * coef * dist, 0.0, 0.0);
    gs_out.color = tce_out[1].color;
    gs_out.uv = tce_out[1].uv;
    EmitVertex();

    gl_Position = clipSpace2 + vec4(normal * -coef * dist, 0.0, 0.0);
    gs_out.color = tce_out[1].color;
    gs_out.uv = tce_out[1].uv;
    EmitVertex();

    gl_Position = clipSpace1 + vec4(normal * coef * dist, 0.0, 0.0);
    gs_out.color = tce_out[0].color;
    gs_out.uv = tce_out[0].uv;
    EmitVertex();
    
    EndPrimitive();
    */
}