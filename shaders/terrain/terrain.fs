#version 410 core
out vec4 fragmentColor;

// Uniforms pour chaque texture. Supposons que vous ayez 4 textures.
uniform sampler2D u_textureGrass;
uniform sampler2D u_textureRock;
uniform sampler2D u_textureShoreline;
uniform sampler2D u_textureSnow;

// Passer les informations sur la hauteur.
in float v_height;

// Interpolation des coordonnées de texture depuis le vertex shader.
in vec2 v_texCoords;

void main() {
    // Obtenir la couleur de chaque texture.
    vec4 color1 = texture(u_textureGrass, v_texCoords);
    vec4 color2 = texture(u_textureRock, v_texCoords);
    vec4 color3 = texture(u_textureShoreline, v_texCoords);
    vec4 color4 = texture(u_textureSnow, v_texCoords);

    // Définir les seuils de hauteur pour chaque texture.
    float height1 = 0.25;
    float height2 = 0.5;
    float height3 = 0.75;

    // Mélanger les textures en fonction des seuils de hauteur.
    vec4 color;
    //if (v_height < height1) {
        color = color1;
    //} else if (v_height < height2) {
    //    color = mix(color1, color2, (v_height - height1) / (height2 - height1));
    //} else if (v_height < height3) {
    //    color = mix(color2, color3, (v_height - height2) / (height3 - height2));
    //} else {
    //    color = mix(color3, color4, (v_height - height3) / (1.0 - height3));
    //}

    fragmentColor = color;
}
