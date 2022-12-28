#version 450

in VS_OUT{
    vec2 uv;
} gs_out;

layout(location = 0) out vec4 output_color;

uniform sampler2D depth_sampler;
uniform sampler2D lines_sampler;

void main()
{

    vec4 texture_depth = texture(depth_sampler, gs_out.uv);
    vec4 texture_lines = texture(lines_sampler, gs_out.uv);

    if (texture_lines.a < texture_depth.a)
    {
        output_color = texture_lines;
    }
    else
    {
        output_color = texture_depth;
    }
        
    //output_color = texture(depth_sampler, gs_out.uv) * texture(lines_sampler, gs_out.uv);

}
