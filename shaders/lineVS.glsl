#version 330 core

layout (std140) uniform sceneGlobal {
    mat4 projectionViewMat;
    mat4 viewMat;
    mat4 projMat;
    vec3 camPos;
};

uniform samplerBuffer instanceData;

out vec4 emmission;

void main()
{
    int x = gl_VertexID % 10;
    int id = gl_VertexID / 10;
    x = clamp(x - 1, 0, 7);
    bool side = (x < 4);
    float rSign = side? -1.0 : 1.0;
    bool edge = (x < 2 || x > 5);
    rSign *= edge? 1.0 : 0.0;
    float upSign = (x % 2 == 1)? 1.0 : -1.0;
    upSign *= edge? 0.7 : 1.0;

    emmission = texelFetch(instanceData, id * 4 + (side? 2 : 3));
    vec4 posSize2 = texelFetch(instanceData, id * 4 + 1);
    vec4 posSize = texelFetch(instanceData, id * 4);
    
    vec4 viewPos = viewMat * vec4(posSize.xyz, 1.0);
    vec4 viewPos2 = viewMat * vec4(posSize2.xyz, 1.0);
    vec4 projPos = projMat * viewPos;
    vec4 projPos2 = projMat * viewPos2;
    vec2 screenPos = projPos.xy / projPos.w;
    vec2 screenPos2 = projPos2.xy / projPos2.w;
    vec2 sideDir = screenPos2 - screenPos;
    sideDir = mix(normalize(sideDir), vec2(1.0 * sign(sideDir.x), 0.0), 1.0 - clamp(length(sideDir) * 256.0, 0.0, 1.0));
    sideDir *= sign(projPos.w) * sign(projPos2.w);
    vec2 upDir = vec2(-sideDir.y, sideDir.x);
    vec2 offset = sideDir * rSign + upDir * upSign;
    offset *= side? posSize.w : posSize2.w;

    vec3 pos = side? viewPos.xyz : viewPos2.xyz;
    pos += vec3(offset, 0.0);

    gl_Position = vec4(pos, 1.0);
    gl_Position = projMat * gl_Position;
}