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

    float priority = -1.0;
    int NUMBER_OF_SAMPLES = 3;

    for (int i = -1; i <= 0; i++) {

        for (int j = -1; j <= 0; j++) {
            vec2 cell_t = cell + vec2(i, j);
            vec2 offset_t = offset - vec2(i, j) ;
            vec2 randomUV = cell_t.xy * vec2(0.037, 0.119);

              
           
           for (int k = 0; k < NUMBER_OF_SAMPLES; k++) {
                vec4 random = texture(randomTex, randomUV);
                randomUV += vec2(0.03, 0.17);    

                vec4 image = texture(imageTex, offset_t - random.xy);
                
                if (image.w > 0.0 && random.z > priority) {
                    finalcolor.xyz = random.xyz;
                    priority = random.z;
                }
           }
             
        }    
        
    }
  
	fColor = vec4(finalcolor, 1.0);
}
