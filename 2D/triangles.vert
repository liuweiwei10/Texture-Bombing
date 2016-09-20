#version 410 core

layout(location = 0) in vec4 vPosition;

out vec2 position;

void
main()
{
    position = vPosition.xy;
    gl_Position = vPosition;
}
