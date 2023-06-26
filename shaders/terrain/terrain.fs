#version 410 core
out vec4 fragmentColor;

// Uniforms pour chaque texture. Supposons que vous ayez 4 textures.
uniform sampler2D u_textureGrass;
uniform sampler2D u_textureRock;
uniform sampler2D u_textureSand;
uniform sampler2D u_textureSnow;
uniform sampler2D u_textureWater;

// Passer les informations sur la hauteur.
in float v_height;

// Interpolation des coordonnées de texture depuis le vertex shader.
in vec2 v_texCoords;

void main() {
    // Obtenir la couleur de chaque texture.
    vec4 color1 = texture(u_textureSand, v_texCoords);
    vec4 color2 = texture(u_textureGrass, v_texCoords);
    vec4 color3 = texture(u_textureRock, v_texCoords);
    vec4 color4 = texture(u_textureSnow, v_texCoords);
    //vec4 color5 = texture(u_textureWater, v_texCoords);

    // Définir les seuils de hauteur pour chaque texture.
	float heightSand = -10;
    float heightGrass = 10;
    float heightRock = 15;
    float heightSnow = 30;

    // Mélanger les textures en fonction des seuils de hauteur.
    vec4 color;
    if (v_height < heightSand) 
	{
        color = color1;
    } 
	else if (v_height < heightGrass) 
	{
        color = mix(color1, color2, (v_height - heightSand) / (heightGrass - heightSand));
    } 
	else if (v_height < heightRock) 
	{
       color = mix(color2, color3, (v_height - heightGrass) / (heightRock - heightGrass));
	}
	else
	{
        color = mix(color3, color4, (v_height - heightRock) / (heightSnow - heightRock));
    }
    //} else if (v_height < height4) {
    //    color = mix(color3, color4, (v_height - height3) / (height4 - height3));
    //} else {
     //   color = mix(color4, color5, (v_height - height4) / (1.0 - height4));
    //}

    fragmentColor = color;
}
