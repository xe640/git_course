#version 330 core 

out vec4 FragColor;

in vec4 emmission;

void main()
{
    FragColor = emmission;
}