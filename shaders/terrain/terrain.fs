#version 410 core
out vec4 fragmentColor;

// Uniforms pour chaque texture. Supposons que vous ayez 4 textures.
uniform sampler2D u_textureAlbedoGrass;
uniform sampler2D u_textureAlbedoRock;
uniform sampler2D u_textureAlbedoSand;
uniform sampler2D u_textureRoughnessGrass;
uniform sampler2D u_textureRoughnessRock;
uniform sampler2D u_textureRoughnessSand;
uniform sampler2D u_textureNormalGrass;
uniform sampler2D u_textureNormalRock;
uniform sampler2D u_textureNormalSand;

uniform vec3 u_lightColor;
uniform vec3 u_cameraPosition;
uniform float u_specularExponent;
uniform float u_specularIntensity;
uniform float u_ambientFactor;

in float v_height;
in vec2 v_texCoords;
in vec3 v_fragmentPosition;
in vec3 v_normalVector;
in vec3 v_tangentLightDir;
in vec3 v_tangentViewPos;
in vec3 v_tangentFragPos;

void main() 
{
	// Get diffuse texture
	vec4 rockColor = texture(u_textureAlbedoRock, v_texCoords);
	vec4 grassColor = texture(u_textureAlbedoGrass, v_texCoords);
    vec4 sandColor = texture(u_textureAlbedoSand, v_texCoords);
	
	// Get specular intensity
	float rockRoughness = texture(u_textureRoughnessRock, v_texCoords).r * 2.0 - 1.0;
    float grassRoughness = texture(u_textureRoughnessGrass, v_texCoords).r * 2.0 - 1.0;
    float sandRoughness = texture(u_textureRoughnessSand, v_texCoords).r * 2.0 - 1.0;
	
    // Adapt texturing to vertex height
	float heightRock = -15;
	float heightGrass = 10;
    float heightSand = 15;

    vec4 color;
	float roughness;
	vec3 normal;
    if (v_height < heightRock) 
	{
        color = rockColor;
        roughness = rockRoughness;
		
		// Get normal map in range [0, 1]
		normal = texture(u_textureNormalRock, v_texCoords).rgb;
    } 
	else if (v_height < heightGrass) 
	{
		float t = (v_height - heightRock) / (heightGrass - heightRock);
        color = mix(rockColor, grassColor, t);
        roughness = mix(rockRoughness, grassRoughness, t);
		
		// Get normal map in range [0, 1]
		normal = texture(u_textureNormalGrass, v_texCoords).rgb;
    } 
	else
	{
		float t = (v_height - heightGrass) / (heightSand - heightGrass);
        color = mix(grassColor, sandColor, t);
        roughness = mix(grassRoughness, sandRoughness, t);
		
		// Get normal map in range [0, 1]
		normal = texture(u_textureNormalSand, v_texCoords).rgb;
	}
	
	// Transform normal vector to range [-1, 1]
	normal = normalize(normal * 2.0 - 1.0);
	
	// Compute lighting
	vec3 lightDirection = normalize(-v_tangentLightDir);
    vec3 viewDirection = normalize(v_tangentViewPos - v_tangentFragPos);
	
	// Compute Blinn-Phong reflection model
    float diff = max(dot(lightDirection, normal), 0.0);
    vec3 halfwayDir = normalize(lightDirection + viewDirection);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), u_specularExponent);

	vec3 ambient = u_ambientFactor * color.rgb;
	vec3 diffuse = diff * color.rgb;
	vec3 specular = u_specularIntensity * spec * color.rgb;
    vec3 finalColor = (ambient + diffuse + specular) * u_lightColor;

    fragmentColor = vec4(finalColor, 1.0);
}
