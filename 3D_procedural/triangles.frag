#version 410 core

in vec4 color;
in vec4 Position;
in vec3 Normal; 

out vec4 fColor;

uniform sampler2D randomTex;

void main(void)
{
    vec3 scale =  vec3(3.0);
	vec3 finalcolor = color.xyz;
	vec3 scaledUV = Position.xyz * scale;
    vec3 cell = floor(scaledUV); 
    vec3 offset = scaledUV - cell;
    float priority = -1.0;

    for (int i = -1; i <= 0; i++) {
        for (int j = -1; j <= 0; j++) {
            for (int k = -1; k <= 0; k++) {
                
                vec3 cell_t = cell + vec3(i, j, k);
                vec3 offset_t = offset - vec3(i, j, k);
                vec2 randomUV = cell_t.xy * vec2(0.037, 0.119) +
                   cell_t.z * 0.003;

            
                vec4 random = texture(randomTex, randomUV);
                if(random.x==0 && random.y == 0 ) {
                    finalcolor = vec3(1, 0, 0);

                } else {
                vec3 offset_t3 = offset_t - (vec3(0.5, 0.5, 0.5) + vec3(random));
                float radius2 = dot(offset_t3, offset_t3);

                if (random.w > priority && radius2 < 0.2) {
                    //finalcolor = vec3(1, 0, 1);
                    finalcolor = texture(randomTex, randomUV + vec2(0.13, 0.4)).xyz;
                    priority = random.w;

                }                
                }
            }
        }
    }
float factor = dot(Normal.xyz, vec3(0.5, 0.75, -0.3)) * 0.6 + 0.5;
finalcolor.xyz = finalcolor.xyz * factor;
  
fColor = vec4(finalcolor, 1.0);
}