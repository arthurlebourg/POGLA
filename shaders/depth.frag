#version 450

//layout(location = 0) out float output_depth;
layout(location = 0) out vec4 output_depth;

float near = 0.1; 
float far  = 300.0; 

in GS_OUT {
    vec3 position;
    vec3 normal;
    vec3 color;
    float diffuse;
    vec2 uv;
} gs_out;

uniform sampler2D texture_sampler;
// uniform vec3 light_position = vec3(0.0, 10.0, 0.0);
uniform vec3 light_color = vec3(1.0, 1.0, 1.0);
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    float kD = 0.5;
    vec3 normal = normalize(gs_out.normal);

    // #define NB_LIGHTS 4
    // vec3 lights[NB_LIGHTS] = vec3[NB_LIGHTS](
    //     vec3(0.0, 10.0, 0.0),
    //     vec3(9.0, 10.0, 0.0),
    //     vec3(-9.0, 10.0, 0.0),
    //     vec3(10.0, 10.0, 10.0)
    // );
    
    // vec3 diffuse = vec3(0.0, 0.0, 0.0);
    // for (int i = 0; i < NB_LIGHTS; i++)
    // {
    //     vec3 light_direction = normalize(lights[i] - gs_out.position);
    //     diffuse += kD * max(dot(light_direction, normal), 0.0) * light_color; 
    // }

    vec3 white = vec3(1.0);
    vec3 threshold = vec3(0.5);

    vec3 texture_color = texture(texture_sampler, gs_out.uv).rgb;
    //vec3 color = vec3(LinearizeDepth(gl_FragCoord.z) / far); // to change
    // output_depth = vec4(texture_color * gs_out.color * diffuse, LinearizeDepth(gl_FragCoord.z) / far);
    /*
    if (length(threshold) <= length(gs_out.diffuse))
        output_depth = vec4(texture_color * gs_out.color, LinearizeDepth(gl_FragCoord.z) / far);
    else
        output_depth = vec4(white, LinearizeDepth(gl_FragCoord.z) / far);*/
    //enablePrintf();
    //printf("%f\n", gs_out.diffuse);
    //float coef = gs_out.uv.x * gs_out.uv.y * gs_out.diffuse;
    float coef = gs_out.diffuse;
    float t = coef;
    int beta = 2;
    coef = 1.0 / (1.0 + pow(t / (1.0 - t), -beta)); 
    //coef = pow(coef, 4);
    // coef = 1.0 / (1.0 + exp(-coef-0.5));
    output_depth = vec4(texture_color * gs_out.color * coef + white * (1.0 - coef), LinearizeDepth(gl_FragCoord.z) / far);
    // output_depth = vec4(texture_color * gs_out.color, LinearizeDepth(gl_FragCoord.z) / far);

    //output_depth = vec4(vec3(LinearizeDepth(gl_FragCoord.z) / far), 1.0); // divide by far for demonstration
    //output_depth = vec4(vec3(gl_FragCoord.z), 1.0); // divide by far for demonstration
    //output_depth = LinearizeDepth(gl_FragCoord.z) / far;
}
