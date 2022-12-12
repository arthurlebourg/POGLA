#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;

void main()
{
    for (int i = 0; i < 3; i++)
    {
        gl_Position = projection_matrix * model_view_matrix * gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}