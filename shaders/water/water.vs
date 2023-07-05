#version 410 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec2 i_normal;
layout (location = 2) in vec2 i_texCoord;

out vec2 v_textureCoords;
out vec3 v_worldPosition;
out vec3 v_viewVector;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec3 u_cameraPosition;

void main()
{
    vec4 worldPosition = u_model * vec4(i_position, 1.0);
    vec4 clipSpace = u_projection * u_view * worldPosition;
    gl_Position = clipSpace;
    
    v_textureCoords = i_texCoord;
    v_worldPosition = worldPosition.xyz;
    v_viewVector = u_cameraPosition - worldPosition.xyz;
}