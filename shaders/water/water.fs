#version 410 core

in vec3 v_normal;
in vec4 v_clipspaceCoords;
in vec2 v_textureCoords;
in vec3 v_toCameraVector;

out vec4 fragColor;

uniform vec3 u_lightColor;
uniform vec3 u_lightDirection;

uniform sampler2D u_reflectionTexture;
uniform sampler2D u_refractionTexture;
uniform sampler2D u_dudvMap;
uniform sampler2D u_normalMap;

uniform vec3 u_waterColor;
uniform float u_waveLevel;
uniform float u_moveFactor;
uniform float u_refractiveFactor;
uniform float u_shineDamper;
uniform float u_reflectivity;

void main()
{   
	/*
	* Normalized device space, process projective texturing
	*/
    vec2 ndc = (v_clipspaceCoords.xy / v_clipspaceCoords.w) / 2.0 + 0.5;

	vec2 reflectionTexCoords = vec2(ndc.x, ndc.y);
	vec2 refractionTexCoords = vec2(ndc.x, -ndc.y);

	/*
	* Add distortion to texture thanks to DUDV map, add motion with "u_moveFactor"
	*/ 
	vec2 distortion1 = (texture(u_dudvMap, vec2(v_textureCoords.x + u_moveFactor, v_textureCoords.y)).rg * 2.0 - 1.0) * u_waveLevel;
	vec2 distortion2 = (texture(u_dudvMap, vec2(-v_textureCoords.x + u_moveFactor, v_textureCoords.y + u_moveFactor)).rg * 2.0 - 1.0) * u_waveLevel;
	vec2 totalDistortion = (distortion1 + distortion2);
	
	reflectionTexCoords += totalDistortion;
	reflectionTexCoords = clamp(reflectionTexCoords, 0.001, 0.999);
	
	refractionTexCoords += totalDistortion;
	refractionTexCoords.x = clamp(refractionTexCoords.x, 0.001, 0.999);
	refractionTexCoords.y = clamp(refractionTexCoords.y, -0.999, -0.001);
	
	/*
	* Get textures
	*/
	vec4 reflectionColor = texture(u_reflectionTexture, reflectionTexCoords);
	vec4 refractionColor = texture(u_refractionTexture, refractionTexCoords);
	
	/*
	* Normal map
	*/ 
	vec4 normalMapColor = texture(u_normalMap, totalDistortion);
	
	/*
	* Final colour
	*/
	fragColor = mix(refractionColor, reflectionColor, u_refractiveFactor);
	vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b * 3.0, normalMapColor.g * 2.0 - 1.0);
	normal = normalize(normal);
	
	/*
	* Fresnel effect
	*/
	vec3 viewVector = normalize(v_toCameraVector);
	float refractiveFactor = dot(viewVector, normal);
	refractiveFactor = pow(refractiveFactor, u_refractiveFactor);
	
	vec3 reflectedLight = reflect(normalize(u_lightDirection), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular, u_shineDamper);
	vec3 specularHighLights = u_lightColor * specular * u_reflectivity;
	
	/*
	* Add a color effet
	*/
	fragColor = mix(fragColor, vec4(u_waterColor, 1.0f), 0.2) + vec4(specularHighLights, 0.0);
}