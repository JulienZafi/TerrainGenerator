#version 410 core

in vec2 v_texCoords;

out vec4 color;

uniform sampler2D u_reflectionTexture;
uniform sampler2D u_refractionTexture;

void main() 
{
	vec4 reflectionColour = texture(u_reflectionTexture, v_texCoords);
	vec4 refractionColour = texture(u_refractionTexture, v_texCoords);
	
	color = mix(reflectionColour, refractionColour, 0.5f);
}