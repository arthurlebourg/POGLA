#version 450

in GS_OUT {
    vec2 uv;
} gs_out;

layout(location = 0) out vec4 output_color;

uniform sampler2D texture_sampler;
uniform sampler2D depth_sampler;
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
    //enablePrintf();
    printf("textureDepth: %f     Depth: %f\n", texture(depth_sampler, gl_FragCoord.xy).r, gl_FragCoord.z);
    //output_color = vec4(vec3(texture(depth_sampler, gl_FragCoord.xy / viewSize).r), 1.0);
    //output_color = texture(texture_sampler, gs_out.uv);

    float depth = LinearizeDepth(gl_FragCoord.z);
    output_color = vec4(vec3(depth), 1.0);

    /*if (gl_FragCoord.z < texture(depth_sampler, gl_FragCoord.xy).r)
        discard;
    
    
    
    output_color = texture(texture_sampler, gs_out.uv);*/
}
