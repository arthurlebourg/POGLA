#version 450

// put the layout for lines
layout(isolines, equal_spacing, ccw) in;

in TCS_OUT {
    vec3 normal;
    vec3 color;
    vec2 uv;
    vec3 p1;
    vec3 p2;
} tcs_out[];

out TCE_OUT {
    vec3 normal;
    vec3 color;
    vec2 uv;
    vec3 p1;
    vec3 p2;
} tce_out;

uniform float seed;
uniform mat4 model_view_matrix;
uniform float far = 300.0;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * (43758.5453));
} // random between 0 and 1

void main()
{
    vec3 n = mix(tcs_out[0].normal, tcs_out[1].normal, gl_TessCoord.x);
    tce_out.normal = n;

    vec3 c = mix(tcs_out[0].color, tcs_out[1].color, gl_TessCoord.x);
    tce_out.color = c;
    
    vec2 uv = mix(tcs_out[0].uv, tcs_out[1].uv, gl_TessCoord.x);
    tce_out.uv = uv;

    vec3 p1 = mix(tcs_out[0].p1, tcs_out[1].p1, gl_TessCoord.x);
    tce_out.p1 = p1;

    vec3 p2 = mix(tcs_out[0].p2, tcs_out[1].p2, gl_TessCoord.x);
    tce_out.p2 = p2;

    vec4 p = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);

    vec3 cam_pos = (inverse(model_view_matrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    float dist = distance(cam_pos, p.xyz);
    dist /= far;
    
    gl_Position = p + vec4(n * (0.1 + rand(uv * seed) * dist * 5.0), 0.0);
}