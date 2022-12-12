#version 450

layout(quads, equal_spacing, ccw) in;

void main()
{
    vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
    vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
    vec4 p = mix(p1, p2, gl_TessCoord.y);

    //enablePrintf();
    printf("p = %f %f %f %f\n", p.x, p.y, p.z, p.w);
    
    gl_Position = p;
}