#version 410 core

out vec4 fColor;


in vec2 position;
 
//Create uniform variables so dots can be spaced and scaled by user

//uniform float Scale, DotSize;

uniform sampler2D RandomTex;
uniform vec3 uColor;

//uniform vec2 PositionTextureMultiplier;
 
void main(void)
{
    float radius2;
	float Scale = 1.0f;
	float DotSize = 5.0f;
	vec2  randomXY; 
	vec3  finalcolor = uColor.xyz;

	vec2  scaledXY = position * vec2(Scale);
   

   // if(scaledXY.x < 1 && scaledXY.y < 1 && scaledXY.x > -1 && scaledXY.y > -1) {
   //   finalcolor = vec3(1, 1, 0);
   // }
    
	vec2  cell = floor(scaledXY);
	int limit = int(Scale);
    for(int i = -limit; i < limit; i++)  
        for(int j = -limit; j < limit; j++){
            if(cell.x == i && cell.y == j) {
                if(i < 0 && j < 0) {
                  finalcolor = vec3(0, 0.2 * (-i), 0.2 * (-j));
                } 
                if(i < 0 && j >= 0){
                  finalcolor = vec3(1, 0.2 * (-i), 0.2 * j);
                }
                if(i >= 0 && j < 0){
                  finalcolor = vec3(1, 0.2 * i, 0.2 * (-j));
                }
                if(i >= 0 && j >= 0) {
                  finalcolor = vec3(0, 0.2 * i, 0.2 * j);
                } 

            }
        }
    

	vec2  offset = scaledXY - cell;

	vec2  currentOffset;
	vec4  random;
 
	float priority = -1.0;

	if(false) 
	{
	for(float i = -1.0; i <= 0.0; i++)
	{
		for(float j = -1.0; j <= 0.0; j++)
		{
				vec2 currentCell = cell + vec2(i, j);
				vec2 cellOffset = offset - vec2(i, j);

				randomXY = currentCell.xy * vec2(0.037, 0.119);
				random = texture(RandomTex, randomXY);
                
                currentOffset = cellOffset - (vec2(0.5, 0.5) + vec2(random));
 
				radius2 = dot(currentOffset, currentOffset);
				if(random.w > priority && radius2 < DotSize)
				{
					finalcolor = texture(RandomTex, randomXY).xyz;
					priority = random.w;
				}
		}
	}
	}
 
	fColor = vec4(finalcolor, 1.0);
}
