#version 450

//layout(location = 0) out float output_depth;
layout(location = 0) out vec4 output_depth;

float near = 0.1; 
float far  = 300.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{             
    output_depth = vec4(LinearizeDepth(gl_FragCoord.z) / far); // divide by far for demonstration
    //output_depth = vec4(vec3(LinearizeDepth(gl_FragCoord.z) / far), 1.0); // divide by far for demonstration
    //output_depth = vec4(vec3(gl_FragCoord.z), 1.0); // divide by far for demonstration
    //output_depth = LinearizeDepth(gl_FragCoord.z) / far;
}
