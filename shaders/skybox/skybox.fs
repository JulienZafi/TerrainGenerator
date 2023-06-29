#version 410 core
out vec4 color;

in vec3 v_texCoords;

uniform samplerCube u_skyboxTexture;

void main()
{    
    color = texture(u_skyboxTexture, v_texCoords);
}