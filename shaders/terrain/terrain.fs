#version 410 core
out vec4 fragmentColor;

struct Light
{
	vec3 direction;
	vec3 color;
};

// Uniforms pour chaque texture. Supposons que vous ayez 4 textures.
uniform sampler2D u_textureGrass;
uniform sampler2D u_textureRock;
uniform sampler2D u_textureSand;

uniform Light u_light;

in float v_height;
in vec2 v_texCoords;
in vec3 v_fragmentPosition;
in vec3 v_normalVector;

void main() 
{
	vec4 rockColor = texture(u_textureRock, v_texCoords);
	vec4 grassColor = texture(u_textureGrass, v_texCoords);
    vec4 sandColor = texture(u_textureSand, v_texCoords);
 
    // Définir les seuils de hauteur pour chaque texture.
	float heightRock = -15;
	float heightGrass = 10;
    float heightSand = 15;
    
    // Mélanger les textures en fonction des seuils de hauteur.
    vec4 color;
    if (v_height < heightRock) 
	{
        color = rockColor;
    } 
	else if (v_height < heightGrass) 
	{
		float t = (v_height - heightRock) / (heightGrass - heightRock);
        color = mix(rockColor, grassColor, t);
    } 
	else
	{
		float t = (v_height - heightGrass) / (heightSand - heightGrass);
		color = mix(grassColor, sandColor, t);
	}
	
	// Calcul de l'éclairage
    vec3 lightDirection = normalize(-u_light.direction);
    float lightIntensity = max(dot(lightDirection, v_normalVector), 0);
    vec3 finalColor = color.rgb * lightIntensity * u_light.color;

    fragmentColor = vec4(finalColor, 1.0);
    //fragmentColor = color;
}
