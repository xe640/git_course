#version 330 core

layout (std140) uniform sceneGlobal {
   mat4 projectionViewMat;
   mat4 viewMat;
   mat4 projMat;
};

void main()
{
   int x = gl_VertexID % 3;
   x -= 1;
   float y = 0.5 - abs(x);
   gl_Position = vec4(x * 0.5, y, 0.0, 1.0);
   gl_Position = projectionViewMat * gl_Position;
}