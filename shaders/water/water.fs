#version 410 core

in vec2 v_textureCoords;
in vec3 v_viewVector;
in vec3 v_normal;

out vec4 fragColor;

uniform sampler2D u_reflectionTexture;
uniform sampler2D u_refractionTexture;
uniform sampler2D u_dudvMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_depthMap;

uniform vec3 u_lightPosition;
uniform vec3 u_lightColor;
uniform vec3 u_waterColor;

uniform float u_waveFactor;
uniform float u_waveStrength;
uniform float u_reflectionFactor;

const float shineDamper = 20.0;
const float reflectivity = 0.6;

void main()
{
    vec2 distortedCoords = texture(u_dudvMap, v_textureCoords).rg * 0.1;
    distortedCoords = v_textureCoords + vec2(distortedCoords.x, distortedCoords.y + u_waveFactor * u_waveStrength);
    
    vec2 refractionCoords = v_textureCoords;
    vec2 reflectionCoords = vec2(v_textureCoords.x, -v_textureCoords.y);
    
    vec4 reflectionColor = texture(u_reflectionTexture, reflectionCoords);
    vec4 refractionColor = texture(u_refractionTexture, refractionCoords);
    
    vec4 normalMapColor = texture(u_normalMap, distortedCoords);
    vec3 normal = normalize(vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0));
    
    vec3 viewVector = normalize(v_viewVector);
    vec3 reflectedLightDirection = reflect(-viewVector, normal);
    
    float refractiveFactor = pow(dot(viewVector, normal), u_reflectionFactor);
    
    vec3 lightDirection = normalize(u_lightPosition - gl_FragCoord.xyz);
    float diffuseFactor = max(dot(normal, lightDirection), 0.0);
    
    vec3 diffuseLighting = u_lightColor * diffuseFactor;
    
    vec3 reflectedLight = reflect(lightDirection, normal);
    float specular = pow(max(dot(reflectedLight, reflectedLightDirection), 0.0), shineDamper);
    vec3 specularLighting = u_lightColor * specular * reflectivity;
    
    vec3 waterColor = mix(reflectionColor.rgb, refractionColor.rgb, refractiveFactor);
    waterColor = mix(waterColor, u_waterColor, u_reflectionFactor);
    waterColor += diffuseLighting + specularLighting;
    
    fragColor = vec4(waterColor, 1.0);
}
