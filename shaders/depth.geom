#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in TES_OUT {
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 uv;
} tes_out[];

out GS_OUT {
    vec3 position;
    vec3 normal;
    vec3 color;
    float diffuse;
    vec2 uv;
} gs_out;

uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;
uniform float seed;

void main()
{
    for (int i = 0; i < 3; i++)
    {
        gl_Position = projection_matrix * model_view_matrix * gl_in[i].gl_Position;
        gs_out.position = tes_out[i].position;
        gs_out.normal = tes_out[i].normal;
        gs_out.color = tes_out[i].color;
        gs_out.uv = tes_out[i].uv;
        float kD = 0.5;
        vec3 normal = normalize(gs_out.normal);

        vec3 cam_pos = (inverse(model_view_matrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
        vec3 light = cam_pos + vec3(0.0, 20.0, 0.0) * seed;
        vec3 light_direction = normalize(light - gs_out.position);
        float diffuse = kD * max(dot(light_direction, normal), 0.0); 
        gs_out.diffuse = diffuse;
        EmitVertex();
    }
    EndPrimitive();
}