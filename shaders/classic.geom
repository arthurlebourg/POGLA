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

    vec2 seg = modelSpace1.xy - modelSpace2.xy;
    float len = length(seg);
    
    float size = 0.125 * len;
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
    return;
}