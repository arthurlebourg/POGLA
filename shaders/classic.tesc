#version 450

layout(vertices = 2) out;

in VS_OUT {
    vec3 normal;
    vec3 color;
    vec2 uv;
} vs_out[]; // 3 is the adjencent vertex, segment is 0 1

out TCS_OUT {
    vec3 normal;
    vec3 color;
    vec2 uv;
} tcs_out[];

uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;
uniform float far = 300.0;

void main()
{
    tcs_out[gl_InvocationID].normal = vs_out[gl_InvocationID].normal;
    tcs_out[gl_InvocationID].uv = vs_out[gl_InvocationID].uv;
    tcs_out[gl_InvocationID].color = vs_out[gl_InvocationID].color;
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    if (gl_InvocationID == 0)
    {
        vec3 center_main = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz) / 3.0;
        vec3 center_adj = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[3].gl_Position.xyz) / 3.0;

        
        vec3 cam_pos = (inverse(model_view_matrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz;

        vec3 mid_segment = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz) / 2.0;
        vec3 line = normalize(gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz);
        vec3 cam_ray = normalize(mid_segment - cam_pos);

        vec3 side = normalize(cross(cam_ray, line));
        
        vec3 adj_to_center = normalize(center_adj - mid_segment);
        vec3 main_to_center = normalize(center_main - mid_segment);

        float dot_adj = dot(adj_to_center, side);
        float dot_main = dot(main_to_center, side);
        //enablePrintf();
        //printf("dot_adj: %f dot_main: %f mult: %f\n", dot_adj, dot_main, dot_adj * dot_main);

        /*if (dot_adj * dot_main < 0.0)
        {
            gl_TessLevelOuter[0] = 0.0; // discards the patch
            return;
        }*/


        // https://math.stackexchange.com/questions/214187/point-on-the-left-or-right-side-of-a-plane-in-3d-space

        // does exactly the same as the above code, but with a matrix, mdr j ai été trop bon
        vec3 A = cam_pos;
        vec3 B = gl_in[0].gl_Position.xyz;
        vec3 C = gl_in[1].gl_Position.xyz;
        vec3 X = center_main;
        vec3 X2 = center_adj;

        vec3 B_prime = B - A;
        vec3 C_prime = C - A;
        vec3 X_prime = X - A;
        vec3 X2_prime = X2 - A;

        //make 3x3 matrix from B_prime, C_prime, X_prime
        mat3 M1 = mat3(B_prime, C_prime, X_prime);
        mat3 M2 = mat3(B_prime, C_prime, X2_prime);

        //get the determinant of the matrix
        float det1 = determinant(M1);
        float det2 = determinant(M2);

        if (det1 * det2 < 0.0)
        {
            gl_TessLevelOuter[0] = 0.0; // discards the patch
            return;
        }
        
        float dist = distance(cam_pos, mid_segment);
        dist /= far;

        gl_TessLevelOuter[0] = max(dist * 10.0, 1.0);
        gl_TessLevelOuter[1] = max(dist * 10.0, 1.0);
    }
}