#version 410 core

out vec4 fColor;

in vec2 position;

uniform sampler2D randomTex;
uniform sampler2D imageTex;
uniform sampler2D fruitTex1;
uniform sampler2D fruitTex2;
uniform sampler2D fruitTex3;
uniform sampler2D fruitTex4;
uniform sampler2D fruitTex5;
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
    bool checkX;
    bool checkY;
    float priority = -1.0;
    int NUMBER_OF_SAMPLES = 1;
    int NUMBER_OF_IMAGES = 5;

    for (int i = -1; i <= 0; i++) {

        for (int j = -1; j <= 0; j++) {
            vec2 cell_t = cell + vec2(i, j);
            vec2 offset_t = offset - vec2(i, j) ;
            vec2 randomUV = cell_t.xy * vec2(0.037, 0.119);
            
            /*      
            vec4 random = texture(randomTex, randomUV);
            offset_t -= vec2(0.5, 0.5) + vec2(random);
            float radius2 = dot(offset_t, offset_t);
            if (random.w > priority && radius2 < 0.25) {
                finalcolor.xyz = texture(randomTex, randomUV + vec2(0.13, 0.4)).xyz;
                priority = random.w;
            } 
            */    
              
           
           for (int k = 0; k < NUMBER_OF_SAMPLES; k++) {
              vec4 random = texture(randomTex, randomUV);
              randomUV += vec2(0.03, 0.17);    

               vec4 image = texture(imageTex, offset_t - random.xy);
                
                if (image.w > 0.0 && random.w > priority) {
                    finalcolor.xyz = random.xyz;
                    //finalcolor.xyz = image.xyz;
                    priority = random.w;
                }
           }
           
           

            
            /*
            vec4 random = texture(randomTex, randomUV);

            float index = floor(random.z * NUMBER_OF_IMAGES);
            vec2 uv = offset_t.xy - random.xy;
            vec4 image;
            if(index == 0)
                image = texture(fruitTex1, uv);
            else if(index == 1)
                image = texture(fruitTex2, uv);
            else if(index == 2) 
                image = texture(fruitTex3, uv);
            else if(index == 3)
                image = texture(fruitTex4, uv);
            else 
                image = texture(fruitTex5, uv);
                
            if (image.w > 0.0 && random.w > priority) {
                    //finalcolor.xyz = random.xyz;
                    finalcolor.xyz = image.xyz;
                    priority = random.w;
            }*/

             
        }    
        
    }
  
	fColor = vec4(finalcolor, 1.0);
}
