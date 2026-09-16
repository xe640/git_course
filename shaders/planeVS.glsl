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
    float rSign = (x % 2 == 1)? 1.0 : -1.0;
    float upSign = (x < 2)? 1.0 : -1.0;

    emmission = texelFetch(instanceData, id * 3 + 2);
    vec3 normal = texelFetch(instanceData, id * 3 + 1).xyz;
    vec4 posSize = texelFetch(instanceData, id * 3);
    vec3 up, right;
    vec3 referenceAxis = vec3(0.0, 1.0, 0.0);

    if (abs(normal.y) > 0.9) {
        referenceAxis = vec3(0.0, 0.0, 1.0);
    }

    right = normalize(cross(referenceAxis, normal));
    up = normalize(cross(normal, right));

    vec3 pos = (up * upSign + right * rSign) * posSize.w * 0.5 + posSize.xyz;

    gl_Position = vec4(pos, 1.0);
    gl_Position = projectionViewMat * gl_Position;
}