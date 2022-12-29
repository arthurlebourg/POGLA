#version 450

layout(quads, equal_spacing, ccw) in;

in TCS_OUT {
    vec3 normal;
    vec3 color;
    vec2 uv;
} tcs_out[];

out TES_OUT {
    vec3 normal;
    vec3 color;
    vec2 uv;
} tes_out;

uniform float seed;
uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;
uniform float far = 300.0;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * (43758.5453));
} // random between 0 and 1

void main()
{
    vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
    vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
    vec4 p = mix(p1, p2, gl_TessCoord.y);

    vec3 n1 = mix(tcs_out[0].normal, tcs_out[1].normal, gl_TessCoord.x);
    vec3 n2 = mix(tcs_out[2].normal, tcs_out[3].normal, gl_TessCoord.x);
    vec3 n = mix(n1, n2, gl_TessCoord.y);
    
    vec3 c1 = mix(tcs_out[0].color, tcs_out[1].color, gl_TessCoord.x);
    vec3 c2 = mix(tcs_out[2].color, tcs_out[3].color, gl_TessCoord.x);
    vec3 c = mix(c1, c2, gl_TessCoord.y);
    
    vec2 uv1 = mix(tcs_out[0].uv, tcs_out[1].uv, gl_TessCoord.x);
    vec2 uv2 = mix(tcs_out[2].uv, tcs_out[3].uv, gl_TessCoord.x);
    vec2 uv = mix(uv1, uv2, gl_TessCoord.y);
    
    tes_out.normal = n;
    tes_out.color = c;
    tes_out.uv = uv;

    /*vec3 cam_pos = (inverse(model_view_matrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    float dist = distance(cam_pos, p.xyz);
    dist /= far;*/

    gl_Position = p;// + vec4(n * rand(uv * seed) * dist * 5.0, 0.0);
}