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
    finalcolor = vec3(1, 1, 1);    
	vec2 scaledUV = position * scale;
    vec2 cell = floor(scaledUV); 
    vec2 offset = scaledUV - cell;
    float priority = -1.0;

    for (int i = -1; i <= 0; i++) {

        for (int j = -1; j <= 0; j++) {
            vec2 cell_t = cell + vec2(i, j);
            vec2 offset_t = offset - vec2(i, j) ;
            vec2 randomUV = cell_t.xy * vec2(0.037, 0.119);
                              
            vec4 random = texture(randomTex, randomUV);
            offset_t -= vec2(0.5, 0.5) + vec2(random);
            float radius2 = dot(offset_t, offset_t);
            if (random.z > priority && radius2 < 0.25) {
                finalcolor.xyz = texture(randomTex, randomUV + vec2(0.13, 0.4)).xyz;
                priority = random.z;
            } 
                               
        }
    }
  
	fColor = vec4(finalcolor, 1.0);
}
