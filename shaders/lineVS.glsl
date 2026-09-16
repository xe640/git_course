#version 330 core

layout (std140) uniform sceneGlobal {
    mat4 projectionViewMat;
    mat4 viewMat;
    mat4 projMat;
};

uniform samplerBuffer instanceData;

out vec4 emmission;

void main()
{
    int x = gl_VertexID % 6;
    int id = gl_VertexID / 6;
    x = clamp(x - 1, 0, 3);
    bool side = (x % 2 == 1);
    float rSign = side? -1.0 : 1.0;
    float upSign = (x < 2)? 1.0 : -1.0;

    emmission = texelFetch(instanceData, id * 4 + (side? 2 : 3));
    vec4 posSize2 = texelFetch(instanceData, id * 4 + 1);
    vec4 posSize = texelFetch(instanceData, id * 4);
    
    vec4 viewPos = viewMat * vec4(posSize.xyz, 1.0);
    vec4 viewPos2 = viewMat * vec4(posSize2.xyz, 1.0);
    vec2 sideDir = normalize(viewPos2.xy - viewPos.xy);
    vec2 upDir = vec2(-sideDir.y, sideDir.x);
    vec2 offset = sideDir * rSign + upDir * upSign;
    offset *= side? posSize.w : posSize2.w;

    vec3 pos = side? viewPos.xyz : viewPos2.xyz;
    pos += vec3(offset, 0.0);

    gl_Position = vec4(pos, 1.0);
    gl_Position = projMat * gl_Position;
}