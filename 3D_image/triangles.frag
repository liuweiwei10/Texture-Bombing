#version 410 core

struct LightProperties {
bool isEnabled;
bool isLocal;
bool isSpot;
vec3 ambient;
vec3 color;
vec3 position;
vec3 halfVector;
vec3 coneDirection;
float spotCosCutoff;
float spotExponent;
float constantAttenuation; 
float linearAttenuation; 
float quadraticAttenuation;
};

const int MAXLIGHTS = 4;
uniform LightProperties Lights[MAXLIGHTS];

// material description 
uniform vec3 ambient; 
uniform vec3 diffuse; 
uniform vec3 specular; 
uniform float shininess;

uniform sampler2D randomTex;
uniform sampler2D imageTex;
uniform int istexture;

in vec3 Normal; // normal in eye coordinates
in vec4 Position; // vertex position in eye coordinates
in vec2 TextureCoor;

out vec4 FragColor;

void
main()
{
    vec3 scatteredLight = vec3(0.0); // or, to a global ambient light
    vec3 reflectedLight = vec3(0.0);
    vec3 eyeDirection;
    vec3 lightDirection; 
    vec3 halfVector;
    vec3 myNormal;
    float attenuation = 1.0f; 
    float diffuseCoeff;
    float specularCoeff;
    vec3 diffuseColor = diffuse;
    //vec3 diffuseColor = vec3(0, 0, 1);
    float alpha = 1.0f;

    //diffuseColor = vec3(texture(tex, TextureCoor));
    //if(istexture != 0) {
    //   diffuseColor = (texture(tex, TextureCoor)).rgb;
    //   alpha = (texture(tex, TextureCoor)).a;
    //}

    vec2 scale =  vec2(8.0);  
    vec2 scaledUV = TextureCoor * scale;
    vec2 cell = floor(scaledUV); 
    vec2 offset = scaledUV - cell;
    float priority = -1.0;
 
    for (int i = -1; i <= 0; i++) {

        for (int j = -1; j <= 0; j++) {
            vec2 cell_t = cell + vec2(i, j);
            vec2 offset_t = offset - vec2(i, j) ;
            vec2 randomUV = cell_t.xy * vec2(0.037, 0.119);

              vec4 random = texture(randomTex, randomUV);
              randomUV += vec2(0.03, 0.17);    

               vec4 image = texture(imageTex, offset_t - random.xy);
                
                if (image.w > 0.0 && random.w > priority) {
                    //finalcolor.xyz = random.xyz;
                    diffuseColor.xyz = image.xyz;
                    priority = random.w;
                }
        }
    }



    // loop over all the lights
    for (int light = 0; light < MAXLIGHTS; ++light) {
        if (! Lights[light].isEnabled)
            continue;
        eyeDirection = normalize(-vec3(Position)); // since we are in eye coordinates
                                                   // eye position is 0,0,0
        if (Lights[light].isLocal) {
            lightDirection = Lights[light].position - vec3(Position); 
            float lightDistance = length(lightDirection); 
            lightDirection = lightDirection / lightDistance;
            attenuation = 1.0 /
                               (Lights[light].constantAttenuation
         						+ Lights[light].linearAttenuation    * lightDistance
         						+ Lights[light].quadraticAttenuation * lightDistance
                                              						 * lightDistance);
            if (Lights[light].isSpot) {
				vec3 myConeDirection = normalize(Lights[light].coneDirection); 
				float spotCos = dot(lightDirection, -myConeDirection);
				if (spotCos < Lights[light].spotCosCutoff)
				    attenuation = 0.0; 
				else
					attenuation *= pow(spotCos, Lights[light].spotExponent); 
		    }

            halfVector = normalize(lightDirection + eyeDirection); 
        } else { // directional light
 			lightDirection = normalize(Lights[light].position);
			halfVector = normalize(lightDirection + eyeDirection); 
		}

    	myNormal = normalize(Normal);
    	diffuseCoeff = max(0.0, dot(myNormal, lightDirection)); 
    	specularCoeff = max(0.0, dot(myNormal, halfVector));
    	if (diffuseCoeff == 0.0) 
        	specularCoeff = 0.0;
    	else
        	specularCoeff = pow(specularCoeff, shininess); // * Strength;

        // Accumulate all the lights’ effects as it interacts with material properties 
        scatteredLight += Lights[light].ambient * ambient * attenuation +
                          Lights[light].color * (diffuseCoeff * diffuseColor) * attenuation; 
        reflectedLight += Lights[light].color * (specularCoeff * specular) * attenuation;
    }

    vec3 rgb = min(scatteredLight + reflectedLight, vec3(1.0));
    FragColor = vec4(rgb, alpha);

}
