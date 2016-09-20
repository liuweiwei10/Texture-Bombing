#version 410 core

out vec4 fColor;

in vec2 position;

uniform sampler2D randomTex;
uniform sampler2D imageTex;
uniform sampler2D choiceTex1;
uniform sampler2D choiceTex2;
uniform sampler2D choiceTex3;
uniform sampler2D choiceTex4;
uniform sampler2D choiceTex5;
uniform sampler2D randomSelectTex;


uniform vec3 uColor;
 
void main(void)
{
    vec2 scale =  vec2(5.0);
	vec3 finalcolor = uColor.xyz;
    finalcolor = vec3(0, 0, 1);    
	vec2 scaledUV = position * scale;
    vec2 cell = floor(scaledUV); 
    vec2 offset = scaledUV - cell;

    vec2 randomUV = cell.xy * vec2(0.037, 0.119);
    vec4 random = texture(randomTex, randomUV);

    vec4 image = texture(imageTex, offset - random.xy);
                
    if (image.w > 0.0 ) {
        finalcolor.xyz = image.xyz;
    }
  
	fColor = vec4(finalcolor, 1.0);
}
