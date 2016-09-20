#version 410 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTextureIndices;

out vec3 Normal; // vertex normal in eye coordinates 
out vec4 Position; // vertex position in eye coordinates
out vec2 TextureCoor; //texture coordinates

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void
main()
{
    mat4 MVmatrix = view * model;
    Normal = normalize(mat3(MVmatrix)  * vertexNormal);
    Position = MVmatrix * vec4(vertexPosition, 1.0f);
    TextureCoor = vertexTextureIndices;
    gl_Position = projection * view * model * vec4(vertexPosition,1.0);
}
