#version 410 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 ambientColor;
layout(location = 3) in vec3 diffuseColor;

out vec4 color;
out vec3 Normal; // vertex normal in eye coordinates 
out vec4 Position; // vertex position in eye coordinates
out vec2 TextureCoor; //texture coordinates

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void
main()
{
    gl_Position = projection * view * model * vec4(vertexPosition,1.0);
    vec3 normal = mat3(model) * vertexNormal;
    normal = normalize( normal );
    vec3 light = normalize(vec3(0.1, 0.5, 1.0 ));
    float nDotL = max( 0.0, dot( normal, light ));
    nDotL = min(1.0, nDotL);
    color = vec4( ambientColor + nDotL * diffuseColor, 0.0 );
    mat4 MVmatrix = view * model;
    Normal = normalize(vertexNormal);
    Position = vec4(vertexPosition, 1.0f);
}
