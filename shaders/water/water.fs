#version 410 core

in vec4 v_clipSpace;
in vec2 v_texturecoords;
in vec3 v_vertexToCamVector;
in vec3 v_lightToWaterVector;

out vec4 color;

uniform sampler2D u_reflectionTexture;
uniform sampler2D u_refractionTexture;
uniform sampler2D u_dudvMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_depthMap;

uniform vec3 u_lightColour;

uniform float u_waveFactor;
uniform float u_waveStrength;
uniform float u_reflectionFactor;

const float shineDamper = 20.0f;
const float reflectivity = 0.6f;

void main() 
{
	if (v_clipSpace.w == 0.0)
    {
        // Early exit if w coordinate is zero to prevent division by zero.
        color = vec4(1.0, 0.0, 0.0, 1.0);  // Red color for debugging.
        return;
    }
	else
	{
		vec2 normalizedDeviceSpace = (v_clipSpace.xy / v_clipSpace.w) / 2.0f + 0.5f;
		vec2 refractionTextureCoords = vec2(normalizedDeviceSpace.x, normalizedDeviceSpace.y);
		vec2 reflectionTextureCoords = vec2(normalizedDeviceSpace.x, -normalizedDeviceSpace.y);
		
		// Soft Edges
		float near = 0.1f;
		float far = 1000.0f;
		float depth = texture(u_refractionTexture, refractionTextureCoords).r;
		float floorDistance = 2.0f * near * far / (far + near - (2.0 * depth - 1.0f) * (far - near));
		
		depth = gl_FragCoord.z;
		float waterDistance = 2.0f * near * far / (far + near - (2.0 * depth - 1.0f) * (far - near));
		float waterDepth = floorDistance - waterDistance;
		
		vec2 distortedCoords = texture(u_dudvMap, vec2(v_texturecoords.x + u_waveFactor, v_texturecoords.y)).rg * 0.1f;
		distortedCoords = v_texturecoords + vec2(distortedCoords.x, distortedCoords.y + u_waveFactor);
		
		vec2 totalDistortion = (texture(u_dudvMap, distortedCoords).rg * 2.0f - 1.0f) * u_waveStrength * clamp(waterDepth / 20.0f, 0.0f, 1.0f);
		
		refractionTextureCoords += totalDistortion;
		refractionTextureCoords = clamp(refractionTextureCoords, 0.001f, 0.999f);
		
		reflectionTextureCoords += totalDistortion;
		reflectionTextureCoords.x = clamp(reflectionTextureCoords.x, 0.001f, 0.999f);
		reflectionTextureCoords.y = clamp(reflectionTextureCoords.y, -0.999f, -0.001f);
		
		vec4 reflectionColour = texture(u_reflectionTexture, reflectionTextureCoords);
		vec4 refractionColour = texture(u_refractionTexture, refractionTextureCoords);
		
		vec4 normalMapColour = texture(u_normalMap, distortedCoords);
		vec3 normal = vec3(normalMapColour.r * 2.0f - 1.0f, normalMapColour.b, normalMapColour.g * 2.0f - 1.0f);
		normal = normalize(normal);
		
		vec3 viewVector = normalize(v_vertexToCamVector);
		float refractiveFactor = dot(viewVector, normal);
		refractiveFactor = pow(refractiveFactor, u_reflectionFactor);
		
		// calculate specular illumination
		vec3 reflectedLight = reflect(normalize(v_lightToWaterVector), normal);
		float specular = max(dot(reflectedLight, viewVector), 0.0f);
		specular = pow(specular, shineDamper);
		vec3 specularHighlights = u_lightColour * specular * reflectivity * clamp(waterDepth / 5.0f, 0.0f, 1.0f);;
		
		color = mix(reflectionColour, refractionColour, refractiveFactor);
		color = mix(color, vec4(0.0f, 0.3f, 0.5f, 1.0f), 0.2f) + vec4(specularHighlights, 0.0f);
		color.a = clamp(waterDepth / 5.0f, 0.0f, 1.0f);
	}
}