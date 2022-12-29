#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in TES_OUT {
    vec3 normal;
    vec3 color;
    vec2 uv;
} tes_out[];

out GS_OUT {
    vec3 normal;
    vec3 color;
    vec2 uv;
} gs_out;

uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;

void main()
{
    for (int i = 0; i < 3; i++)
    {
        gl_Position = projection_matrix * model_view_matrix * gl_in[i].gl_Position;
        //tes_out[i].normal = normalize(mat3(model_view_matrix) * tes_out[i].normal);
        gs_out.normal = tes_out[i].normal;
        gs_out.color = tes_out[i].color;
        gs_out.uv = tes_out[i].uv;
        EmitVertex();
    }
    EndPrimitive();
}