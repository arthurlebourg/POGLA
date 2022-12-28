#version 450

in GS_OUT {
    vec3 color;
    vec2 uv;
} gs_out;

layout(location = 0) out vec4 output_color;

uniform sampler2D texture_sampler;
//uniform sampler2D depth_sampler;
uniform vec2 viewSize;

float near = 0.1; 
float far  = 300.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    float shader_depth = LinearizeDepth(gl_FragCoord.z) / far;
    output_color = vec4(gs_out.color, shader_depth) * texture(texture_sampler, gs_out.uv);
}
